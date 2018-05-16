#define full_analyzer_cxx
#include "full_analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>

using namespace std;
//Contents:
//  run_over_file		: This is the main function to loop over events of a certain file, give the exact filename of the signal/background/data to the function
//  clean_jets			: cleans collection of jets by vetoing jets with dR < 0.4 with electrons or muons
//  clean_ele			: cleans collection of electrons by vetoing electrons with dR < 0.4 with muons
//  find_2_highest_pt_particles : finds 2 highest pt particles, if there is only one or zero, then i_jet1 or i_jet2 are put to -1
//  loop 			: currently not used

void full_analyzer::run_over_file(TString filename, TString flavor)
{
// Short description of program flow:
//     - initialize file
//     - initialize histograms
//     - start loop over events
//     - construct booleans for object selection? should be done at ntuplizer level, but all used variables should be included too
//     - functions for every signal region event selection
//     Make it very structured and clear!

    LeptonID b;
    b.test_function();

    if(flavor != "e" && flavor != "mu"){ cout << "Wrong flavor" << endl; return;}

    //TFile *input = new TFile("/user/bvermass/public/heavyNeutrino/" + filename + "/dilep.root", "open");
    TFile *input = new TFile(filename, "open");
    TTree *tree  = (TTree*) input->Get("blackJackAndHookers/blackJackAndHookersTree");
    Init(tree);


    //This map contains all 1D histograms
    std::map<TString, TH1*> hists;
    hists["ee_sigreg_fraction"]			= new TH1F("ee_sigreg_fraction",";signal regions;Events", 13, 0, 13);
    hists["mumu_sigreg_fraction"]		= new TH1F("mumu_sigreg_fraction",";signal regions;Events", 13, 0, 13);
    hists["1eovertotal"]			= new TH1F("1eovertotal",";bin1 = total, bin2 = 1e;Events",2,0,2);
    hists["1muovertotal"]			= new TH1F("1muovertotal",";bin1 = total, bin2 = 1mu;Events",2,0,2);
    // signal regions that are included:
    // 0 = 2iso l, 0jet
    // 1 = 2iso l, 1jet
    // 2 = 2iso l, 2jet
    // 3 = 1iso l, 1non-iso l, 0jet
    // 4 = 1iso l, 1non-iso l, 1jet
    // 5 = 1iso l, 1non-iso l, 2jet
    // 6 = 1iso l, 1displ l, 0jet
    // 7 = 1iso l, 1displ l, 1jet
    // 8 = 1iso l, 1displ l, 2jet
    // 9 = 1iso l, 0jet
    // 10= 1iso l, 1jet
    // 11= 1iso l, 2jet
    // 12= other
    for(auto&& sh : hists){
	auto&& h = sh.second;
	h->Sumw2();
    }
   
    int ee_else = 0; 
    int n_ele = 0;
    int n_after_eta = 0;
    int n_after_pt = 0;
    int n_after_dxy = 0;
    int n_after_dz = 0;
    int n_after_sip3d = 0;
    int n_after_reliso = 0;
    int n_after_invreliso = 0;
    int n_after_pogmedium = 0;
    int n_after_convveto = 0;
    int n_after_missinghits = 0;
    int n_after_invreliso_pogmedium = 0;
    int n_after_invreliso_convveto = 0;
    int n_after_invreliso_missinghits = 0;


    Long64_t nentries = tree->GetEntries();
    cout << "file: " << filename << endl;
    cout << "Number of events: " << nentries << endl;
    for(unsigned jentry = 0; jentry < nentries; ++jentry){
	LoadTree(jentry);
	tree->GetEntry(jentry);
	bool printevent = (jentry%5000 == 0);
	if(printevent){
	    cout << jentry << " of " << nentries << endl;
	}

	for(unsigned i = 0; i < _nL; ++i){
    	    if(_lFlavor[i] != 0) continue;
	    ++n_ele;
	    if(fabs(_lEta[i]) < 2.5) ++n_after_eta;
    	    if(_lPt[i] > 30) ++n_after_pt;
    	    if(fabs(_dxy[i]) < 0.05) ++n_after_dxy;
    	    if(fabs(_dz[i])  < 0.1) ++n_after_dz;
    	    if(_3dIPSig[i]   < 4) ++n_after_sip3d;
    	    if(_relIso[i]    < 0.2) ++n_after_reliso;
	    if(_relIso[i]    > 0.2) ++n_after_invreliso;
    	    if(_lPOGMedium[i]) ++n_after_pogmedium;
    	    if(_lElectronPassConvVeto[i]) ++n_after_convveto;
    	    if(_lElectronMissingHits[i] < 1) ++n_after_missinghits;
	    if(_relIso[i] > 0.03 && _lPOGMedium[i]) ++n_after_invreliso_pogmedium;
	    if(_relIso[i] > 0.2 && _lElectronPassConvVeto[i]) ++n_after_invreliso_convveto;
	    if(_relIso[i] > 0.2 && _lElectronMissingHits[i]) ++n_after_invreliso_missinghits;
	}
	
	bool fullElectronID[10];
	bool nonisoElectronID[10];
	bool displElectronID[10];
	bool fullMuonID[10];
	bool nonisoMuonID[10];
	bool displMuonID[10];
        bool fullJetID[20];
	get_electronID(&fullElectronID[0]);
	get_noniso_electronID(&nonisoElectronID[0]);
	get_displ_electronID(&displElectronID[0]);
	get_muonID(&fullMuonID[0]);
	get_noniso_muonID(&nonisoMuonID[0]);
	get_displ_muonID(&displMuonID[0]);
	get_jetID(&fullJetID[0]);

	bool jet_clean_full[20];
	bool jet_clean_noniso[20];
	bool jet_clean_displ[20];
	get_clean_jets(&jet_clean_full[0],   &fullElectronID[0], &fullMuonID[0]);
	get_clean_jets(&jet_clean_noniso[0], &nonisoElectronID[0], &nonisoMuonID[0]);
	get_clean_jets(&jet_clean_displ[0],  &displElectronID[0], &displMuonID[0]);
	bool ele_clean_full[10];
	bool ele_clean_noniso[10];
	bool ele_clean_displ[10];
	get_clean_ele(&ele_clean_full[0],   &fullMuonID[0]);
	get_clean_ele(&ele_clean_noniso[0], &nonisoMuonID[0]);
	get_clean_ele(&ele_clean_displ[0],  &displMuonID[0]);
	bool jet_clean_full_noniso[20];
	bool jet_clean_full_displ[20];
	for(unsigned i = 0; i < 20; ++i){
	    jet_clean_full_noniso[i] = jet_clean_full[i] && jet_clean_noniso[i];
	    jet_clean_full_displ[i] = jet_clean_full[i] && jet_clean_displ[i];
	}
	bool ele_clean_full_noniso_displ[10];
	for(unsigned i = 0; i < 10; ++i){
	    ele_clean_full_noniso_displ[i] = ele_clean_full[i] && ele_clean_noniso[i] && ele_clean_displ[i];
	}



	int i_leading_e     		= find_leading_e(&fullElectronID[0], &ele_clean_full_noniso_displ[0]);
	int i_subleading_e  		= find_subleading_e(&fullElectronID[0], &ele_clean_full_noniso_displ[0], i_leading_e);
	int i_leading_mu    		= find_leading_mu(&fullMuonID[0]);
	int i_subleading_mu 		= find_subleading_mu(&fullMuonID[0], i_leading_mu);
	int i_subleading_noniso_e  	= find_subleading_e(&nonisoElectronID[0], &ele_clean_full_noniso_displ[0], i_leading_e);
	int i_subleading_noniso_mu 	= find_subleading_mu(&nonisoMuonID[0], i_leading_mu);
	int i_subleading_displ_e  	= find_subleading_e(&displElectronID[0], &ele_clean_full_noniso_displ[0], i_leading_e);
	int i_subleading_displ_mu 	= find_subleading_mu(&displMuonID[0], i_leading_mu);
	
	int i_leading_jet_for_full	= find_leading_jet(&fullJetID[0], &jet_clean_full[0]);
	int i_subleading_jet_for_full	= find_subleading_jet(&fullJetID[0], &jet_clean_full[0], i_leading_jet_for_full);
	int i_leading_jet_for_noniso	= find_leading_jet(&fullJetID[0], &jet_clean_full_noniso[0]);
	int i_subleading_jet_for_noniso	= find_subleading_jet(&fullJetID[0], &jet_clean_full_noniso[0], i_leading_jet_for_noniso);
	int i_leading_jet_for_displ	= find_leading_jet(&fullJetID[0], &jet_clean_full_displ[0]);
	int i_subleading_jet_for_displ	= find_subleading_jet(&fullJetID[0], &jet_clean_full_displ[0], i_leading_jet_for_displ);

	/*for(unsigned i = 0; i < _nL; ++i){
	    if(nonisoElectronID[i]) cout << "noniso electron passed: " << nonisoElectronID[i] << endl;
	    if(nonisoMuonID[i]) cout << "noniso muon passed: " << nonisoMuonID[i] << endl;
	}*/
	if(printevent && flavor == "mu" && false){
	    cout << "i_subleading_displ_mu: " << i_subleading_displ_mu << endl;
	    /*cout << "good_leading_e: " << index_good_leading_e << endl;
	    cout << "good_subleading_e: " << index_good_subleading_e << endl;
	    cout << "good_leading_mu: " << index_good_leading_mu << endl;
	    cout << "good_subleading_mu: " << index_good_subleading_mu << endl;
	    cout << "good_leading_jet: " << index_good_leading_jet << endl;
	    cout << "good_subleading_jet: " << index_good_subleading_jet << endl;
	    if(flavor == "e") cout << "good_subleading_noniso_e: " << index_good_subleading_noniso_e << endl;
	    else if(flavor == "mu") cout << "good_subleading_noniso_mu: " << index_good_subleading_noniso_mu << endl;*/
	}		

        bool _1e			= i_leading_e != -1;
	bool _2e0jet 			= i_leading_e != -1 && i_subleading_e != -1 && i_leading_jet_for_full == -1 && i_subleading_jet_for_full == -1;
	bool _2e1jet 			= i_leading_e != -1 && i_subleading_e != -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full == -1;
	bool _2e2jet 			= i_leading_e != -1 && i_subleading_e != -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full != -1;
	bool _1e1nonisoe0jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e != -1 && i_leading_jet_for_noniso == -1 && i_subleading_jet_for_noniso == -1;
	bool _1e1nonisoe1jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e != -1 && i_leading_jet_for_noniso != -1 && i_subleading_jet_for_noniso == -1;
	bool _1e1nonisoe2jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e != -1 && i_leading_jet_for_noniso != -1 && i_subleading_jet_for_noniso != -1;
	bool _1e1disple0jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e != -1 && i_leading_jet_for_displ == -1 && i_subleading_jet_for_displ == -1;
	bool _1e1disple1jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e != -1 && i_leading_jet_for_displ != -1 && i_subleading_jet_for_displ == -1;
	bool _1e1disple2jet		= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e != -1 && i_leading_jet_for_displ != -1 && i_subleading_jet_for_displ != -1;
	bool _1e0jet			= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e == -1 && i_leading_jet_for_full == -1 && i_subleading_jet_for_full == -1;
	bool _1e1jet			= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e == -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full == -1;
	bool _1e2jet			= i_leading_e != -1 && i_subleading_e == -1 && i_subleading_noniso_e == -1 && i_subleading_displ_e == -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full != -1;

	bool _1mu			= i_leading_mu != -1;
	bool _2mu0jet 			= i_leading_mu != -1 && i_subleading_mu != -1 && i_leading_jet_for_full == -1 && i_subleading_jet_for_full == -1;
	bool _2mu1jet 			= i_leading_mu != -1 && i_subleading_mu != -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full == -1;
	bool _2mu2jet 			= i_leading_mu != -1 && i_subleading_mu != -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full != -1;
	bool _1mu1nonisomu0jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu != -1 && i_leading_jet_for_noniso == -1 && i_subleading_jet_for_noniso == -1;
	bool _1mu1nonisomu1jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu != -1 && i_leading_jet_for_noniso != -1 && i_subleading_jet_for_noniso == -1;
	bool _1mu1nonisomu2jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu != -1 && i_leading_jet_for_noniso != -1 && i_subleading_jet_for_noniso != -1;
	bool _1mu1displmu0jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu != -1 && i_leading_jet_for_displ == -1 && i_subleading_jet_for_displ == -1;
	bool _1mu1displmu1jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu != -1 && i_leading_jet_for_displ != -1 && i_subleading_jet_for_displ == -1;
	bool _1mu1displmu2jet		= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu != -1 && i_leading_jet_for_displ != -1 && i_subleading_jet_for_displ != -1;
	bool _1mu0jet			= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu == -1 && i_leading_jet_for_full == -1 && i_subleading_jet_for_full == -1;
	bool _1mu1jet			= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu == -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full == -1;
	bool _1mu2jet			= i_leading_mu != -1 && i_subleading_mu == -1 && i_subleading_noniso_mu == -1 && i_subleading_displ_mu == -1 && i_leading_jet_for_full != -1 && i_subleading_jet_for_full != -1;


	hists["1eovertotal"]->Fill(0);
	if(_1e){
	    hists["1eovertotal"]->Fill(1);
	    if(_2e0jet) hists["ee_sigreg_fraction"]->Fill(0);
	    else if(_2e1jet) hists["ee_sigreg_fraction"]->Fill(1);
	    else if(_2e2jet) hists["ee_sigreg_fraction"]->Fill(2);
	    else if(_1e1nonisoe0jet) hists["ee_sigreg_fraction"]->Fill(3);
	    else if(_1e1nonisoe1jet) hists["ee_sigreg_fraction"]->Fill(4);
	    else if(_1e1nonisoe2jet) hists["ee_sigreg_fraction"]->Fill(5);
	    else if(_1e1disple0jet)  hists["ee_sigreg_fraction"]->Fill(6);
	    else if(_1e1disple1jet)  hists["ee_sigreg_fraction"]->Fill(7);
	    else if(_1e1disple2jet)  hists["ee_sigreg_fraction"]->Fill(8);
	    else if(_1e0jet) hists["ee_sigreg_fraction"]->Fill(9);
	    else if(_1e1jet) hists["ee_sigreg_fraction"]->Fill(10);
	    else if(_1e2jet) hists["ee_sigreg_fraction"]->Fill(11);
	    else hists["ee_sigreg_fraction"]->Fill(12);
	}
	hists["1muovertotal"]->Fill(0);
	if(_1mu){
	    hists["1muovertotal"]->Fill(1);
	    if(_2mu0jet) hists["mumu_sigreg_fraction"]->Fill(0);
	    else if(_2mu1jet) hists["mumu_sigreg_fraction"]->Fill(1);
	    else if(_2mu2jet) hists["mumu_sigreg_fraction"]->Fill(2);
	    else if(_1mu1nonisomu0jet) hists["mumu_sigreg_fraction"]->Fill(3);
	    else if(_1mu1nonisomu1jet) hists["mumu_sigreg_fraction"]->Fill(4);
	    else if(_1mu1nonisomu2jet) hists["mumu_sigreg_fraction"]->Fill(5);
	    else if(_1mu1displmu0jet)  hists["mumu_sigreg_fraction"]->Fill(6);
	    else if(_1mu1displmu1jet)  hists["mumu_sigreg_fraction"]->Fill(7);
	    else if(_1mu1displmu2jet)  hists["mumu_sigreg_fraction"]->Fill(8);
	    else if(_1mu0jet) hists["mumu_sigreg_fraction"]->Fill(9);
	    else if(_1mu1jet) hists["mumu_sigreg_fraction"]->Fill(10);
	    else if(_1mu2jet) hists["mumu_sigreg_fraction"]->Fill(11);
	    else hists["mumu_sigreg_fraction"]->Fill(12);
	}
    }
    //cout << "ee else: " << ee_else << endl;
    /*cout << "n total " << n_ele << endl;
    cout << "after eta " << n_after_eta << endl;
    cout << "after pt " << n_after_pt << endl;
    cout << "after dxy " << n_after_dxy << endl;
    cout << "after dz " << n_after_dz << endl;
    cout << "after sip3d " << n_after_sip3d << endl;
    cout << "after reliso " << n_after_reliso << endl;
    cout << "after inverted reliso " << n_after_invreliso << endl;
    cout << "after pogmedium " << n_after_pogmedium << endl;
    cout << "after convveto " << n_after_convveto << endl;
    cout << "after missinghits " << n_after_missinghits << endl;
    cout << "after invreliso and pogmedium " << n_after_invreliso_pogmedium << endl;
    cout << "after invreliso and convveto " << n_after_invreliso_convveto << endl;
    cout << "after invreliso and missinghits " << n_after_invreliso_missinghits << endl;*/

    for(auto&& sh : hists){
	auto&& h  = sh.second;
	h->Scale(100/h->GetEntries());
    }

    if(flavor == "e"){
        cout << "2iso e, 0jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(1) << endl;
        cout << "2iso e, 1jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(2) << endl;
        cout << "2iso e, 2jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(3) << endl;
        cout << "1iso e, 1noniso e, 0jet: " << hists["ee_sigreg_fraction"]->GetBinContent(4) << endl;
        cout << "1iso e, 1noniso e, 1jet: " << hists["ee_sigreg_fraction"]->GetBinContent(5) << endl;
        cout << "1iso e, 1noniso e, 2jet: " << hists["ee_sigreg_fraction"]->GetBinContent(6) << endl;
        cout << "1iso e, 1displ e, 0jet:  " << hists["ee_sigreg_fraction"]->GetBinContent(7) << endl;
        cout << "1iso e, 1displ e, 1jet:  " << hists["ee_sigreg_fraction"]->GetBinContent(8) << endl;
        cout << "1iso e, 1displ e, 2jet:  " << hists["ee_sigreg_fraction"]->GetBinContent(9) << endl << endl;
        cout << "1iso e, 0jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(10) << endl;
        cout << "1iso e, 1jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(11) << endl;
        cout << "1iso e, 2jet:            " << hists["ee_sigreg_fraction"]->GetBinContent(12) << endl;
        cout << "other:                   " << hists["ee_sigreg_fraction"]->GetBinContent(13) << endl;
    }
    if(flavor == "mu"){
        cout << "2iso mu, 0jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(1) << endl;
        cout << "2iso mu, 1jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(2) << endl;
        cout << "2iso mu, 2jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(3) << endl;
        cout << "1iso mu, 1noniso mu, 0jet: " << hists["mumu_sigreg_fraction"]->GetBinContent(4) << endl;
        cout << "1iso mu, 1noniso mu, 1jet: " << hists["mumu_sigreg_fraction"]->GetBinContent(5) << endl;
        cout << "1iso mu, 1noniso mu, 2jet: " << hists["mumu_sigreg_fraction"]->GetBinContent(6) << endl;
        cout << "1iso mu, 1displ mu, 0jet:  " << hists["mumu_sigreg_fraction"]->GetBinContent(7) << endl;
        cout << "1iso mu, 1displ mu, 1jet:  " << hists["mumu_sigreg_fraction"]->GetBinContent(8) << endl;
        cout << "1iso mu, 1displ mu, 2jet:  " << hists["mumu_sigreg_fraction"]->GetBinContent(9) << endl << endl;
        cout << "1iso mu, 0jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(10) << endl;
        cout << "1iso mu, 1jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(11) << endl;
        cout << "1iso mu, 2jet:             " << hists["mumu_sigreg_fraction"]->GetBinContent(12) << endl;
        cout << "other:                     " << hists["mumu_sigreg_fraction"]->GetBinContent(13) << endl;
    }


    TString outputfilename = "";
    if(filename.Index("HeavyNeutrino") != -1) outputfilename = "histograms/hists_full_analyzer_" + filename(filename.Index("/") + 1, filename.Length() - filename.Index("/") - 1) + "_" + filename(0, filename.Index("/"))  + ".root";
    cout << "output to: " << outputfilename << endl;
    TFile *output = new TFile(outputfilename, "recreate");
    for(auto&& sh : hists){
	auto&& h  = sh.second;
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
	h->Write();
    }
    output->Close();
}




