#include <TROOT.h>
#include <TFile.h>
#include <TF1.h>
#include <TH2.h>
#include <TH1.h>
#include <THistPainter.h>
#include <TGraphAsymmErrors.h>
#include <TLatex.h>
#include <TTree.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPad.h>
#include <TCanvas.h>
#include <iostream>
#include <string>
#include <cmath>
#include <TRandom3.h>
#include <TKey.h>
#include <TSystem.h>
#include <THStack.h>

#include "../interface/plotterfunctions.h"
#include "../interface/stackplotterfunctions.h"

using namespace std;

std::map<TString, TH1*>::iterator it2;

// Argument 1 in argv will be the partition
// Argument 2 in argv will be the partitionjobnumber
// Argument 3 - ((argc + 3)/2 - 1)  will be the root sample files
// Argument ((argc + 3)/2) - (argc - 1) will be the legends for the sample files
# ifndef __CINT__
int main(int argc, char * argv[])
{
    TString pathname                = "/user/bvermass/public/2l2q_analysis/plots/stacks/";
    TString pathname_with_signal    = "/user/bvermass/public/2l2q_analysis/plots/stacks_with_signal/";

    // define canvas and legend(s)
    TCanvas *c = new TCanvas("c","",700,700);
    gStyle->SetPalette(55);
    gStyle->SetOptStat(0);
    gPad->SetTicks(1,1);

    TLegend lgendrup(.6,.6,.87,.87);
    lgendrup.SetBorderSize(0);
    lgendrup.SetFillStyle(0);
   

    // put all background files in a map that can be looped over
    std::map<TString, TFile*> files;
    for(int i = 3; i < (argc + 3)/2; i++){
        TString name = (TString)argv[i];
        TString fullname = to_string(i) + "_" + name(name.Index("full_analyzer/") + 14, name.Index(".root") - name.Index("full_analyzer") - 14);
        files[fullname] = TFile::Open(argv[i]);
        cout << fullname << endl;
    }
    // signal files
    TFile * HNL3GeV_e_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-2_V-0.0141421356237_e_massiveAndCKM_LO_displaced.root");
    TFile * HNL3GeV_mu_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-2_V-0.0141421356237_mu_massiveAndCKM_LO_displaced.root");
    TFile * HNL7GeV_e_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-5_V-0.00707106781187_e_massiveAndCKM_LO_displaced.root");
    TFile * HNL7GeV_mu_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-5_V-0.00707106781187_mu_massiveAndCKM_LO_displaced.root");
    TFile * HNL10GeV_e_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-8_V-0.004472135955_e_massiveAndCKM_LO_displaced.root");
    TFile * HNL10GeV_mu_file = TFile::Open("/user/bvermass/public/2l2q_analysis/histograms/full_analyzer/hists_full_analyzer_HeavyNeutrino_lljj_M-8_V-0.004472135955_mu_massiveAndCKM_LO_displaced.root");


    std::map<TString, TH1*> hists;
    std::map<TString, TH1*> signals;
    TString nameforkey = (TString)argv[(argc + 3)/2 - 1];
    TString fullnameforkey = to_string((argc + 3)/2 - 1) + "_" + nameforkey(nameforkey.Index("full_analyzer/") + 14, nameforkey.Index(".root") - nameforkey.Index("full_analyzer") - 14);
    TIter next(files[fullnameforkey]->GetListOfKeys());
    TKey * key;

    Int_t partition = atoi(argv[1]);
    Int_t partitionjobnumber = atoi(argv[2]);
    Int_t counter_begin = floor(1.0 * partitionjobnumber / partition * files[fullnameforkey]->GetNkeys());
    Int_t counter_end   = floor(1.0 * (partitionjobnumber + 1) / partition * files[fullnameforkey]->GetNkeys());
    Int_t counter       = 0;

    // loop over histograms
    while ((key = (TKey*)next())) {
        if(counter >= counter_begin and counter < counter_end){
            hists.clear();
            signals.clear();
    
            // get correct reference histogram
            TClass *cl = gROOT->GetClass(key->GetClassName());
            if (!cl->InheritsFrom("TH1") or cl->InheritsFrom("TH2")) continue; //don't plot 2d plots or TGraphAsymmErrors(efficiencies) for now ( don't know why a stack plot of this would be useful
            TH1F *h_ref = (TH1F*)key->ReadObj(); //h_ref is the reference histogram that knows the name etc. of the histogram
            TString histname = h_ref->GetName();
            
            if(h_ref->GetMaximum() == 0) continue; //if h_ref is empty, no need to draw the histogram
            if(histname.Index("ctau") != -1) continue; // if this is a histogram of ctau, I shouldn't plot it for background (and it gave a segmentation fault for some reason)

            TString pathname_lin                = make_pathname(histname, pathname, "lin");
            TString pathname_log                = make_pathname(histname, pathname, "log");
            TString pathname_with_signal_lin    = make_pathname(histname, pathname_with_signal, "lin");
            TString pathname_with_signal_log    = make_pathname(histname, pathname_with_signal, "log");
            gSystem->Exec("mkdir -p " + pathname_lin);
            gSystem->Exec("mkdir -p " + pathname_log);
            gSystem->Exec("mkdir -p " + pathname_with_signal_lin);
            gSystem->Exec("mkdir -p " + pathname_with_signal_log);

            // find flavor e or mu
            TString flavor;
            TString checkflavor = histname;                 //improve else option of flavor!
            if(checkflavor.Index("_e_") != -1) flavor = "e";
            else if(checkflavor.Index("_mu_") != -1) flavor = "mu";
            else flavor = "e";

            // get background and signal histograms
            for(int i = 3; i < (argc + 3)/2; i++){
                TString name = (TString)argv[i];
                TString fullname = to_string(i) + "_" + name(name.Index("full_analyzer/") + 14, name.Index(".root") - name.Index("full_analyzer") - 14) ;
                hists[fullname] = (TH1F*) files[fullname]->Get(histname);
            }
            if(flavor == "e"){
                signals["1_3GeV"] = (TH1F*) HNL3GeV_e_file->Get(histname);
                signals["2_7GeV"] = (TH1F*) HNL7GeV_e_file->Get(histname);
                signals["3_10GeV"] = (TH1F*) HNL10GeV_e_file->Get(histname);
            }else if(flavor == "mu"){
                signals["1_3GeV"] = (TH1F*) HNL3GeV_mu_file->Get(histname);
                signals["2_7GeV"] = (TH1F*) HNL7GeV_mu_file->Get(histname);
                signals["3_10GeV"] = (TH1F*) HNL10GeV_mu_file->Get(histname);
            }

            // style of signal
            markerstyle((TH1F*)signals["1_3GeV"], "red");
            markerstyle((TH1F*)signals["2_7GeV"], "magenta");
            markerstyle((TH1F*)signals["3_10GeV"], "blue");
            mapmarkerstyle(hists);

            THStack *stack = new THStack("stack", histname);
            int i = (argc + 3)/2;//to iterate over legends    CHANGE THIS TO LET A FUNCTION DECIDE ON THE LEGEND NAME BASED ON THE FILENAME
            lgendrup.Clear();
            for( it2 = hists.begin(); it2 != hists.end(); it2++){
                TH1* h = it2->second;
	            stack->Add(h);
                lgendrup.AddEntry(h, argv[i]);
                i++;
            }

            // x log or lin
            int xlog = (histname.Index("xlog") == -1)? 0 : 1;
            // Events or Eff. in yaxis title
            TString yaxistitle = h_ref->GetYaxis()->GetTitle(); 
            
            cout << histname << endl;
            draw_stack(pathname_lin + histname + ".pdf", c, stack, &lgendrup, h_ref->GetXaxis()->GetTitle(), yaxistitle, 0, xlog, -1, -1, -1, -1, "");
            draw_stack(pathname_log + histname + ".pdf", c, stack, &lgendrup, h_ref->GetXaxis()->GetTitle(), yaxistitle, 1, xlog, -1, -1, 10, -1, "");
            
            lgendrup.AddEntry(signals["1_3GeV"], "HNL 2GeV, c#tau~96.09mm");
            lgendrup.AddEntry(signals["2_7GeV"], "HNL 5GeV, c#tau~2.95mm");
            lgendrup.AddEntry(signals["3_10GeV"], "HNL 8GeV, c#tau~0.62mm");
            
            draw_stack_with_signal(pathname_with_signal_lin + histname + ".pdf", c, stack, signals, "hist", &lgendrup, h_ref->GetXaxis()->GetTitle(), yaxistitle, 0, xlog, -1, -1, -1, -1, "");
            draw_stack_with_signal(pathname_with_signal_log + histname + ".pdf", c, stack, signals, "hist", &lgendrup, h_ref->GetXaxis()->GetTitle(), yaxistitle, 1, xlog, -1, -1, 10, -1, "");
        }
        counter++;
    } 
    cout << "plots finished" << endl;
    return 0;
}
# endif
