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
      {0, "LFV_MuTau_0Jet_1_13TeVMuTau"},
      {1, "LFV_MuTau_1Jet_1_13TeVMuTau"},
      {2, "LFV_MuTau_2Jet_1_13TeVMuTau"},
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
  vector<string> bkg_procs = {"ZTauTau", "Zothers", "Diboson", "TT","T","ggHTauTau","vbfHTauTau","Fakes"};
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

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","T","Diboson","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_lumi", "lnN", SystMap<>::init(1.023));

//  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","Diboson","ggHTauTau","vbfHTauTau"}}))
//      .AddSyst(cb, "lumi_$ERA", "lnN", SystMap<era>::init
//      ({"2015"}, 1.12));
  //! [part5]

  //! [part6]
  cb.cp().process({"LFVGG","ggHTauTau"})
      .AddSyst(cb, "pdf_gg", "lnN", SystMap<>::init(1.1));
  cb.cp().process({"LFVVBF","vbfHTauTau"})
      .AddSyst(cb, "pdf_vbf", "lnN", SystMap<>::init(1.1));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","T","Diboson","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_eff_m", "lnN", SystMap<>::init(1.03));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","T","Diboson","ggHTauTau","vbfHTauTau"}}))
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

  cb.cp().process({"Zothers"})
      .AddSyst(cb, "norm_dyother", "lnN", SystMap<>::init(1.1));

 cb.cp().process({"Zothers"})
      .AddSyst(cb,
        "norm_dyother_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"Diboson"})
      .AddSyst(cb, "norm_WW ", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"TT"})
      .AddSyst(cb, "norm_tt ", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"T"})
      .AddSyst(cb, "norm_t ", "lnN", SystMap<>::init(1.1));

 cb.cp().process({"Diboson"})
      .AddSyst(cb,
        "norm_WW_$BIN", "lnN", SystMap<>::init(1.05));

 cb.cp().process({"TT"})
      .AddSyst(cb,
        "norm_TT_$BIN", "lnN", SystMap<>::init(1.05));

 cb.cp().process({"T"})
      .AddSyst(cb,
        "norm_T_$BIN", "lnN", SystMap<>::init(1.05));


  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","T","Diboson","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_MET_Jes","shape",SystMap<>::init(1.));

//  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","Diboson","ggHTauTau","vbfHTauTau"}}))
//      .AddSyst(cb, "CMS_MET_Ues","shape",SystMap<>::init(1.));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "Zothers", "TT","T","Diboson","ggHTauTau","vbfHTauTau"}}))
      .AddSyst(cb, "CMS_MET_Tes","shape",SystMap<>::init(1.));

  cb.cp().process(ch::JoinStr({{"Fakes"}}))
      .AddSyst(cb, "FakeShapeMuTau","shape",SystMap<>::init(1.));


