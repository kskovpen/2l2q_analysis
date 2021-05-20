/////////////////////////////////////////////////////////
// CombinePrep prepares datacards for the Combine Tool //
/////////////////////////////////////////////////////////


#ifndef CombineDatacardPrep_h
#define CombineDatacardPrep_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>

#include <iostream>
#include <fstream>
#include <cmath>

#include <string>
#include <utility>
#include <TSystem.h>

#include "../../plotting/helper_plotter_functions.h"

# ifndef __CINT__ 
int main(int argc, char * argv[]);
void makequadFile(TString quad_filename, TH1F* hist_signal, TH1F* hist_data, std::vector<TH1F*> hists_bkg, const std::string& sigName, const std::string& dataName, const std::vector<std::string>& bkgNames);
void makeShapeSRFile(TString shapeSR_filename, TH1F* hist_signal, TH1F* hist_data, std::vector<TH1F*> hists_bkg, const std::string& sigName, const std::string& dataName, const std::vector<std::string>& bkgNames, std::vector<TH1F*> hist_signal_sys, std::vector<std::vector<TH1F*>> hists_bkg_sys, const std::vector<std::string>& sigName_sys, const std::vector<std::vector<std::string>>& bkgNames_sys);
void printDataCard(const std::string& cardName, const double obsYield, const double sigYield, const std::string& sigName, const double* bkgYield, const unsigned nBkg, const std::string* bkgNames, const std::vector<std::vector<double> >& systUnc = std::vector< std::vector< double > >(), const unsigned nSyst = 0, const std::string* systNames = nullptr, const std::string* systDist = nullptr, const bool shapeCard = false, const std::string& shapeFileName = "", const bool autoMCStats = false);
#endif

//class CombinePrep {
//    public :
//        CombinePrep();
//        ~CombinePrep();
//        void printDataCard(const std::string& cardName, const double obsYield, const double sigYield, const std::string& sigName, const double* bkgYield, const unsigned nBkg, const std::string* bkgNames, const std::vector<std::vector<double> >& systUnc = std::vector< std::vector< double > >(), const unsigned nSyst = 0, const std::string* systNames = nullptr, const std::string* systDist = nullptr, const bool shapeCard = false, const std::string& shapeFileName = "", const bool autoMCStats = false);
//};
#endif
