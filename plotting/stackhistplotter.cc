#include "stackhistplotter.h"

# ifndef __CINT__
int main(int argc, char * argv[])
{
    // set general plot style
    setTDRStyle();
    gROOT->ForceStyle();

    // Argument 1: specific directory for plots (related to x in dilep_x.root or specific set of plots)
    // Argement 2 - (n+1)/2: name of root input files -> separate them between HNL and background based on filename
    // Argument (n+3)/2 - n: legends associated to samples
    int i_rootfiles = 2;
    int i_legends   = argc/2 + 1;

    std::vector<TFile*>  files_signal;
    std::vector<TFile*>  files_bkg;
    std::vector<TFile*>  files_data;
    for(int i = i_rootfiles; i < i_legends; i++){
        TString filename = (TString)argv[i];
        if(filename.Index("_HeavyNeutrino_lljj_") != -1) files_signal.push_back(TFile::Open(filename));
        else if(filename.Index("_Background_") != -1) files_bkg.push_back(TFile::Open(filename));
        else if(filename.Index("_Run201") != -1) files_data.push_back(TFile::Open(filename));
    }
    std::vector<TString> legends_signal;
    std::vector<TString> legends_bkg;
    std::vector<TString> legends_data;
    for(int i = i_legends; i < argc; i++){
        TString legendname = (TString)argv[i];
        if(legendname.Index("HNL") != -1) legends_signal.push_back(adjust_legend(legendname));
        else if(legendname.Index("201") != -1 or legendname.Index("Data") != -1) legends_data.push_back(adjust_legend(legendname));
        else legends_bkg.push_back(adjust_legend(legendname));
    }

    // determine whether the samplelist wants plotting with data or without
    bool withdata = false;
    if(files_data.size() == 1){
        std::cout << "Making plots with data" << std::endl;
        withdata = true;
    } else if(files_data.size() > 1){ 
        std::cout << "Error: giving more than one data file!" << std::endl; 
        return 1; 
    }


    // Name of directory where plots will end up
    TString specific_dir = (TString)argv[1];
    TString general_pathname = make_general_pathname("stacks/", specific_dir + "/");

    TCanvas* c = new TCanvas("c","",700,700);
    c->cd();

    // Make the pad that will contain the plot
    TPad* pad  = new TPad("pad", "", 0., 0., 1., 1.);
    pad->Draw();
    pad->cd();

    TLegend legend = get_legend(0.18, 0.84, 0.95, 0.93, 3);

    // Get margins and make the CMS and lumi basic latex to print on top of the figure
    TString CMStext   = "#bf{CMS} #scale[0.8]{#it{Preliminary}}";
    TString lumitext  = "35.9 fb^{-1} (13 TeV)";
    float leftmargin  = pad->GetLeftMargin();
    float topmargin   = pad->GetTopMargin();
    float rightmargin = pad->GetRightMargin();
    TLatex CMSlatex  = get_latex(0.8*topmargin, 11, 42);
    TLatex lumilatex = get_latex(0.6*topmargin, 31, 42);

    TIter next(files_bkg.back()->GetListOfKeys());
    TKey* key;
    while(key = (TKey*)next()){
        legend.Clear();

        TClass *cl = gROOT->GetClass(key->GetClassName());

        // -- TH1 --
        if (cl->InheritsFrom("TH1") and ! cl->InheritsFrom("TH2")){ // second requirement is because TH2 also inherits from TH1
            
            // Get a reference histogram for the name, then get all histograms in  a vector
            TH1F*   sample_hist_ref = (TH1F*)key->ReadObj();
            TString histname   = sample_hist_ref->GetName();
            std::cout << histname << std::endl;
            
            if(sample_hist_ref->GetMaximum() == 0 and withdata) continue; // bkg histogram is empty and there is no data file to plot
            
            // get data histogram and fill legend
            std::cout << "ok1" << std::endl;
            TH1F* data_hist;
            std::cout << "ok2" << std::endl;
            if(withdata){ 
            std::cout << "ok3" << std::endl;
                data_hist = (TH1F*) files_data[0]->Get(histname);
                std::cout << data_hist << std::endl;
            std::cout << "ok4" << std::endl;
                legend.AddEntry(data_hist, legends_data[0], "pl");
            std::cout << "ok5" << std::endl;
                if(histname.Index("_CR") == -1) continue; // Only print Control region plots for data
            std::cout << "ok6" << std::endl;
                if(data_hist == 0 or data_hist->GetMaximum() == 0) continue; // data histogram is empty
            std::cout << "ok7" << std::endl;
            }
            std::cout << "ok8" << std::endl;
            
            // get background histograms and fill legend
            THStack* hists_bkg = new THStack("stack_bkg", "");
            for(int i = 0; i < files_bkg.size(); i++){
                TH1* hist = (TH1*)files_bkg[i]->Get(histname);
                if(hist->GetMaximum() > 0){
                    int color = get_color(legends_bkg[i]);
                    hist->SetFillStyle(1001);
                    hist->SetFillColor(color);
                    hist->SetLineColor(color);
                    hists_bkg->Add(hist);
                    legend.AddEntry(hist, legends_bkg[i], "f");
                }
            }
            
            // get signal histograms and fill legend
            THStack* hists_signal = new THStack("stack_signal", "");
            for(int i = 0; i < files_signal.size(); i++){
                TH1* hist = (TH1*)files_signal[i]->Get(histname);
                if(hist->GetMaximum() > 0){
                    int color = get_color(legends_signal[i]);
                    hist->SetLineColor(color);
                    hists_signal->Add(hist);
                    legend.AddEntry(hist, legends_signal[i], "l");
                }
            }

            // get plot specific pathnames
            TString pathname_lin    = make_plotspecific_pathname(histname, general_pathname, "lin/");
            TString pathname_log    = make_plotspecific_pathname(histname, general_pathname, "log/");
            
            // set x range to log if necessary
            int xlog = (histname.Index("xlog") == -1)? 0 : 1;
            pad->SetLogx(xlog);
            if(xlog){
                TIter stack_iterator(hists_bkg->GetHists());
                TObject* sample_hist = 0;
                while(sample_hist = stack_iterator()){
                    divide_by_binwidth((TH1F*) sample_hist);
                }
                stack_iterator = hists_signal->GetHists();
                while(sample_hist = stack_iterator()){
                    divide_by_binwidth((TH1F*) sample_hist);
                }
                if(withdata) divide_by_binwidth(data_hist);
            }


            // Draw lin version
            pad->Clear();
            pad->SetLogy(0);

            hists_bkg->Draw("hist");
            if(withdata) hists_bkg->SetMaximum(1.25*std::max(hists_bkg->GetMaximum(), std::max(hists_signal->GetMaximum("nostack"), data_hist->GetMaximum())));
            else hists_bkg->SetMaximum(1.25*std::max(hists_bkg->GetMaximum(), hists_signal->GetMaximum("nostack")));
            hists_bkg->SetMinimum(0.);
            if(hists_signal->GetNhists() != 0) hists_signal->Draw("hist nostack same");
            if(withdata) data_hist->Draw("E0 X0 P same");
            legend.Draw("same");
            CMSlatex.DrawLatex(leftmargin, 1-0.8*topmargin, CMStext);
            lumilatex.DrawLatex(1-rightmargin, 1-0.8*topmargin, lumitext);
            
            pad->Modified();
            c->Print(pathname_lin + histname + ".pdf");

            // Draw log version
            pad->Clear();
            pad->SetLogy(1);

            hists_bkg->Draw("hist");
            if(withdata) hists_bkg->SetMaximum(10*std::max(hists_bkg->GetMaximum(), std::max(hists_signal->GetMaximum("nostack"), data_hist->GetMaximum())));
            else hists_bkg->SetMaximum(10*std::max(hists_bkg->GetMaximum(), hists_signal->GetMaximum("nostack")));
            hists_bkg->SetMinimum(0.1);
            if(hists_signal->GetNhists() != 0) hists_signal->Draw("hist nostack same");
            if(withdata) data_hist->Draw("E0 X0 P same");
            legend.Draw("same");
            CMSlatex.DrawLatex(leftmargin, 1-0.8*topmargin, CMStext);
            lumilatex.DrawLatex(1-rightmargin, 1-0.8*topmargin, lumitext);
            
            pad->Modified();
            c->Print(pathname_log + histname + ".pdf");


            // Calculate and draw efficiencies as TGraphAsymmErrors in a multigraph
            if(histname.Index("eff_num") != -1){
                pad->Clear();
                pad->SetLogy(0);
                legend.Clear();
                
                TMultiGraph* multigraph = new TMultiGraph();
                multigraph->SetTitle((TString)";" + sample_hist_ref->GetXaxis()->GetTitle() + ";Eff.");
                for(int i = 0; i < files_bkg.size(); i++){
                    TGraphAsymmErrors* graph = new TGraphAsymmErrors((TH1F*) files_bkg[i]->Get(histname), (TH1F*) files_bkg[i]->Get(histname(0, histname.Index("eff_num")+4) + "den"), "cp");
                    graph->SetMarkerColor(get_color(legends_bkg[i]));
                    multigraph->Add(graph);
                    legend.AddEntry(graph, legends_bkg[i], "pl");
                }
                for(int i = 0; i < files_signal.size(); i++){
                    TGraphAsymmErrors* graph = new TGraphAsymmErrors((TH1F*) files_signal[i]->Get(histname), (TH1F*) files_signal[i]->Get(histname(0, histname.Index("eff_num")+4) + "den"), "cp");
                    graph->SetMarkerColor(get_color(legends_signal[i]));
                    multigraph->Add(graph);
                    legend.AddEntry(graph, legends_signal[i], "pl");
                }
                if(withdata){
                    TGraphAsymmErrors* graph = new TGraphAsymmErrors((TH1F*) files_data[0]->Get(histname), (TH1F*) files_data[0]->Get(histname(0, histname.Index("eff_num")+4) + "den"), "cp");
                    graph->SetMarkerColor(get_color(legends_data[0]));
                    multigraph->Add(graph);
                    legend.AddEntry(graph, legends_data[0], "pl");
                }

                multigraph->Draw("AP");
                multigraph->SetMinimum(0.);
                multigraph->SetMaximum(1.25);

                legend.Draw("same");
                CMSlatex.DrawLatex(leftmargin, 1-0.8*topmargin, CMStext);
                lumilatex.DrawLatex(1-rightmargin, 1-0.8*topmargin, lumitext);

                pad->Modified();
                c->Print(pathname_lin + histname(0, histname.Index("eff_num") + 3) + ".pdf");
            }
        }
    }
}
# endif