void full_analyzer::testrun()
{
    run_over_file("prompt/HeavyNeutrino_lljj_M-1_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-1_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-2_V-0.1054524236_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-2_V-0.1054524236_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-3_V-0.03823254899_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-3_V-0.03823254899_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-5_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-5_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-10_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-10_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-20_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-20_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-30_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-30_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-40_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-40_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-50_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-50_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-60_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-60_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-80_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-80_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-100_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-100_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-120_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-120_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-200_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-400_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("prompt/HeavyNeutrino_lljj_M-400_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-800_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("prompt/HeavyNeutrino_lljj_M-1000_V-0.01_mu_onshell_pre2017_NLO", "mu");
    run_over_file("displaced/HeavyNeutrino_lljj_M-1_V-0.59587618054_e_onshell_pre2017_NLO", "e");
    run_over_file("displaced/HeavyNeutrino_lljj_M-1_V-0.59587618054_mu_onshell_pre2017_NLO", "mu");
    run_over_file("displaced/HeavyNeutrino_lljj_M-2_V-0.1054524236_e_onshell_pre2017_NLO", "e");
    run_over_file("displaced/HeavyNeutrino_lljj_M-2_V-0.1054524236_mu_onshell_pre2017_NLO", "mu");
    run_over_file("displaced/HeavyNeutrino_lljj_M-3_V-0.03823254899_e_onshell_pre2017_NLO", "e");
    run_over_file("displaced/HeavyNeutrino_lljj_M-3_V-0.03823254899_mu_onshell_pre2017_NLO", "mu");
    run_over_file("displaced/HeavyNeutrino_lljj_M-5_V-0.01_e_onshell_pre2017_NLO", "e");
    run_over_file("displaced/HeavyNeutrino_lljj_M-5_V-0.01_mu_onshell_pre2017_NLO", "mu");

    print_table();
}




void full_analyzer::get_electronID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 0 &&
			fabs(_lEta[i]) < 2.5 &&
			_lPt[i] > 7 &&
			fabs(_dxy[i]) < 0.05 &&
			fabs(_dz[i])  < 0.1 &&
			_3dIPSig[i]   < 4 &&
			_relIso[i]    < 0.2 &&
			_lPOGMedium[i] &&
			_lElectronPassConvVeto[i] &&
			_lElectronMissingHits[i] < 1;
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}

