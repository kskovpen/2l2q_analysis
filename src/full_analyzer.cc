
//Include C++ libraries
#include <iostream>
#include <fstream>

//Include header for this class
#include "../interface/full_analyzer.h"


//Include ROOT header files
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TSystem.h>

using namespace std;

//  run_over_file		: This is the main function to loop over events of a certain file, it does the main event selection and delegates to other functions
//  'filename' is the file containing the events over which we will run
//  'cross_section' is the cross section of the process
//  'max_entries' is the maximal entry over which must be ran, if its 50000, then we will run over the first 50000 events, if its -1, all events will be ran
//  'partition' is the number of jobs that the file will be split in, if 5, then the code knows it is 1 of 5 jobs running over this file
//  'partitionjobnumber' is the number of the particular job in the partition, so if it's 3 and partition is 5, then this code will run over the third fraction of events if they are divided in 5 fractions
void full_analyzer::run_over_file(TString filename, double cross_section, int max_entries, int partition, int partitionjobnumber)
{
// Short description of program flow:
//     - initialize file
//     - initialize histograms
//     - start loop over events
//     - construct booleans for object selection? should be done at ntuplizer level, but all used variables should be included too
//     - functions for every signal region event selection


    sampleflavor = "bkg";
    if(filename.Index("_e_") != -1) sampleflavor = "e";
    else if(filename.Index("_mu_") != -1) sampleflavor = "mu";

    TString promptordisplaced = "";
    if(filename.Index("prompt") != -1) promptordisplaced = "prompt";
    else if(filename.Index("displaced") != -1) promptordisplaced = "displaced";

    TFile *input = new TFile(filename, "open");
    TTree *tree  = (TTree*) input->Get("blackJackAndHookers/blackJackAndHookersTree");
    double total_weight = (cross_section * 35900) / ((TH1F*) input->Get("blackJackAndHookers/hCounter"))->GetBinContent(1); // 35900 is in inverse picobarn, because cross_section is given in picobarn
    
    _gen_Nmass = ((TString)filename(filename.Index("_M-") + 3, filename.Index("_V-") - filename.Index("_M-") - 3)).Atof();

    //TH1F* hweight = (TH1F*) input->Get("blackJackAndHookers/hCounter");
    //hweight->Scale(hweight->GetBinContent(1) / (cross_section * 35900)); //this is the inverted weight!!! since hadd needs to be able to sum up the weights!
    
    Init(tree);

    //This map contains all 1D histograms
    std::map<TString, TH1*> hists;
    std::map<TString, TH2*> hists2D;

    init_HLT_efficiency(&hists, "Beforeptcut");//found in src/HLT_eff.cc, does everything HLT efficiency related
    init_HLT_efficiency(&hists, "Afterptcut");//found in src/HLT_eff.cc, does everything HLT efficiency related
    init_HLT_allevents_efficiency(&hists, "");
    init_HNL_MC_check(&hists, &hists2D);

    for(const TString &lep_region : {"_OS_e", "_SS_e", "_OS_mu", "_SS_mu"}){
        for(const TString &ev_region : {"", "_beforereliso", "_before1jet", "_afterdispl", "_endofselection"}){
            add_histograms(&hists, &hists2D, lep_region + ev_region);
        }
    }

    //assures statistical errors are dealt with correctly
    for( it = hists.begin(); it != hists.end(); it++){
        TH1* h = it->second;
        h->Sumw2();
    }
    for( it2D = hists2D.begin(); it2D != hists2D.end(); it2D++){
        TH2* h = it2D->second;
        h->Sumw2();
    }
   
    HNLtagger hnltagger_e(filename, "HNLtagger_electron", partition, partitionjobnumber);
    HNLtagger hnltagger_mu(filename, "HNLtagger_muon", partition, partitionjobnumber);
    HNLtagger hnltagger_gen_e(filename, "HNLtagger_gen_electron", partition, partitionjobnumber);
    HNLtagger hnltagger_gen_mu(filename, "HNLtagger_gen_muon", partition, partitionjobnumber);

    PFNReader pfn_mu("/user/bvermass/heavyNeutrino/Dileptonprompt/CMSSW_10_2_14/src/deepLearning/jetTagger.h5", {50,11}, 2);
    PFNReader pfn_e("/user/bvermass/heavyNeutrino/Dileptonprompt/CMSSW_10_2_14/src/deepLearning/jetTagger.h5", {50,11}, 2);

    //these were meant to test cut flow selection, maybe should make these into histograms eventually
    int SSe = 0, SSe2 = 0, SSe3 = 0;
    int OSe = 0, OSe2 = 0, OSe3 = 0;
    int SSmu = 0, SSmu2 = 0, SSmu3 = 0;
    int OSmu = 0, OSmu2 = 0, OSmu3 = 0;
    int SSe_weight = 0, SSe2_weight = 0, SSe3_weight = 0;
    int OSe_weight = 0, OSe2_weight = 0, OSe3_weight = 0;
    int SSmu_weight = 0, SSmu2_weight = 0, SSmu3_weight = 0;
    int OSmu_weight = 0, OSmu2_weight = 0, OSmu3_weight = 0;

    int count = 0;

    // Determine range of events to loop over
    Long64_t nentries = tree->GetEntries();
    //cout << "full_analyzer.cc file: " << filename << endl;
    //cout << "Number of events: " << nentries << endl;
    if(max_entries == -1 || max_entries > nentries) max_entries = nentries;
    total_weight = 1.0 * nentries / max_entries * total_weight; //Correct weight for the amount of events that is actually ran (if only 20.000 of 100.000 are ran, then each event should have 5 times its original weight)
    //hweight->Scale(hweight->GetBinContent(1) * nentries / max_entries);
    
    Long64_t j_begin = floor(1.0 * max_entries * partitionjobnumber / partition);
    Long64_t j_end   = floor(1.0 * max_entries * (partitionjobnumber + 1) / partition);
    unsigned notice = ceil(0.01 * (j_end - j_begin) / 20) * 100;
    cout << "j_begin j_end max_entries: " << j_begin << " " << j_end << " " << max_entries << endl;
    if(j_end - j_begin > 2000000) cout << "More than 2million events to run over! Increase partition (" << j_end - j_begin << ")" << endl;

    //main loop begins here
    for(unsigned jentry = j_begin; jentry < j_end; ++jentry){
	    tree->GetEntry(jentry);
	    bool printevent = ((jentry -j_begin)%notice == 0);
	    if(printevent){
	        cout << jentry - j_begin << " of " << j_end - j_begin << endl;
	    }


        //Calculate Event weight
        event_weight = _weight;

        //Get ID
        get_electronID(&fullElectronID[0]);
        get_loose_electronID(&looseElectronID[0]);
        get_pogmedium_electronID(&pogmediumElectronID[0]);
        get_displ_electronID(&displElectronID[0]);
        get_muonID(&fullMuonID[0]);
        get_loose_muonID(&looseMuonID[0]);
        get_pogmedium_muonID(&pogmediumMuonID[0]);
        get_displ_muonID(&displMuonID[0]);
        get_jetID(&fullJetID[0]);


        //Get Cleaning for jets
	    get_clean_jets(&jet_clean_full[0],   &fullElectronID[0], &fullMuonID[0]);
	    get_clean_jets(&jet_clean_displ[0],  &displElectronID[0], &displMuonID[0]);
        get_clean_jets(&jet_clean_loose[0],  &looseElectronID[0], &looseMuonID[0]);
	    for(unsigned i = 0; i < 20; ++i){
	        jet_clean_full_displ[i] = jet_clean_full[i] && jet_clean_displ[i];
	    }
	    
        //Get cleaning for electrons
	    get_clean_ele(&ele_clean_full[0],   &fullMuonID[0]);
	    get_clean_ele(&ele_clean_displ[0],  &displMuonID[0]);
        get_clean_ele(&ele_clean_loose[0], &looseMuonID[0]);
	    for(unsigned i = 0; i < 10; ++i){
	        ele_clean_full_displ[i] = ele_clean_full[i] && ele_clean_displ[i];
	    }

        //Find leptons and jets with leading pt
	    i_leading_e     		    = find_leading_e(&fullElectronID[0], &ele_clean_full_displ[0]);
	    i_subleading_e  		    = find_subleading_e(&fullElectronID[0], &ele_clean_full_displ[0], i_leading_e);
	    i_subleading_displ_e  	    = find_subleading_e(&displElectronID[0], &ele_clean_full_displ[0], i_leading_e);
        i_leading_pogmedium_e       = find_leading_e(&pogmediumElectronID[0], &ele_clean_full_displ[0]);
	    i_leading_mu    		    = find_leading_mu(&fullMuonID[0]);
	    i_subleading_mu 		    = find_subleading_mu(&fullMuonID[0], i_leading_mu);
	    i_subleading_displ_mu 	    = find_subleading_mu(&displMuonID[0], i_leading_mu);
        i_leading_pogmedium_mu      = find_leading_mu(&pogmediumMuonID[0]);
	    
	    i_leading_jet	    = find_leading_jet(&fullJetID[0], &jet_clean_full_displ[0]);
	    i_subleading_jet	= find_subleading_jet(&fullJetID[0], &jet_clean_full_displ[0], i_leading_jet);
        i_thirdleading_jet  = find_thirdleading_jet(&fullJetID[0], &jet_clean_full_displ[0], i_leading_jet, i_subleading_jet);

        // Trigger efficiency of all events (before selection matches 
        fill_HLT_allevents_efficiency(&hists, "");

        //make sure we are either in e or mu signal region. Based on leading lepton with highest pt.
        if(i_leading_e != -1 and i_leading_mu != -1){
            if(_lPt[i_leading_e] > _lPt[i_leading_mu]) i_leading_mu = -1;
            else i_leading_e = -1;
        }

        //get signal region booleans
        signal_regions();
        
        // Find gen level leptons matching to the leading and subleading lepton (geometrically when its background or data, pgdID when its signal
        i_gen_leading_e             = find_gen_lep(i_leading_e);                //finds closest dR match
        i_gen_subleading_displ_e    = find_gen_lep(i_subleading_displ_e);
        i_gen_leading_mu            = find_gen_lep(i_leading_mu);
        i_gen_subleading_displ_mu   = find_gen_lep(i_subleading_displ_mu);
        
        // Find jet closest to l2, this jet can contain l2
        if(_1e1disple)   i_closel2_jet = find_jet_closest_to_lepton(&fullJetID[0], i_subleading_displ_e);
        if(_1mu1displmu) i_closel2_jet = find_jet_closest_to_lepton(&fullJetID[0], i_subleading_displ_mu);
        
        find_gen_l1_and_l2();                                                   //finds HNL process l1 and l2 gen leptons
        if(_1e)          leadingIsl1 = leptonIsGenLepton(i_leading_e, i_gen_l1);
        if(_1mu)         leadingIsl1 = leptonIsGenLepton(i_leading_mu, i_gen_l1);
        if(_1e1disple)   subleadingIsl2 = leptonIsGenLepton(i_subleading_displ_e, i_gen_l2);                
        if(_1mu1displmu) subleadingIsl2 = leptonIsGenLepton(i_subleading_displ_mu, i_gen_l2);
        
        // Fill histograms
        
        fill_HNL_MC_check(&hists, &hists2D);
        //fill_HLT_efficiency(&hists, "Beforeptcut", (i_leading_e != -1), (i_leading_mu != -1));
        //fill_HLT_efficiency(&hists, "Afterptcut", (i_leading_e != -1 && leadptcut(i_leading_e)), (i_leading_mu != -1 && leadptcut(i_leading_mu)));

        fill_cutflow_e(&hists, "_SS_e");
        fill_cutflow_e(&hists, "_OS_e");
        fill_cutflow_mu(&hists, "_SS_mu");
        fill_cutflow_mu(&hists, "_OS_mu");


        TString signs_and_flavor = "";
        if(_1e1disple) signs_and_flavor = (_lCharge[i_leading_e] == _lCharge[i_subleading_displ_e])? "_SS_e" : "_OS_e";
        else if(_1mu1displmu) signs_and_flavor = (_lCharge[i_leading_mu] == _lCharge[i_subleading_displ_mu])? "_SS_mu" : "_OS_mu"; 
        

        if(_1e1disple){
            fill_lepton_eff(&hists, signs_and_flavor, i_leading_e, i_gen_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        //    fill_KVF_eff(&hists, signs_and_flavor, i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            fill_IVF_eff(&hists, signs_and_flavor, i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        }
        if(_1e1displedispl){
            fill_HNLtagger_tree(hnltagger_e, i_closel2_jet);
            JetTagVal = get_PFNprediction(pfn_e, i_closel2_jet);
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_afterdispl", i_leading_e, i_subleading_displ_e);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_afterdispl", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            if(sampleflavor != "bkg") fill_gen_HNLtagger_tree(hnltagger_gen_e, i_closel2_jet);
            if(signs_and_flavor == "_SS_e"){ SSe++; SSe_weight += event_weight;}
            else if(signs_and_flavor == "_OS_e"){ OSe++; OSe_weight += event_weight;}
        }
        if(_1mu1displmu){
            fill_lepton_eff(&hists, signs_and_flavor, i_leading_mu, i_gen_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        //    fill_KVF_eff(&hists, signs_and_flavor, i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            fill_IVF_eff(&hists, signs_and_flavor, i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        }
        if(_1mu1displmudispl){
            fill_HNLtagger_tree(hnltagger_mu, i_closel2_jet);
            JetTagVal = get_PFNprediction(pfn_mu, i_closel2_jet);
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_afterdispl", i_leading_mu, i_subleading_displ_mu);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_afterdispl", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            if(sampleflavor != "bkg") fill_gen_HNLtagger_tree(hnltagger_gen_mu, i_closel2_jet);
            if(signs_and_flavor == "_SS_mu"){ SSmu++; SSmu_weight += event_weight;}
            else if(signs_and_flavor == "_OS_mu"){ OSmu++; OSmu_weight += event_weight;}
        }

        //if(_1e1displevtx){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforedispl", i_leading_e, i_subleading_displ_e);
        //    fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_beforedispl", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        //}
        //if(_1mu1displmuvtx){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforedispl", i_leading_mu, i_subleading_displ_mu);
        //    fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_beforedispl", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        //}
        //
        //if(_1e1disple0adde){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforemll", i_leading_e, i_subleading_displ_e);
        //}
        //if(_1mu1displmu0addmu){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforemll", i_leading_mu, i_subleading_displ_mu);
        //}

        //if(_1e1displemll){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforedphi", i_leading_e, i_subleading_displ_e);
        //}
        //if(_1mu1displmumll){
        //    fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforedphi", i_leading_mu, i_subleading_displ_mu);
        //}

        if(_1e1displedphi){
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforereliso", i_leading_e, i_subleading_displ_e);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavor + "_beforereliso", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_beforereliso", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        //    if(i_gen_subleading_displ_e != -1 and _lIVF_match[i_subleading_displ_e] != -1 and get_IVF_SVgenreco(i_gen_subleading_displ_e, _lIVF_match[i_subleading_displ_e]) > 0.2) fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_invIVFSVgenreco", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        }
        if(_1mu1displmudphi){
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_beforereliso", i_leading_mu, i_subleading_displ_mu);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavori + "_beforereliso", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_beforereliso", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        //    if(i_gen_subleading_displ_mu != -1 and _lIVF_match[i_subleading_displ_mu] != -1 and get_IVF_SVgenreco(i_gen_subleading_displ_mu, _lIVF_match[i_subleading_displ_mu]) > 0.2) fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_invIVFSVgenreco", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        }
        if(_1e1displeReliso){
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_e, i_subleading_displ_e);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            if(signs_and_flavor == "_SS_e"){ SSe2++; SSe2_weight += event_weight;}
            else if(signs_and_flavor == "_OS_e"){ OSe2++; OSe2_weight += event_weight;}
        }
        if(_1mu1displmuReliso){
            fill_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_mu, i_subleading_displ_mu);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor + "_before1jet", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            if(signs_and_flavor == "_SS_mu"){ SSmu2++; SSmu2_weight += event_weight;}
            else if(signs_and_flavor == "_OS_mu"){ OSmu2++; OSmu2_weight += event_weight;}
        }
        if(_1e1disple1jet){
            fill_histograms(&hists, &hists2D, signs_and_flavor, i_leading_e, i_subleading_displ_e);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavor, i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor, i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
            if(signs_and_flavor == "_SS_e"){ SSe3++; SSe3_weight += event_weight;}
            else if(signs_and_flavor == "_OS_e"){ OSe3++; OSe3_weight += event_weight;}
        }
        if(_1mu1displmu1jet){
            fill_histograms(&hists, &hists2D, signs_and_flavor, i_leading_mu, i_subleading_displ_mu);
        //    fill_KVF_histograms(&hists, &hists2D, signs_and_flavor, i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            fill_IVF_histograms(&hists, &hists2D, signs_and_flavor, i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
            if(signs_and_flavor == "_SS_mu"){ SSmu3++; SSmu3_weight += event_weight;}
            else if(signs_and_flavor == "_OS_mu"){ OSmu3++; OSmu3_weight += event_weight;}
        }

        if(_1e1displedphi_novtx){
            fill_IVF_eff(&hists, signs_and_flavor + "_endofselection", i_leading_e, i_subleading_displ_e, i_gen_subleading_displ_e);
        }
        if(_1mu1displmudphi_novtx){
            fill_IVF_eff(&hists, signs_and_flavor + "_endofselection", i_leading_mu, i_subleading_displ_mu, i_gen_subleading_displ_mu);
        }
    }
/*
 * Small summary to write to terminal in order to quickly check state of results
 */
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Channel    #events     #events(with ind. weight)    #events(with tot. weight)" << endl;
    cout << "-------------------------2 leptons, vtx, displ-------------------------------" << endl;
    cout << "SS ee:       " << SSe <<  "        " << SSe_weight <<  "       " << 1.0*SSe_weight*total_weight << endl;
    cout << "SS mumu:     " << SSmu << "        " << SSmu_weight << "       " << 1.0*SSmu_weight*total_weight << endl;
    cout << "OS ee:       " << OSe <<  "        " << OSe_weight <<  "       " << 1.0*OSe_weight*total_weight << endl;
    cout << "OS mumu:     " << OSmu << "        " << OSmu_weight << "       " << 1.0*OSmu_weight*total_weight << endl;
    cout << "---------------------------After Reliso----------------------------------" << endl;
    cout << "SS ee:       " << SSe2 <<  "        " << SSe2_weight <<  "       " << 1.0*SSe2_weight*total_weight << endl;
    cout << "SS mumu:     " << SSmu2 << "        " << SSmu2_weight << "       " << 1.0*SSmu2_weight*total_weight << endl;
    cout << "OS ee:       " << OSe2 <<  "        " << OSe2_weight <<  "       " << 1.0*OSe2_weight*total_weight << endl;
    cout << "OS mumu:     " << OSmu2 << "        " << OSmu2_weight << "       " << 1.0*OSmu2_weight*total_weight << endl;
    cout << "---------------------------After 1jet----------------------------------" << endl;
    cout << "SS ee:       " << SSe3 <<  "        " << SSe3_weight <<  "       " << 1.0*SSe3_weight*total_weight << endl;
    cout << "SS mumu:     " << SSmu3 << "        " << SSmu3_weight << "       " << 1.0*SSmu3_weight*total_weight << endl;
    cout << "OS ee:       " << OSe3 <<  "        " << OSe3_weight <<  "       " << 1.0*OSe3_weight*total_weight << endl;
    cout << "OS mumu:     " << OSmu3 << "        " << OSmu3_weight << "       " << 1.0*OSmu3_weight*total_weight << endl;
    cout << "count:       " << count << endl;


/*
 * Write everything to output:
 * 1. find output name based on input sample name
 * 2. create output TFile
 * 3. normalize histograms and make under- and overflow visible
 * 4. give necessary text bin labels
 * 5. write events to output
 */
    if(sampleflavor == "bkg"){
        hnltagger_e.write_HNLtagger_tree();
        hnltagger_mu.write_HNLtagger_tree();
        hnltagger_gen_e.delete_HNLtagger_tree();
        hnltagger_gen_mu.delete_HNLtagger_tree();
    }else if(sampleflavor == "e"){
        hnltagger_e.write_HNLtagger_tree();
        hnltagger_mu.delete_HNLtagger_tree();
        hnltagger_gen_e.write_HNLtagger_tree();
        hnltagger_gen_mu.delete_HNLtagger_tree();
    }else if(sampleflavor == "mu"){
        hnltagger_e.delete_HNLtagger_tree();
        hnltagger_mu.write_HNLtagger_tree();
        hnltagger_gen_e.delete_HNLtagger_tree();
        hnltagger_gen_mu.write_HNLtagger_tree();
    }

    TString outputfilename = make_outputfilename(filename, "/user/bvermass/public/2l2q_analysis/histograms/", "hists_full_analyzer", partition, partitionjobnumber);
    cout << "output to: " << outputfilename << endl;
    TFile *output = new TFile(outputfilename, "recreate");

    // Add under- and overflow to first and last bins and normalize histograms to correct total weight.
    for( it = hists.begin(); it != hists.end(); it++){
        TH1* h = it->second;
        int nb = h->GetNbinsX();
        double b0  = h->GetBinContent( 0  );
        double e0  = h->GetBinError  ( 0  );
        double b1  = h->GetBinContent( 1  );
        double e1  = h->GetBinError  ( 1  );
        double bn  = h->GetBinContent(nb  );
        double en  = h->GetBinError  (nb  );
        double bn1 = h->GetBinContent(nb+1);
        double en1 = h->GetBinError  (nb+1);
    
        h->SetBinContent(0   , 0.);
        h->SetBinError  (0   , 0.);
        h->SetBinContent(1   , b0+b1);
        h->SetBinError  (1   , std::sqrt(e0*e0 + e1*e1  ));
        h->SetBinContent(nb  , bn+bn1);
        h->SetBinError  (nb  , std::sqrt(en*en + en1*en1));
        h->SetBinContent(nb+1, 0.);
        h->SetBinError  (nb+1, 0.);
        
        //if bin content is below zero, set it to 0 (dealing with negative weights)
        for(int i = 0; i < nb+1; i++){
            if(h->GetBinContent(i) < 0.) h->SetBinContent(i, 0.);
        }
        if(((TString)h->GetName()).Index("_eff_") == -1) h->Scale(total_weight); //this scaling now happens before the plotting stage, since after running, the histograms need to be hadded.
    }
    // Normalize 2D histograms to correct total weight
    for( it2D = hists2D.begin(); it2D != hists2D.end(); it2D++){
        TH2* h = it2D->second;
        if(((TString)h->GetName()).Index("_eff_") == -1) h->Scale(total_weight);
    }

    // Give histograms that need it the correct text bin labels
    give_alphanumeric_labels(&hists, "_SS_e");
    give_alphanumeric_labels(&hists, "_OS_e");
    give_alphanumeric_labels(&hists, "_SS_mu");
    give_alphanumeric_labels(&hists, "_OS_mu");


    // Write 1D histograms to outputfile
    for( it = hists.begin(); it != hists.end(); it++){
        TH1* h = it->second;
	    h->Write();
    }

    // Write 2D histograms to outputfile
    for( it2D = hists2D.begin(); it2D != hists2D.end(); it2D++){
        TH2* h = it2D->second;
        h->Write();
    }
 
    // This is a histogram with 1 bin that is filled with value 1.
    // After hadding files together, this allows to see how many were hadded together.
    // This was used for efficiency histograms that were elevated by number of hadded files. at the moment it is not used anymore (efficiencies are calculated after hadding, in plotting step)
    TH1F* hadd_counter = new TH1F("hadd_counter", "nr. of files hadded together;;", 1, 0, 1);
    hadd_counter->Fill(0);
    hadd_counter->Write();

    //hweight->Write();
    output->Close();
}

void full_analyzer::Loop()
{
//   In a ROOT session, you can do:
//      root> .L full_analyzer.cc
//      root> full_analyzer t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        if(jentry%1000 == 0) cout << "yeey " << jentry << endl;
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;
    }
}
