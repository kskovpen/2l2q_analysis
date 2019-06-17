#include <iostream>

//Include root header files
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

Long64_t get_nentries_from_file(TString filename, TString treename)
{
    TFile *input = new TFile(filename, "open");
    TTree *tree  = (TTree*) input->Get(treename);
    Long64_t entries = tree->GetEntries();
    input->Close();
    return entries;
}

#ifndef __CINT__
int main(int argc, char * argv[])
{
    if(argc == 0) return 1;
    TString file = (TString) argv[1];
    TString treename = "blackJackAndHookers/blackJackAndHookersTree";
    Long64_t entries = get_nentries_from_file(file, treename);
    std::cout << 1000*floor(0.001*entries) << " entries in " << file << "(" << entries << ")" << std::endl;
    return 0;
}
#endif