void full_analyzer::get_noniso_electronID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 0 &&
			fabs(_lEta[i]) < 2.5 &&
			_lPt[i] > 7 &&
			fabs(_dxy[i]) < 0.05 &&
			fabs(_dz[i])  < 0.1 &&
			_3dIPSig[i]   < 4 &&
			_relIso[i]    > 0.2 &&
			//_lPOGMedium[i] && //->clashes with reliso > 0.2
			_lElectronPassConvVeto[i] &&
			_lElectronMissingHits[i] < 1;
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}

void full_analyzer::get_displ_electronID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 0 &&
			fabs(_lEta[i]) < 2.5 &&
			_lPt[i] > 7 &&
			fabs(_dxy[i])  > 0.05 &&
			//no dz cut
			//no SIP3D cut
			//also invert reliso?
			//no pogmedium?
			_lElectronPassConvVeto[i];//figure out how this one works exactly to be sure it can still be applied!
			//no missing hits cut?
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}


void full_analyzer::get_muonID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 1 &&
			fabs(_lEta[i]) < 2.4 &&
			_lPt[i] > 5 &&
			fabs(_dxy[i]) < 0.05 &&
			fabs(_dz[i])  < 0.1 &&
			_3dIPSig[i]   < 4 &&
			_relIso[i]    < 0.2 &&
			_lPOGMedium[i];
			// innertrack, PFmuon and global or tracker muon conditions are executed at ntuplizer level and not stored
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}

void full_analyzer::get_noniso_muonID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 1 &&
			fabs(_lEta[i]) < 2.4 &&
			_lPt[i] > 5 &&
			fabs(_dxy[i]) < 0.05 &&
			fabs(_dz[i])  < 0.1 &&
			_3dIPSig[i]   < 4 &&
			_relIso[i]    > 0.2 &&
			_lPOGMedium[i];
			// innertrack, PFmuon and global or tracker muon conditions are executed at ntuplizer level and not stored
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}

