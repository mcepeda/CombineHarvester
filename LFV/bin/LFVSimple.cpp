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

using namespace std;

int main(int argc, char* argv[]){

  printf ("Creating LFV MuTau_had datacards. \n ");

  string folder="lfvauxiliaries/datacards";
  string lumi="goldenJson2p11";
  string inputFile="LFV_2p11fb_mutauhad_1Dic";
  string outputFile="HMuTau_mutauhad_2015_input";
  string dirInput="Maria2015Data";
  bool  binbybin=false;

  if (argc > 1)
    { int count=0;
      for (count = 1; count < argc; count++)
      {
        if(strcmp(argv[count] ,"--i")==0) inputFile=argv[count+1];
        if(strcmp(argv[count] ,"--o")==0) outputFile=argv[count+1];
        if(strcmp(argv[count] ,"--l")==0) lumi=argv[count+1];
        if(strcmp(argv[count] ,"--d")==0) dirInput=argv[count+1];
        if(strcmp(argv[count] ,"--f")==0) folder=argv[count+1];
        if(strcmp(argv[count] ,"--b")==0) binbybin=true;

      }
    }


  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/lfvauxiliaries/shapes/";

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      {-1, "mutau_preselection"},
      {0, "mutau_gg"},
      {1, "mutau_boost"},
      {2, "mutau_vbf"},
    };

/*
  ch::Categories cats = {
      {-1, "mutau"},
      {0, "mutau_gg"},
      {1, "mutau_boost"},
      {2, "mutau_vbf"},
    };
*/

  // ch::Categories is just a typedef of vector<pair<int, string>>
  //! [part1]


  //! [part2]
  vector<string> masses = ch::MassesFromRange("125");
  // Or equivalently, specify the mass points explicitly:
  //    vector<string> masses = {"120", "125", "130", "135"};
  //! [part2]

  //! [part3]
  cb.AddObservations({"*"}, {"HMuTau"}, {"2015"}, {"mutauhad"}, cats);
  //! [part3]

  //! [part4]
  vector<string> bkg_procs = {"ZTauTau", "DY", "Dibosons", "TT","ggHTauTau","vbfHTauTau","Fakes"};
  cb.AddProcesses({"*"}, {"HMuTau"}, {"2015"}, {"mutauhad"}, bkg_procs, cats, false);

  vector<string> sig_procs = {"LFVGG", "LFVVBF"};
  cb.AddProcesses(masses, {"HMuTau"}, {"2015"}, {"mutauhad"}, sig_procs, cats, true);
  //! [part4]


  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;


  //! [part5]

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "DY", "TT","Dibosons","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_lumi", "lnN", SystMap<>::init(1.046));

//  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "DY", "TT","Dibosons","ggHTauTau","vbfHTauTau"}}))
//      .AddSyst(cb, "lumi_$ERA", "lnN", SystMap<era>::init
//      ({"2015"}, 1.12));
  //! [part5]

  //! [part6]
  cb.cp().process({"LFVGG","ggHTauTau"})
      .AddSyst(cb, "pdf_gg", "lnN", SystMap<>::init(1.1));
  cb.cp().process({"LFVVBF","vbfHTauTau"})
      .AddSyst(cb, "pdf_vbf", "lnN", SystMap<>::init(1.1));


  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "DY", "TT","Dibosons","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_eff_m", "lnN", SystMap<>::init(1.03));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "DY", "TT","Dibosons","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_eff_tau", "lnN", SystMap<>::init(1.10));

  cb.cp().process({"Fakes"})
      .AddSyst(cb, "norm_taufakes_mutauhad", "lnN", SystMap<>::init(1.3));

 cb.cp().process({"Fakes"})
      .AddSyst(cb,
        "norm_taufakes_mutauhad_uncor_$BIN", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"ZTauTau"})
      .AddSyst(cb, "norm_dy", "lnN", SystMap<>::init(1.1));

 cb.cp().process({"ZTauTau"})
      .AddSyst(cb,
        "norm_dy_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"DY"})
      .AddSyst(cb, "norm_dyother", "lnN", SystMap<>::init(1.1));

 cb.cp().process({"DY"})
      .AddSyst(cb,
        "norm_dyother_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"Dibosons"})
      .AddSyst(cb, "norm_WW ", "lnN", SystMap<>::init(1.1));
  cb.cp().process({"TT"})
      .AddSyst(cb, "norm_tt ", "lnN", SystMap<>::init(1.2));

  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + dirInput+"/"+inputFile+".root",
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
//      aux_shapes + "Maria2015Data/LFV_2p11fb_mutauhad_1Dic.root",
      aux_shapes + dirInput+"/"+inputFile+".root",
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");



  //! [part8]
 if(binbybin){
 auto bbb = ch::BinByBinFactory()
   .SetAddThreshold(0.1)
   .SetFixNorm(true);
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);
  }

  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  boost::filesystem::create_directories(folder);
  boost::filesystem::create_directories(folder + "/"+lumi);


  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
//  TFile output("HMuTau_mutauhad_2015.input.root", "RECREATE");

    string textbinbybin="";
    if(binbybin) textbinbybin="_bbb";

    TFile output((folder + "/"+lumi+"/"+outputFile+"_"+lumi+textbinbybin+".root").c_str(), "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      if(b=="HMuTau_mutauhad_-1_2015"){ cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(folder + "/"+lumi+"/"+"HMuTau_mutauhad_preselection_2015"+textbinbybin+"_m"+m+"_"+lumi+".txt", output);} 
      else{

      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(folder + "/"+lumi+"/"+
          b +textbinbybin+"_m" + m + "_"+lumi+".txt", output);
      
      }

    }
  }

      cb.cp().mass({"125", "*"}).bin({"HMuTau_mutauhad_0_2015","HMuTau_mutauhad_1_2015","HMuTau_mutauhad_2_2015"}).WriteDatacard(folder + "/"+lumi+"/"+"HMuTau_mutauhad_combined_2015"+textbinbybin+"_m125_"+lumi+".txt", output);


  //! [part9]

}
