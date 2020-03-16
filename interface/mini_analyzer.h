/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mini Analyzer class, meant to operate on files created by BkgEstimator.
// Class performs any kind of operations necessary for background prediction, either data-driven, straight from MC if prompt, or whatever I come up with 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef mini_analyzer_h
#define mini_analyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <string>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TSystem.h>
#include <TGraphAsymmErrors.h>

// helper functions
#include "../interface/helper_histo_functions.h"
#include "../interface/BkgEstimator.h"
#include "../interface/bTagWP.h"
#include "../helpertools/LorentzVector/LorentzVector.h"

class mini_analyzer {
    public :
        BkgEstimator event;
        TString sr_flavor;
        bool isData;
        std::map<TString, TH1*> hists;
        std::map<TString, TH2*> hists2D;

        //For ABCD method based on PFN output and dphi
        std::vector<TString> ABCDtags;
        bool isPrompt;
        bool baseline_cutphill;
        bool baseline_cutmll;
        bool baseline_cutphiORmll;
        bool baseline_cutmlSV;

        //General functions
        mini_analyzer(TString filename);
        ~mini_analyzer();

        void analyze(int max_entries, int partition, int partitionjobnumber);

        void ABCD_ratios();
        void calculate_ratio(TString numerator_tag, TString denominator_tag, TString ratio_tag);
        void apply_ratio(TString ratio_tag, TString histo_tag, TString target_tag);

        void set_signal_regions();

        void add_histograms();
        void add_fraction_histograms(TString prefix);
        void add_standard_histograms(TString prefix);
        void add_pfn_histograms(TString prefix);

        void fill_histograms();
        void fill_fraction_histograms(TString prefix, double event_weight);
        void fill_standard_histograms(TString prefix, double event_weight);
        void fill_pfn_histograms(TString prefix, double event_weight, unsigned i);

        void sum_quad_histograms();
        void sum_histograms_based_on_tags(TString base_tag, TString second_tags, TString target_tag);
        double get_SRShapebin(double PVSVdist_2D, double SV_mass);
};

#endif