void full_analyzer::get_displ_muonID(bool* ID)
{
    for(unsigned i = 0; i < _nL; ++i){
	bool fullID = 	_lFlavor[i] == 1 &&
			fabs(_lEta[i]) < 2.4 &&
			_lPt[i] > 5 &&
			fabs(_dxy[i]) > 0.05;
			//no dz cut
			//no SIP3D cut
			//also invert reliso?
			//no POGMedium? no because it requires dxy of the track to be small 
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}



void full_analyzer::get_jetID(bool* ID)
{
    for(unsigned i = 0; i < _nJets; ++i){
	bool fullID = 	fabs(_jetEta[i]) < 2.4 &&
			_jetPt[i] > 20 &&
			_jetIsTight[i];
	if(fullID) *(ID + i) = true;
	else *(ID + i) = false;
    }
}




void full_analyzer::get_clean_jets(bool* cleaned, bool* electronID, bool* muonID)
{
    // jets are cleaned from leptons if dR < 0.4 
    TLorentzVector lepton;
    TLorentzVector jet;

    for(unsigned i = 0; i < _nJets; ++i){
	*(cleaned + i) = true;
	jet.SetPtEtaPhiE(_jetPt[i], _jetEta[i], _jetPhi[i], _jetE[i]);

	for(unsigned j = 0; j < _nL; ++j){
	    lepton.SetPtEtaPhiE(_lPt[j], _lEta[j], _lPhi[j], _lE[j]);
	    if((_lFlavor[j] == 0 && *(electronID + j)) || (_lFlavor[j] == 1 && *(muonID + j))){
		if(lepton.DeltaR(jet) < 0.4) *(cleaned + i) = false;
	    }
	}
    }
}




void full_analyzer::get_clean_ele(bool* cleaned, bool* muonID)
{
    // This function returns false in the boolean cleaned for electrons within a cone of 0.4 around muons
    TLorentzVector muon;
    TLorentzVector electron;

    for(unsigned i_el = 0; i_el < _nL; ++i_el){
	*(cleaned + i_el) = true;
	if(_lFlavor[i_el] != 0) continue;
	electron.SetPtEtaPhiE(_lPt[i_el], _lEta[i_el], _lPhi[i_el], _lE[i_el]);

	for(unsigned i_mu = 0; i_mu < _nL; ++i_mu){
	    if(_lFlavor[i_mu] == 1 && *(muonID + i_mu)){
		muon.SetPtEtaPhiE(_lPt[i_mu], _lEta[i_mu], _lPhi[i_mu], _lE[i_mu]);
		if(muon.DeltaR(electron) < 0.4) *(cleaned + i_el) = false;
	    }
	}
    }
}




int full_analyzer::find_leading_e(bool* electronID, bool* ele_clean)
{
    int index_good_leading = -1;
    for(unsigned i = 0; i < _nL; ++i){
	if(_lFlavor[i] != 0)   continue;
	if(!*(electronID + i)) continue;
	if(!*(ele_clean + i))  continue;
	if(_lPt[i] < 30)       continue;
	if(index_good_leading == -1) index_good_leading = i;
	if(_lPt[i] > _lPt[index_good_leading]) index_good_leading = i;
    }
    return index_good_leading;
}

int full_analyzer::find_leading_mu(bool* muonID)
{
    int index_good_leading = -1;
    for(unsigned i = 0; i < _nL; ++i){
	if(_lFlavor[i] != 1) continue;
	if(!*(muonID + i))   continue;
	if(_lPt[i] < 25)     continue;
	if(index_good_leading == -1) index_good_leading = i;
	if(_lPt[i] > _lPt[index_good_leading]) index_good_leading = i;
    }
    return index_good_leading;
}

int full_analyzer::find_leading_jet(bool* jetID, bool* jet_clean)
{
    int index_good_leading = -1;
    for(unsigned i = 0; i < _nJets; ++i){
	if(!*(jetID + i))      continue;
	if(!*(jet_clean + i))  continue;
	if(index_good_leading == -1) index_good_leading = i;
	if(_lPt[i] > _lPt[index_good_leading]) index_good_leading = i;
    }
    return index_good_leading;
}

int full_analyzer::find_subleading_e(bool* electronID, bool* ele_clean, int index_good_leading)
{
    int index_good_subleading = -1;
    if(index_good_leading == -1) return index_good_subleading;
    for(unsigned i = 0; i < _nL; ++i){
	if(i == index_good_leading) continue;
	if(_lFlavor[i] != 0)   	    continue;
	//if(_lCharge[i] != _lCharge[index_good_leading]) continue;
	if(!*(electronID + i))      continue;
	if(!*(ele_clean + i))       continue;
	if(_lPt[i] < 7)             continue;
	if(index_good_subleading == -1) index_good_subleading = i;
	if(_lPt[i] > _lPt[index_good_subleading]) index_good_subleading = i;
    }
    return index_good_subleading;
}

int full_analyzer::find_subleading_mu(bool* muonID, int index_good_leading)
{
    int index_good_subleading = -1;
    if(index_good_leading == -1) return index_good_subleading;
    for(unsigned i = 0; i < _nL; ++i){
	if(i == index_good_leading) continue;
	if(_lFlavor[i] != 1)   	    continue;
	//if(_lCharge[i] != _lCharge[index_good_leading]) continue;
	if(!*(muonID + i))          continue;
	if(_lPt[i] < 5)             continue;
	if(index_good_subleading == -1) index_good_subleading = i;
	if(_lPt[i] > _lPt[index_good_subleading]) index_good_subleading = i;
    }
    return index_good_subleading;
}

int full_analyzer::find_subleading_jet(bool* jetID, bool* jet_clean, int index_good_leading)
{
    int index_good_subleading = -1;
    if(index_good_leading == -1) return index_good_subleading;
    for(unsigned i = 0; i < _nJets; ++i){
	if(i == index_good_leading) continue;
	if(!*(jetID + i))           continue;
	if(!*(jet_clean + i))       continue;
	if(index_good_subleading == -1) index_good_subleading = i;
	if(_lPt[i] > _lPt[index_good_subleading]) index_good_subleading = i;
    }
    return index_good_subleading;
}

void full_analyzer::print_table()
{
    TString pwd = "/user/bvermass/heavyNeutrino/Dileptonprompt/CMSSW_8_0_30/src/samesign_Analysis/histograms/hists_full_analyzer_";
    TFile *input1eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-1_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input1muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-1_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input2eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-2_V-0.1054524236_e_onshell_pre2017_NLO.root", "open");
    TFile *input2muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-2_V-0.1054524236_mu_onshell_pre2017_NLO.root", "open");
    TFile *input3eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-3_V-0.03823254899_e_onshell_pre2017_NLO.root", "open");
    TFile *input3muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-3_V-0.03823254899_mu_onshell_pre2017_NLO.root", "open");
    TFile *input5eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-5_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input5muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-5_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input10eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-10_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input10muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-10_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input20eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-20_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input20muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-20_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input30eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-30_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input30muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-30_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input40eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-40_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input40muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-40_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input50eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-50_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input50muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-50_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input60eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-60_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input60muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-60_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input80eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-80_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input80muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-80_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input100eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-100_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input100muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-100_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input120eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-120_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input120muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-120_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input200eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-200_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input400eprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-400_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input400muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-400_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input800muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-800_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input1000muprompt = new TFile(pwd + "prompt/HeavyNeutrino_lljj_M-1000_V-0.01_mu_onshell_pre2017_NLO.root", "open");
    TFile *input1edisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-1_V-0.59587618054_e_onshell_pre2017_NLO.root", "open");
    TFile *input1mudisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-1_V-0.59587618054_mu_onshell_pre2017_NLO.root", "open");
    TFile *input2edisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-2_V-0.1054524236_e_onshell_pre2017_NLO.root", "open");
    TFile *input2mudisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-2_V-0.1054524236_mu_onshell_pre2017_NLO.root", "open");
    TFile *input3edisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-3_V-0.03823254899_e_onshell_pre2017_NLO.root", "open");
    TFile *input3mudisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-3_V-0.03823254899_mu_onshell_pre2017_NLO.root", "open");
    TFile *input5edisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-5_V-0.01_e_onshell_pre2017_NLO.root", "open");
    TFile *input5mudisplaced = new TFile(pwd + "displaced/HeavyNeutrino_lljj_M-5_V-0.01_mu_onshell_pre2017_NLO.root", "open");

    std::map<TString, TH1*> hists;
    hists["1eprompt"] = (TH1F*) input1eprompt->Get("ee_sigreg_fraction");
    hists["1muprompt"] = (TH1F*) input1muprompt->Get("mumu_sigreg_fraction");
    hists["2eprompt"] = (TH1F*) input2eprompt->Get("ee_sigreg_fraction");
    hists["2muprompt"] = (TH1F*) input2muprompt->Get("mumu_sigreg_fraction");
    hists["3eprompt"] = (TH1F*) input3eprompt->Get("ee_sigreg_fraction");
    hists["3muprompt"] = (TH1F*) input3muprompt->Get("mumu_sigreg_fraction");
    hists["5eprompt"] = (TH1F*) input5eprompt->Get("ee_sigreg_fraction");
    hists["5muprompt"] = (TH1F*) input5muprompt->Get("mumu_sigreg_fraction");
    hists["10eprompt"] = (TH1F*) input10eprompt->Get("ee_sigreg_fraction");
    hists["10muprompt"] = (TH1F*) input10muprompt->Get("mumu_sigreg_fraction");
    hists["20eprompt"] = (TH1F*) input20eprompt->Get("ee_sigreg_fraction");
    hists["20muprompt"] = (TH1F*) input20muprompt->Get("mumu_sigreg_fraction");
    hists["30eprompt"] = (TH1F*) input30eprompt->Get("ee_sigreg_fraction");
    hists["30muprompt"] = (TH1F*) input30muprompt->Get("mumu_sigreg_fraction");
    hists["40eprompt"] = (TH1F*) input40eprompt->Get("ee_sigreg_fraction");
    hists["40muprompt"] = (TH1F*) input40muprompt->Get("mumu_sigreg_fraction");
    hists["50eprompt"] = (TH1F*) input50eprompt->Get("ee_sigreg_fraction");
    hists["50muprompt"] = (TH1F*) input50muprompt->Get("mumu_sigreg_fraction");
    hists["60eprompt"] = (TH1F*) input60eprompt->Get("ee_sigreg_fraction");
    hists["60muprompt"] = (TH1F*) input60muprompt->Get("mumu_sigreg_fraction");
    hists["80eprompt"] = (TH1F*) input80eprompt->Get("ee_sigreg_fraction");
    hists["80muprompt"] = (TH1F*) input80muprompt->Get("mumu_sigreg_fraction");
    hists["100eprompt"] = (TH1F*) input100eprompt->Get("ee_sigreg_fraction");
    hists["100muprompt"] = (TH1F*) input100muprompt->Get("mumu_sigreg_fraction");
    hists["120eprompt"] = (TH1F*) input120eprompt->Get("ee_sigreg_fraction");
    hists["120muprompt"] = (TH1F*) input120muprompt->Get("mumu_sigreg_fraction");
    hists["200eprompt"] = (TH1F*) input200eprompt->Get("ee_sigreg_fraction");
    hists["400eprompt"] = (TH1F*) input400eprompt->Get("ee_sigreg_fraction");
    hists["400muprompt"] = (TH1F*) input400muprompt->Get("mumu_sigreg_fraction");
    hists["800muprompt"] = (TH1F*) input800muprompt->Get("mumu_sigreg_fraction");
    hists["1000muprompt"] = (TH1F*) input1000muprompt->Get("mumu_sigreg_fraction");
    hists["1edisplaced"] = (TH1F*) input1edisplaced->Get("ee_sigreg_fraction");
    hists["1mudisplaced"] = (TH1F*) input1mudisplaced->Get("mumu_sigreg_fraction");
    hists["2edisplaced"] = (TH1F*) input2edisplaced->Get("ee_sigreg_fraction");
    hists["2mudisplaced"] = (TH1F*) input2mudisplaced->Get("mumu_sigreg_fraction");
    hists["3edisplaced"] = (TH1F*) input3edisplaced->Get("ee_sigreg_fraction");
    hists["3mudisplaced"] = (TH1F*) input3mudisplaced->Get("mumu_sigreg_fraction");
    hists["5edisplaced"] = (TH1F*) input5edisplaced->Get("ee_sigreg_fraction");
    hists["5mudisplaced"] = (TH1F*) input5mudisplaced->Get("mumu_sigreg_fraction");

    cout << "This text can be copied to Latex in its entirety." << endl;
    cout << "First two mumu tables are printed, then two ee tables for different HNL masses." << endl;
    
    cout << "\\begin{frame} " << endl;
    cout << "\\frametitle{$\\mu \\mu$qq channel}" << endl;
    cout << "\\begin{center}" << endl;
    cout << "\\scalebox{0.85}{" << endl;
    cout << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|}" << endl;
    cout << "\\multicolumn{3}{|c|}{$M_N$ [GeV]} & 2(d) & 3(d) & 5(d) & 10(p) & 20(p) & 30(p) \\\\" << endl;
    cout << "\\hline" << endl;
    cout << "& 1iso $\\mu$ & 0jet & " << round(hists["2mudisplaced"]->GetBinContent(1)) << " & " << round(hists["3mudisplaced"]->GetBinContent(1)) << " & " << round(hists["5mudisplaced"]->GetBinContent(1)) << " & " << round(hists["10muprompt"]->GetBinContent(1)) << " & " << round(hists["20muprompt"]->GetBinContent(1)) << " & " << round(hists["30muprompt"]->GetBinContent(1)) << " \\\\ " << endl;
    cout << "& 1iso $\\mu$ & 1jet & " << round(hists["2mudisplaced"]->GetBinContent(2)) << " & " << round(hists["3mudisplaced"]->GetBinContent(2)) << " & " << round(hists["5mudisplaced"]->GetBinContent(2)) << " & " << round(hists["10muprompt"]->GetBinContent(2)) << " & " << round(hists["20muprompt"]->GetBinContent(2)) << " & " << round(hists["30muprompt"]->GetBinContent(2)) << " \\\\ " << endl;
    cout << "& 1iso $\\mu$ & $\\geq$2jet & " << round(hists["2mudisplaced"]->GetBinContent(3)) << " & " << round(hists["3mudisplaced"]->GetBinContent(3)) << " & " << round(hists["5mudisplaced"]->GetBinContent(3)) << " & " << round(hists["10muprompt"]->GetBinContent(3)) << " & " << round(hists["20muprompt"]->GetBinContent(3)) << " & " << round(hists["30muprompt"]->GetBinContent(3)) << " \\\\ " << endl;
    cout << "& 1non-iso $\\mu$ & 0jet & " << round(hists["2mudisplaced"]->GetBinContent(4)) << " & " << round(hists["3mudisplaced"]->GetBinContent(4)) << " & " << round(hists["5mudisplaced"]->GetBinContent(4)) << " & " << round(hists["10muprompt"]->GetBinContent(4)) << " & " << round(hists["20muprompt"]->GetBinContent(4)) << " & " << round(hists["30muprompt"]->GetBinContent(4)) << " \\\\ " << endl;
    cout << "1iso $\\mu$ & 1non-iso $\\mu$ & 1jet & " << round(hists["2mudisplaced"]->GetBinContent(5)) << " & " << round(hists["3mudisplaced"]->GetBinContent(5)) << " & " << round(hists["5mudisplaced"]->GetBinContent(5)) << " & " << round(hists["10muprompt"]->GetBinContent(5)) << " & " << round(hists["20muprompt"]->GetBinContent(5)) << " & " << round(hists["30muprompt"]->GetBinContent(5)) << " \\\\ " << endl;
    cout << "& 1non-iso $\\mu$ & $\\geq$2jet & " << round(hists["2mudisplaced"]->GetBinContent(6)) << " & " << round(hists["3mudisplaced"]->GetBinContent(6)) << " & " << round(hists["5mudisplaced"]->GetBinContent(6)) << " & " << round(hists["10muprompt"]->GetBinContent(6)) << " & " << round(hists["20muprompt"]->GetBinContent(6)) << " & " << round(hists["30muprompt"]->GetBinContent(6)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & 0jet & " << round(hists["2mudisplaced"]->GetBinContent(7)) << " & " << round(hists["3mudisplaced"]->GetBinContent(7)) << " & " << round(hists["5mudisplaced"]->GetBinContent(7)) << " & " << round(hists["10muprompt"]->GetBinContent(7)) << " & " << round(hists["20muprompt"]->GetBinContent(7)) << " & " << round(hists["30muprompt"]->GetBinContent(7)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & 1jet & " << round(hists["2mudisplaced"]->GetBinContent(8)) << " & " << round(hists["3mudisplaced"]->GetBinContent(8)) << " & " << round(hists["5mudisplaced"]->GetBinContent(8)) << " & " << round(hists["10muprompt"]->GetBinContent(8)) << " & " << round(hists["20muprompt"]->GetBinContent(8)) << " & " << round(hists["30muprompt"]->GetBinContent(8)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & $\\geq$2jet & " << round(hists["2mudisplaced"]->GetBinContent(9)) << " & " << round(hists["3mudisplaced"]->GetBinContent(9)) << " & " << round(hists["5mudisplaced"]->GetBinContent(9)) << " & " << round(hists["10muprompt"]->GetBinContent(9)) << " & " << round(hists["20muprompt"]->GetBinContent(9)) << " & " << round(hists["30muprompt"]->GetBinContent(9)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & 0jet &" << round(hists["2mudisplaced"]->GetBinContent(10)) << " & " << round(hists["3mudisplaced"]->GetBinContent(10)) << " & " << round(hists["5mudisplaced"]->GetBinContent(10)) << " & " << round(hists["10muprompt"]->GetBinContent(10)) << " & " << round(hists["20muprompt"]->GetBinContent(10)) << " & " << round(hists["30muprompt"]->GetBinContent(10)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & 1jet &" << round(hists["2mudisplaced"]->GetBinContent(11)) << " & " << round(hists["3mudisplaced"]->GetBinContent(11)) << " & " << round(hists["5mudisplaced"]->GetBinContent(11)) << " & " << round(hists["10muprompt"]->GetBinContent(11)) << " & " << round(hists["20muprompt"]->GetBinContent(11)) << " & " << round(hists["30muprompt"]->GetBinContent(11)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & $\\geq$2jet &" << round(hists["2mudisplaced"]->GetBinContent(12)) << " & " << round(hists["3mudisplaced"]->GetBinContent(12)) << " & " << round(hists["5mudisplaced"]->GetBinContent(12)) << " & " << round(hists["10muprompt"]->GetBinContent(12)) << " & " << round(hists["20muprompt"]->GetBinContent(12)) << " & " << round(hists["30muprompt"]->GetBinContent(12)) << " \\\\ " << endl;
    cout << "\\hline" << endl;
    cout << "\\end{tabular}" << endl;
    cout << "}" << endl;
    cout << "\\end{center}" << endl;
    cout << "\\end{frame}" << endl << endl;

    cout << "\\begin{frame} " << endl;
    cout << "\\frametitle{$\\mu \\mu$qq channel}" << endl;
    cout << "\\begin{center}" << endl;
    cout << "\\scalebox{0.85}{" << endl;
    cout << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}" << endl;
    cout << "\\multicolumn{3}{|c|}{$M_N$ [GeV]} & 40 & 50 & 60 & 80 & 100 & 120 & 400 \\\\" << endl;
    cout << "\\hline" << endl;
    cout << "& 1iso $\\mu$ & 0jet & " << round(hists["40muprompt"]->GetBinContent(1)) << " & " << round(hists["50muprompt"]->GetBinContent(1)) << " & " << round(hists["60muprompt"]->GetBinContent(1)) << " & " << round(hists["80muprompt"]->GetBinContent(1)) << " & " << round(hists["100muprompt"]->GetBinContent(1)) << " & " << round(hists["120muprompt"]->GetBinContent(1)) << " & " << round(hists["400muprompt"]->GetBinContent(1)) << " \\\\ " << endl;
    cout << "& 1iso $\\mu$ & 1jet & " << round(hists["40muprompt"]->GetBinContent(2)) << " & " << round(hists["50muprompt"]->GetBinContent(2)) << " & " << round(hists["60muprompt"]->GetBinContent(2)) << " & " << round(hists["80muprompt"]->GetBinContent(2)) << " & " << round(hists["100muprompt"]->GetBinContent(2)) << " & " << round(hists["120muprompt"]->GetBinContent(2)) << " & " << round(hists["400muprompt"]->GetBinContent(2)) << " \\\\ " << endl;
    cout << "& 1iso $\\mu$ & $\\geq$2jet & " << round(hists["40muprompt"]->GetBinContent(3)) << " & " << round(hists["50muprompt"]->GetBinContent(3)) << " & " << round(hists["60muprompt"]->GetBinContent(3)) << " & " << round(hists["80muprompt"]->GetBinContent(3)) << " & " << round(hists["100muprompt"]->GetBinContent(3)) << " & " << round(hists["120muprompt"]->GetBinContent(3)) << " & " << round(hists["400muprompt"]->GetBinContent(3)) << " \\\\ " << endl;
    cout << "& 1non-iso $\\mu$ & 0jet & " << round(hists["40muprompt"]->GetBinContent(4)) << " & " << round(hists["50muprompt"]->GetBinContent(4)) << " & " << round(hists["60muprompt"]->GetBinContent(4)) << " & " << round(hists["80muprompt"]->GetBinContent(4)) << " & " << round(hists["100muprompt"]->GetBinContent(4)) << " & " << round(hists["120muprompt"]->GetBinContent(4)) << " & " << round(hists["400muprompt"]->GetBinContent(4)) << " \\\\ " << endl;
    cout << "1iso $\\mu$ & 1non-iso $\\mu$ & 1jet & " << round(hists["40muprompt"]->GetBinContent(5)) << " & " << round(hists["50muprompt"]->GetBinContent(5)) << " & " << round(hists["60muprompt"]->GetBinContent(5)) << " & " << round(hists["80muprompt"]->GetBinContent(5)) << " & " << round(hists["100muprompt"]->GetBinContent(5)) << " & " << round(hists["120muprompt"]->GetBinContent(5)) << " & " << round(hists["400muprompt"]->GetBinContent(5)) << " \\\\ " << endl;
    cout << "& 1non-iso $\\mu$ & $\\geq$2jet & " << round(hists["40muprompt"]->GetBinContent(6)) << " & " << round(hists["50muprompt"]->GetBinContent(6)) << " & " << round(hists["60muprompt"]->GetBinContent(6)) << " & " << round(hists["80muprompt"]->GetBinContent(6)) << " & " << round(hists["100muprompt"]->GetBinContent(6)) << " & " << round(hists["120muprompt"]->GetBinContent(6)) << " & " << round(hists["400muprompt"]->GetBinContent(6)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & 0jet & " << round(hists["40muprompt"]->GetBinContent(7)) << " & " << round(hists["50muprompt"]->GetBinContent(7)) << " & " << round(hists["60muprompt"]->GetBinContent(7)) << " & " << round(hists["80muprompt"]->GetBinContent(7)) << " & " << round(hists["100muprompt"]->GetBinContent(7)) << " & " << round(hists["120muprompt"]->GetBinContent(7)) << " & " << round(hists["400muprompt"]->GetBinContent(7)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & 1jet & " << round(hists["40muprompt"]->GetBinContent(8)) << " & " << round(hists["50muprompt"]->GetBinContent(8)) << " & " << round(hists["60muprompt"]->GetBinContent(8)) << " & " << round(hists["80muprompt"]->GetBinContent(8)) << " & " << round(hists["100muprompt"]->GetBinContent(8)) << " & " << round(hists["120muprompt"]->GetBinContent(8)) << " & " << round(hists["400muprompt"]->GetBinContent(8)) << " \\\\ " << endl;
    cout << "& 1displ $\\mu$ & $\\geq$2jet & " << round(hists["40muprompt"]->GetBinContent(9)) << " & " << round(hists["50muprompt"]->GetBinContent(9)) << " & " << round(hists["60muprompt"]->GetBinContent(9)) << " & " << round(hists["80muprompt"]->GetBinContent(9)) << " & " << round(hists["100muprompt"]->GetBinContent(9)) << " & " << round(hists["120muprompt"]->GetBinContent(9)) << " & " << round(hists["400muprompt"]->GetBinContent(9)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & 0jet & " << round(hists["40muprompt"]->GetBinContent(10)) << " & " << round(hists["50muprompt"]->GetBinContent(10)) << " & " << round(hists["60muprompt"]->GetBinContent(10)) << " & " << round(hists["80muprompt"]->GetBinContent(10)) << " & " << round(hists["100muprompt"]->GetBinContent(10)) << " & " << round(hists["120muprompt"]->GetBinContent(10)) << " & " << round(hists["400muprompt"]->GetBinContent(10)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & 1jet & " << round(hists["40muprompt"]->GetBinContent(11)) << " & " << round(hists["50muprompt"]->GetBinContent(11)) << " & " << round(hists["60muprompt"]->GetBinContent(11)) << " & " << round(hists["80muprompt"]->GetBinContent(11)) << " & " << round(hists["100muprompt"]->GetBinContent(11)) << " & " << round(hists["120muprompt"]->GetBinContent(11)) << " & " << round(hists["400muprompt"]->GetBinContent(11)) << " \\\\ " << endl;
    cout << "& 0 $\\mu$ & $\\geq$2jet & " << round(hists["40muprompt"]->GetBinContent(12)) << " & " << round(hists["50muprompt"]->GetBinContent(12)) << " & " << round(hists["60muprompt"]->GetBinContent(12)) << " & " << round(hists["80muprompt"]->GetBinContent(12)) << " & " << round(hists["100muprompt"]->GetBinContent(12)) << " & " << round(hists["120muprompt"]->GetBinContent(12)) << " & " << round(hists["400muprompt"]->GetBinContent(12)) << " \\\\ " << endl;
    cout << "\\hline" << endl;
    cout << "\\end{tabular}" << endl;
    cout << "}" << endl;
    cout << "\\end{center}" << endl;
    cout << "\\end{frame}" << endl << endl;

    cout << "\\begin{frame} " << endl;
    cout << "\\frametitle{eeqq channel}" << endl;
    cout << "\\begin{center}" << endl;
    cout << "\\scalebox{0.85}{" << endl;
    cout << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|}" << endl;
    cout << "\\multicolumn{3}{|c|}{$M_N$ [GeV]} & 2(d) & 3(d) & 5(d) & 10(p) & 20(p) & 30(p) \\\\" << endl;
    cout << "\\hline" << endl;
    cout << "& 1iso e & 0jet & " << round(hists["2edisplaced"]->GetBinContent(1)) << " & " << round(hists["3edisplaced"]->GetBinContent(1)) << " & " << round(hists["5edisplaced"]->GetBinContent(1)) << " & " << round(hists["10eprompt"]->GetBinContent(1)) << " & " << round(hists["20eprompt"]->GetBinContent(1)) << " & " << round(hists["30eprompt"]->GetBinContent(1)) << " \\\\ " << endl;
    cout << "& 1iso e & 1jet & " << round(hists["2edisplaced"]->GetBinContent(2)) << " & " << round(hists["3edisplaced"]->GetBinContent(2)) << " & " << round(hists["5edisplaced"]->GetBinContent(2)) << " & " << round(hists["10eprompt"]->GetBinContent(2)) << " & " << round(hists["20eprompt"]->GetBinContent(2)) << " & " << round(hists["30eprompt"]->GetBinContent(2)) << " \\\\ " << endl;
    cout << "& 1iso e & $\\geq$2jet & " << round(hists["2edisplaced"]->GetBinContent(3)) << " & " << round(hists["3edisplaced"]->GetBinContent(3)) << " & " << round(hists["5edisplaced"]->GetBinContent(3)) << " & " << round(hists["10eprompt"]->GetBinContent(3)) << " & " << round(hists["20eprompt"]->GetBinContent(3)) << " & " << round(hists["30eprompt"]->GetBinContent(3)) << " \\\\ " << endl;
    cout << "& 1non-iso e & 0jet & " << round(hists["2edisplaced"]->GetBinContent(4)) << " & " << round(hists["3edisplaced"]->GetBinContent(4)) << " & " << round(hists["5edisplaced"]->GetBinContent(4)) << " & " << round(hists["10eprompt"]->GetBinContent(4)) << " & " << round(hists["20eprompt"]->GetBinContent(4)) << " & " << round(hists["30eprompt"]->GetBinContent(4)) << " \\\\ " << endl;
    cout << "1iso e & 1non-iso e & 1jet & " << round(hists["2edisplaced"]->GetBinContent(5)) << " & " << round(hists["3edisplaced"]->GetBinContent(5)) << " & " << round(hists["5edisplaced"]->GetBinContent(5)) << " & " << round(hists["10eprompt"]->GetBinContent(5)) << " & " << round(hists["20eprompt"]->GetBinContent(5)) << " & " << round(hists["30eprompt"]->GetBinContent(5)) << " \\\\ " << endl;
    cout << "& 1non-iso e & $\\geq$2jet & " << round(hists["2edisplaced"]->GetBinContent(6)) << " & " << round(hists["3edisplaced"]->GetBinContent(6)) << " & " << round(hists["5edisplaced"]->GetBinContent(6)) << " & " << round(hists["10eprompt"]->GetBinContent(6)) << " & " << round(hists["20eprompt"]->GetBinContent(6)) << " & " << round(hists["30eprompt"]->GetBinContent(6)) << " \\\\ " << endl;
    cout << "& 1displ e & 0jet & " << round(hists["2edisplaced"]->GetBinContent(7)) << " & " << round(hists["3edisplaced"]->GetBinContent(7)) << " & " << round(hists["5edisplaced"]->GetBinContent(7)) << " & " << round(hists["10eprompt"]->GetBinContent(7)) << " & " << round(hists["20eprompt"]->GetBinContent(7)) << " & " << round(hists["30eprompt"]->GetBinContent(7)) << " \\\\ " << endl;
    cout << "& 1displ e & 1jet & " << round(hists["2edisplaced"]->GetBinContent(8)) << " & " << round(hists["3edisplaced"]->GetBinContent(8)) << " & " << round(hists["5edisplaced"]->GetBinContent(8)) << " & " << round(hists["10eprompt"]->GetBinContent(8)) << " & " << round(hists["20eprompt"]->GetBinContent(8)) << " & " << round(hists["30eprompt"]->GetBinContent(8)) << " \\\\ " << endl;
    cout << "& 1displ e & $\\geq$2jet & " << round(hists["2edisplaced"]->GetBinContent(9)) << " & " << round(hists["3edisplaced"]->GetBinContent(9)) << " & " << round(hists["5edisplaced"]->GetBinContent(9)) << " & " << round(hists["10eprompt"]->GetBinContent(9)) << " & " << round(hists["20eprompt"]->GetBinContent(9)) << " & " << round(hists["30eprompt"]->GetBinContent(9)) << " \\\\ " << endl;
    cout << "& 0 e & 0jet &" << round(hists["2edisplaced"]->GetBinContent(10)) << " & " << round(hists["3edisplaced"]->GetBinContent(10)) << " & " << round(hists["5edisplaced"]->GetBinContent(10)) << " & " << round(hists["10eprompt"]->GetBinContent(10)) << " & " << round(hists["20eprompt"]->GetBinContent(10)) << " & " << round(hists["30eprompt"]->GetBinContent(10)) << " \\\\ " << endl;
    cout << "& 0 e & 1jet &" << round(hists["2edisplaced"]->GetBinContent(11)) << " & " << round(hists["3edisplaced"]->GetBinContent(11)) << " & " << round(hists["5edisplaced"]->GetBinContent(11)) << " & " << round(hists["10eprompt"]->GetBinContent(11)) << " & " << round(hists["20eprompt"]->GetBinContent(11)) << " & " << round(hists["30eprompt"]->GetBinContent(11)) << " \\\\ " << endl;
    cout << "& 0 e & $\\geq$2jet &" << round(hists["2edisplaced"]->GetBinContent(12)) << " & " << round(hists["3edisplaced"]->GetBinContent(12)) << " & " << round(hists["5edisplaced"]->GetBinContent(12)) << " & " << round(hists["10eprompt"]->GetBinContent(12)) << " & " << round(hists["20eprompt"]->GetBinContent(12)) << " & " << round(hists["30eprompt"]->GetBinContent(12)) << " \\\\ " << endl;
    cout << "\\hline" << endl;
    cout << "\\end{tabular}" << endl;
    cout << "}" << endl;
    cout << "\\end{center}" << endl;
    cout << "\\end{frame}" << endl << endl;

    cout << "\\begin{frame} " << endl;
    cout << "\\frametitle{eeqq channel}" << endl;
    cout << "\\begin{center}" << endl;
    cout << "\\scalebox{0.85}{" << endl;
    cout << "\\begin{tabular}{|c|c|c|c|c|c|c|c|c|c|}" << endl;
    cout << "\\multicolumn{3}{|c|}{$M_N$ [GeV]} & 40 & 50 & 60 & 80 & 100 & 120 & 200 \\\\" << endl;
    cout << "\\hline" << endl;
    cout << "& 1iso e & 0jet & " << round(hists["40eprompt"]->GetBinContent(1)) << " & " << round(hists["50eprompt"]->GetBinContent(1)) << " & " << round(hists["60eprompt"]->GetBinContent(1)) << " & " << round(hists["80eprompt"]->GetBinContent(1)) << " & " << round(hists["100eprompt"]->GetBinContent(1)) << " & " << round(hists["120eprompt"]->GetBinContent(1)) << " & " << round(hists["200eprompt"]->GetBinContent(1)) << " \\\\ " << endl;
    cout << "& 1iso e & 1jet & " << round(hists["40eprompt"]->GetBinContent(2)) << " & " << round(hists["50eprompt"]->GetBinContent(2)) << " & " << round(hists["60eprompt"]->GetBinContent(2)) << " & " << round(hists["80eprompt"]->GetBinContent(2)) << " & " << round(hists["100eprompt"]->GetBinContent(2)) << " & " << round(hists["120eprompt"]->GetBinContent(2)) << " & " << round(hists["200eprompt"]->GetBinContent(2)) << " \\\\ " << endl;
    cout << "& 1iso e & $\\geq$2jet & " << round(hists["40eprompt"]->GetBinContent(3)) << " & " << round(hists["50eprompt"]->GetBinContent(3)) << " & " << round(hists["60eprompt"]->GetBinContent(3)) << " & " << round(hists["80eprompt"]->GetBinContent(3)) << " & " << round(hists["100eprompt"]->GetBinContent(3)) << " & " << round(hists["120eprompt"]->GetBinContent(3)) << " & " << round(hists["200eprompt"]->GetBinContent(3)) << " \\\\ " << endl;
    cout << "& 1non-iso e & 0jet & " << round(hists["40eprompt"]->GetBinContent(4)) << " & " << round(hists["50eprompt"]->GetBinContent(4)) << " & " << round(hists["60eprompt"]->GetBinContent(4)) << " & " << round(hists["80eprompt"]->GetBinContent(4)) << " & " << round(hists["100eprompt"]->GetBinContent(4)) << " & " << round(hists["120eprompt"]->GetBinContent(4)) << " & " << round(hists["200eprompt"]->GetBinContent(4)) << " \\\\ " << endl;
    cout << "1iso e & 1non-iso e & 1jet & " << round(hists["40eprompt"]->GetBinContent(5)) << " & " << round(hists["50eprompt"]->GetBinContent(5)) << " & " << round(hists["60eprompt"]->GetBinContent(5)) << " & " << round(hists["80eprompt"]->GetBinContent(5)) << " & " << round(hists["100eprompt"]->GetBinContent(5)) << " & " << round(hists["120eprompt"]->GetBinContent(5)) << " & " << round(hists["200eprompt"]->GetBinContent(5)) << " \\\\ " << endl;
    cout << "& 1non-iso e & $\\geq$2jet & " << round(hists["40eprompt"]->GetBinContent(6)) << " & " << round(hists["50eprompt"]->GetBinContent(6)) << " & " << round(hists["60eprompt"]->GetBinContent(6)) << " & " << round(hists["80eprompt"]->GetBinContent(6)) << " & " << round(hists["100eprompt"]->GetBinContent(6)) << " & " << round(hists["120eprompt"]->GetBinContent(6)) << " & " << round(hists["200eprompt"]->GetBinContent(6)) << " \\\\ " << endl;
    cout << "& 1displ e & 0jet & " << round(hists["40eprompt"]->GetBinContent(7)) << " & " << round(hists["50eprompt"]->GetBinContent(7)) << " & " << round(hists["60eprompt"]->GetBinContent(7)) << " & " << round(hists["80eprompt"]->GetBinContent(7)) << " & " << round(hists["100eprompt"]->GetBinContent(7)) << " & " << round(hists["120eprompt"]->GetBinContent(7)) << " & " << round(hists["200eprompt"]->GetBinContent(7)) << " \\\\ " << endl;
    cout << "& 1displ e & 1jet & " << round(hists["40eprompt"]->GetBinContent(8)) << " & " << round(hists["50eprompt"]->GetBinContent(8)) << " & " << round(hists["60eprompt"]->GetBinContent(8)) << " & " << round(hists["80eprompt"]->GetBinContent(8)) << " & " << round(hists["100eprompt"]->GetBinContent(8)) << " & " << round(hists["120eprompt"]->GetBinContent(8)) << " & " << round(hists["200eprompt"]->GetBinContent(8)) << " \\\\ " << endl;
    cout << "& 1displ e & $\\geq$2jet & " << round(hists["40eprompt"]->GetBinContent(9)) << " & " << round(hists["50eprompt"]->GetBinContent(9)) << " & " << round(hists["60eprompt"]->GetBinContent(9)) << " & " << round(hists["80eprompt"]->GetBinContent(9)) << " & " << round(hists["100eprompt"]->GetBinContent(9)) << " & " << round(hists["120eprompt"]->GetBinContent(9)) << " & " << round(hists["200eprompt"]->GetBinContent(9)) << " \\\\ " << endl;
    cout << "& 0 e & 0jet & " << round(hists["40eprompt"]->GetBinContent(10)) << " & " << round(hists["50eprompt"]->GetBinContent(10)) << " & " << round(hists["60eprompt"]->GetBinContent(10)) << " & " << round(hists["80eprompt"]->GetBinContent(10)) << " & " << round(hists["100eprompt"]->GetBinContent(10)) << " & " << round(hists["120eprompt"]->GetBinContent(10)) << " & " << round(hists["200eprompt"]->GetBinContent(10)) << " \\\\ " << endl;
    cout << "& 0 e & 1jet & " << round(hists["40eprompt"]->GetBinContent(11)) << " & " << round(hists["50eprompt"]->GetBinContent(11)) << " & " << round(hists["60eprompt"]->GetBinContent(11)) << " & " << round(hists["80eprompt"]->GetBinContent(11)) << " & " << round(hists["100eprompt"]->GetBinContent(11)) << " & " << round(hists["120eprompt"]->GetBinContent(11)) << " & " << round(hists["200eprompt"]->GetBinContent(11)) << " \\\\ " << endl;
    cout << "& 0 e & $\\geq$2jet & " << round(hists["40eprompt"]->GetBinContent(12)) << " & " << round(hists["50eprompt"]->GetBinContent(12)) << " & " << round(hists["60eprompt"]->GetBinContent(12)) << " & " << round(hists["80eprompt"]->GetBinContent(12)) << " & " << round(hists["100eprompt"]->GetBinContent(12)) << " & " << round(hists["120eprompt"]->GetBinContent(12)) << " & " << round(hists["200eprompt"]->GetBinContent(12)) << " \\\\ " << endl;
    cout << "\\hline" << endl;
    cout << "\\end{tabular}" << endl;
    cout << "}" << endl;
    cout << "\\end{center}" << endl;
    cout << "\\end{frame}" << endl << endl;
    
    
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