#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"

#include "TTree.h"

using namespace std;

int main(int argc, char* argv[]){

  string folder="$CMSSW_BASE/src/lfvauxiliaries/datacards";
  string dir="goldenJson2p11";
  string inputFile="higgsCombine-exp.Asymptotic.mH125.root";

  if (argc > 1)
    { int count=0;
      for (count = 1; count < argc; count++)
      {
        if(strcmp(argv[count] ,"--i")==0) inputFile=argv[count+1];
        if(strcmp(argv[count] ,"--f")==0) folder=argv[count+1];
        if(strcmp(argv[count] ,"--d")==0) dir=argv[count+1];

      }
    }

    TFile input((folder + "/"+dir+"/"+inputFile).c_str(), "READONLY");


  vector<string> proccess = {"ZTauTau", "DY", "Dibosons", "TT","ggHTauTau","vbfHTauTau","Fakes","LFVGG", "LFVVBF"};

/*
  TH1F * histo = (TH1F*)input.Get("


    TTree* limitTree=(TTree*)input.Get("limit"); limitTree->SetName("limitTree");
    Float_t  quantileExpected;
    Double_t limit; 
    Double_t limitForCout[5];
    limitTree->SetBranchAddress("limit",&limit);
    limitTree->SetBranchAddress("quantileExpected",&quantileExpected);
    Long64_t nentries = limitTree->GetEntries();
    for (Long64_t i=0;i<nentries;i++) {
                       limitTree->GetEntry(i);
                        //std::cout<<limit<<"  "<<quantileExpected<<endl;
                        limitForCout[i]=limit;           
                        }


    TTree* limitTreeSig=(TTree*)inputSig.Get("limit"); limitTreeSig->SetName("limitTree");
    Double_t sig; 
    limitTreeSig->SetBranchAddress("limit",&sig);
    limitTreeSig->GetEntry(0);                        

    printf("%0.1f",lumi); 
    for (Long64_t i=0;i<nentries;i++) { printf("\t %2.3f",limitForCout[i]);}
    printf("\t %2.3f",sig);
    printf("\n");
*/
}