/*
  double ZTauTauJES[3]   ={1.016,1.041,1.252};
  double ZothersJES[3]   ={1.006,1.012,1.516};
  double TTJES[3]        ={1.115,1.091,1.045};
  double DibosonJES[3]   ={1.076,1.191,1.0};
  double ggHTauTauJES[3] ={1.017,1.009,1.042};
  double vbfHTauTauJES[3]={1.129,1.035,1.023};
  double LFVGGJES[3]     ={1.019,1.023,1.071};
  double LFVVBFJES[3]    ={1.129,1.035,1.055};

  cb.cp().AddSyst(cb, "CMS_JES","lnN",SystMap<process,bin_id>::init 
            ({"ZTauTau"},{0},ZTauTauJES[0]) ({"ZTauTau"},{1},ZTauTauJES[1]) ({"ZTauTau"},{2},ZTauTauJES[2]) 
            ({"Zothers"},{0},ZothersJES[0]) ({"Zothers"},{1},ZothersJES[1]) ({"Zothers"},{2},ZothersJES[2]) 
            ({"TT"},{0},TTJES[0]) ({"TT"},{1},TTJES[1]) ({"TT"},{2},TTJES[2])
            ({"Diboson"},{0},DibosonJES[0]) ({"Diboson"},{1},DibosonJES[1]) ({"Diboson"},{2},DibosonJES[2])
            ({"ggHTauTau"},{0},ggHTauTauJES[0]) ({"ggHTauTau"},{1},ggHTauTauJES[1]) ({"ggHTauTau"},{2},ggHTauTauJES[2])
            ({"vbfHTauTau"},{0},vbfHTauTauJES[0]) ({"vbfHTauTau"},{1},vbfHTauTauJES[1]) ({"vbfHTauTau"},{2},vbfHTauTauJES[2])
            ({"LFVGG"},{0},LFVGGJES[0]) ({"LFVGG"},{1},LFVGGJES[1]) ({"LFVGG"},{2},LFVGGJES[2])
            ({"LFVVBF"},{0},LFVVBFJES[0]) ({"LFVVBF"},{1},LFVVBFJES[1]) ({"LFVVBF"},{2},LFVVBFJES[2])
  );


  double ZTauTauTES[3]   ={1.049,1.021,1.209};
  double ZothersTES[3]   ={1.008,1.045,2.042};
  double TTTES[3]        ={1.300,1.312,1.307};
  double DibosonTES[3]   ={1.038,1.049,1.0};
  double ggHTauTauTES[3] ={1.766,1.712,1.681};
  double vbfHTauTauTES[3]={1.130,1.220,1.220};
  double LFVGGTES[3]     ={1.009,1.031,1.080};
  double LFVVBFTES[3]    ={1.006,1.033,1.049};

  cb.cp().AddSyst(cb, "CMS_TES","lnN",SystMap<process,bin_id>::init 
            ({"ZTauTau"},{0},ZTauTauTES[0]) ({"ZTauTau"},{1},ZTauTauTES[1]) ({"ZTauTau"},{2},ZTauTauTES[2]) 
            ({"Zothers"},{0},ZothersTES[0]) ({"Zothers"},{1},ZothersTES[1]) ({"Zothers"},{2},ZothersTES[2]) 
            ({"TT"},{0},TTTES[0]) ({"TT"},{1},TTTES[1]) ({"TT"},{2},TTTES[2])
            ({"Diboson"},{0},DibosonTES[0]) ({"Diboson"},{1},DibosonTES[1]) ({"Diboson"},{2},DibosonTES[2])
            ({"ggHTauTau"},{0},ggHTauTauTES[0]) ({"ggHTauTau"},{1},ggHTauTauTES[1]) ({"ggHTauTau"},{2},ggHTauTauTES[2])
            ({"vbfHTauTau"},{0},vbfHTauTauTES[0]) ({"vbfHTauTau"},{1},vbfHTauTauTES[1]) ({"vbfHTauTau"},{2},vbfHTauTauTES[2])
            ({"LFVGG"},{0},LFVGGTES[0]) ({"LFVGG"},{1},LFVGGTES[1]) ({"LFVGG"},{2},LFVGGTES[2])
            ({"LFVVBF"},{0},LFVVBFTES[0]) ({"LFVVBF"},{1},LFVVBFTES[1]) ({"LFVVBF"},{2},LFVVBFTES[2])
  );
*/

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
   .SetMergeThreshold(0.5)
   .SetFixNorm(false);
//  bbb.MergeBinErrors(cb.cp().backgrounds());
  bbb.MergeBinErrors(cb.cp().process({"Diboson","ZTauTau","Zothers","T","TT","ggHTauTau","vbfHTauTau"}));
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
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(folder + "/"+lumi+"/"+
          b +textbinbybin+"_m" + m + "_"+lumi+".txt", output);
      
    }
  }

      cb.cp().mass({"125", "*"}).bin({"HMuTau_mutauhad_0_2015","HMuTau_mutauhad_1_2015","HMuTau_mutauhad_2_2015"}).WriteDatacard(folder + "/"+lumi+"/"+"HMuTau_mutauhad_combined_2015"+textbinbybin+"_m125_"+lumi+".txt", output);


  //! [part9]

}
