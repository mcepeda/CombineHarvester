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

int main(int argc, char* argv[]) {

 string inputFile="lfvem.root";
 bool  binbybin=false;
 bool  newjes=false;

 if (argc > 1)
 { int count=0;
  for (count = 1; count < argc; count++)
  {
   if(strcmp(argv[count] ,"--i")==0) inputFile=argv[count+1];
   if(strcmp(argv[count] ,"--b")==0) binbybin=true;
   if(strcmp(argv[count] ,"--J")==0) newjes=true;

  }
 }

  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  cout<<"test"<<endl;
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";

  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);

  // Here we will just define two categories for an 8TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.
  ch::Categories cats = {
      /*{1, "em_0jet_low"},
      {2, "em_0jet_high"},
      {3, "em_1jet_low"},
      {4, "em_1jet_high"},
      {5, "em_vbf_low"},
      {6, "em_vbf_high"}*/
      {1, "em_0jet"},
      {2, "em_1jet"},
      {3, "em_2jet_gg"},
      {4, "em_2jet_vbf"}
    };
  vector<string> masses = {""};//ch::MassesFromRange("120");//120-135:5");
  //! [part3]
  cb.AddObservations({"*"}, {"lfv"}, {"13TeV"}, {"em"}, cats);

  vector<string> bkg_procs = {"ZTT", "ZL", "VV", "TT", "W", "QCD","ggH125","qqH125"};//VV
  cb.AddProcesses({"*"}, {"lfv"}, {"13TeV"}, {"em"}, bkg_procs, cats, false);

  vector<string> sig_procs = {"LFV_vbf125","LFV_gg125"};
  cb.AddProcesses(masses, {"lfv"}, {"13TeV"}, {"em"}, sig_procs, cats, true);
  //! [part4]


  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;


  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "TT", "ZL", "ggH125", "qqH125", "VV"}}))//VV
      .AddSyst(cb, "CMS_eff_e", "lnN", SystMap<>::init(1.02));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "TT", "ZL", "ggH125", "qqH125", "VV"}}))//VV
      .AddSyst(cb, "CMS_eff_mu", "lnN", SystMap<>::init(1.02));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "TT", "ZL", "VV", "ggH125", "qqH125"}}))//VV
      .AddSyst(cb, "lumi_13TeV", "lnN", SystMap<>::init(1.026));

  cb.cp().process({"LFV_gg125","ggH125"})
      .AddSyst(cb, "pdf_gg", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"LFV_vbf125","qqH125"})
      .AddSyst(cb, "pdf_vbf", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"QCD"})
      .AddSyst(cb, "QCDnorm", "lnN", SystMap<>::init(1.30));

  cb.cp().process({"VV"})
      .AddSyst(cb, "misbtag", "lnN", SystMap<>::init(1.02));

  cb.cp().process({"TT"})
      .AddSyst(cb, "btag", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"W"})
      .AddSyst(cb, "Wnorm", "lnN", SystMap<>::init(1.20));

  cb.cp().process({"ZTT"})
      .AddSyst(cb, "norm_dy", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"ZTT"})
      .AddSyst(cb,"norm_dy_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"ZL"})
      .AddSyst(cb, "norm_dyother", "lnN", SystMap<>::init(1.15));

  cb.cp().process({"ZL"})
      .AddSyst(cb,"norm_dyother_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"VV"})
      .AddSyst(cb, "norm_WW ", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"TT"})
      .AddSyst(cb, "norm_tt ", "lnN", SystMap<>::init(1.06));

  cb.cp().process({"VV"})
      .AddSyst(cb,"norm_WW_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"TT"})
      .AddSyst(cb,
        "norm_TT_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "ZL", "TT", "W", "ggH125", "qqH125", "VV"}}))//VV
      .AddSyst(cb, "CMS_scale_e_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "ZL", "TT", "W", "ggH125", "qqH125", "VV"}}))//VV
      .AddSyst(cb, "CMS_scale_mu_em_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "ZL", "TT", "W", "ggH125", "qqH125", "VV"}}))
      .AddSyst(cb, "CMS_scale_met_13TeV", "shape", SystMap<>::init(1.00));



// Added 27 JES
//
//

  if(newjes){
  TString JESNAMES[27]={"CMS_JetAbsoluteFlavMap","CMS_JetAbsoluteMPFBias","CMS_JetAbsoluteScale", "CMS_JetAbsoluteStat","CMS_JetFlavorQCD", "CMS_JetFragmentation", "CMS_JetPileUpDataMC", "CMS_JetPileUpPtBB", "CMS_JetPileUpPtEC1", "CMS_JetPileUpPtEC2","CMS_JetPileUpPtHF","CMS_JetPileUpPtRef","CMS_JetRelativeBal","CMS_JetRelativeFSR","CMS_JetRelativeJEREC1", "CMS_JetRelativeJEREC2","CMS_JetRelativeJERHF","CMS_JetRelativePtBB","CMS_JetRelativePtEC1","CMS_JetRelativePtEC2","CMS_JetRelativePtHF","CMS_JetRelativeStatEC","CMS_JetRelativeStatFSR", "CMS_JetRelativeStatHF","CMS_JetSinglePionECAL", "CMS_JetSinglePionHCAL","CMS_JetTimePtEta"}; 

  for (int i=0; i<27;i++){
  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "ZL", "TT", "W", "ggH125", "qqH125", "VV"}}))
      .AddSyst(cb, JESNAMES[i].Data(), "shape", SystMap<>::init(1.00));
  }

  }
  else cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "ZL", "TT", "W", "ggH125", "qqH125", "VV"}})).AddSyst(cb, "CMS_scale_j_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().backgrounds().ExtractShapes(
      aux_shapes + inputFile,
      "$BIN/$PROCESS",
      "$BIN/$PROCESS_$SYSTEMATIC");
  cb.cp().signals().ExtractShapes(
      aux_shapes + inputFile,
      "$BIN/$PROCESS$MASS",
      "$BIN/$PROCESS$MASS_$SYSTEMATIC");
  //! [part7]

  //! [part8]

if(binbybin){
  auto bbb = ch::BinByBinFactory()
  .SetAddThreshold(0.1)
  .SetMergeThreshold(0.5)
  .SetFixNorm(false);
  bbb.MergeBinErrors(cb.cp().backgrounds());
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);
}

  /*auto bbb = ch::BinByBinFactory()
    .SetAddThreshold(0.0)
    .SetFixNorm(false);

  //bbb.AddBinByBin(cb.cp().backgrounds(), cb);
  bbb.AddBinByBin(cb.cp().signals(), cb);
  bbb.AddBinByBin(cb.cp().process({"TT"}), cb);
  bbb.AddBinByBin(cb.cp().process({"QCD"}), cb);
  bbb.AddBinByBin(cb.cp().process({"W"}), cb);
  bbb.AddBinByBin(cb.cp().process({"VV"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZTT"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZLL"}), cb);
*/
  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the htt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  TFile output("lfv_em.input.root", "RECREATE");

  // Finally we iterate through each bin,mass combination and write a
  // datacard.
  for (auto b : bins) {
    for (auto m : masses) {
      cout << ">> Writing datacard for bin: " << b << " and mass: " << m
           << "\n";
      // We need to filter on both the mass and the mass hypothesis,
      // where we must remember to include the "*" mass entry to get
      // all the data and backgrounds.
      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(
          b + "_" + m + ".txt", output);
    }
  }
  //! [part9]

}
