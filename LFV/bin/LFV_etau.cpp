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

 string inputFile="lfvet.root";
 bool  binbybin=false;

 if (argc > 1)
 { int count=0;
  for (count = 1; count < argc; count++)
  {
   if(strcmp(argv[count] ,"--i")==0) inputFile=argv[count+1];
   if(strcmp(argv[count] ,"--b")==0) binbybin=true;
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
      {1, "etauh_0jet"},
      {2, "etauh_1jet"},
      {3, "etauh_2jet_gg"},
      {4, "etauh_2jet_vbf"}
    };
  vector<string> masses = {""};//ch::MassesFromRange("120");//120-135:5");
  //! [part3]
  cb.AddObservations({"*"}, {"lfv"}, {"13TeV"}, {"et"}, cats);


  vector<string> bkg_procs = {"ZTauTau", "Zothers", "Diboson", "TT","T", "Fakes", "ggH_htt", "qqH_htt"};//Diboson
  cb.AddProcesses({"*"}, {"lfv"}, {"13TeV"}, {"et"}, bkg_procs, cats, false);

  vector<string> sig_procs = {"LFVVBF125","LFVGG125"};
  cb.AddProcesses(masses, {"lfv"}, {"13TeV"}, {"et"}, sig_procs, cats, true);
  //! [part4]


  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;


  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "TT","T", "Zothers", "ggH_htt", "qqH_htt","Diboson"}}))//Diboson
      .AddSyst(cb, "CMS_eff_e", "lnN", SystMap<>::init(1.02));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))//Diboson
      .AddSyst(cb, "CMS_eff_tau", "lnN", SystMap<>::init(1.05));

  cb.cp().process(ch::JoinStr({sig_procs, {"ZTauTau", "TT","T", "Zothers", "Diboson", "ggH_htt", "qqH_htt"}}))//Diboson
      .AddSyst(cb, "CMS_lumi_13TeV", "lnN", SystMap<>::init(1.026));

/*
 cb.cp().process({"LFVGG125","ggH_htt"})
  .AddSyst(cb, "scale_yr4", "lnN", SystMap<>::init(1.067));

 cb.cp().process({"LFVVBF125","qqH_htt"})
  .AddSyst(cb, "pdf_yr4", "lnN", SystMap<>::init(1.032));
*/

 cb.cp().process({"LFVGG125","ggH_htt"})
  .AddSyst(cb,"QCDScale_ggH", "lnN", SystMap<>::init(1.039));

 cb.cp().process({"LFVVBF125","qqH_htt"})
  .AddSyst(cb,"QCDScale_qqH", "lnN", SystMap<>::init(1.004));

 cb.cp().process({"LFVGG125","ggH_htt"})
  .AddSyst(cb,"pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));

 cb.cp().process({"LFVVBF125","qqH_htt"})
  .AddSyst(cb,"pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));




 cb.cp().AddSyst(cb,"acceptance_scale_gg","lnN",SystMap<process,bin_id>::init 
   ({"LFVGG125","ggH_htt"},{1},1.02)
   ({"LFVGG125","ggH_htt"},{2},0.996)
   ({"LFVGG125","ggH_htt"},{3},0.97)
   ({"LFVGG125","ggH_htt"},{4},0.97)
   );

 cb.cp().AddSyst(cb,"acceptance_pdf_gg","lnN",SystMap<process,bin_id>::init
   ({"LFVGG125","ggH_htt"},{1},1.005)
   ({"LFVGG125","ggH_htt"},{2},1.005)
   ({"LFVGG125","ggH_htt"},{3},0.995)
   ({"LFVGG125","ggH_htt"},{4},0.995)
   );


 cb.cp().AddSyst(cb,"acceptance_scale_vbf","lnN",SystMap<process,bin_id>::init
   ({"LFVVBF125","qqH_htt"},{1},1.01)
   ({"LFVVBF125","qqH_htt"},{2},1.006)
   ({"LFVVBF125","qqH_htt"},{3},0.999)
   ({"LFVVBF125","qqH_htt"},{4},0.999)
   );


 cb.cp().AddSyst(cb,"acceptance_pdf_vbf","lnN",SystMap<process,bin_id>::init
   ({"LFVVBF125","qqH_htt"},{1},1.005)
   ({"LFVVBF125","qqH_htt"},{2},0.99)
   ({"LFVVBF125","qqH_htt"},{3},0.985)
   ({"LFVVBF125","qqH_htt"},{4},0.985)
   );

 //    Uncertainty on BR for HTT @ 125 GeV
cb.cp().process({"ggH_htt","qqH_htt"}).AddSyst(cb,"BR_htt_THU", "lnN", SystMap<>::init(1.017));
cb.cp().process({"ggH_htt","qqH_htt"}).AddSyst(cb,"BR_htt_PU_mq", "lnN", SystMap<>::init(1.0099));
cb.cp().process({"ggH_htt","qqH_htt"}).AddSyst(cb,"BR_htt_PU_alphas", "lnN", SystMap<>::init(1.0062));

  cb.cp().process({"Fakes"})
      .AddSyst(cb, "norm_taufakes_etauhad", "lnN", SystMap<>::init(1.30));
 
  cb.cp().process({"Fakes"})
      .AddSyst(cb,
        "norm_taufakes_etauhad_uncor_$BIN", "lnN", SystMap<>::init(1.1));
 
  cb.cp().process({"ZTauTau"})
      .AddSyst(cb, "norm_ztt", "lnN", SystMap<>::init(1.1));
 
  cb.cp().process({"ZTauTau"})
      .AddSyst(cb,"norm_ztt_$BIN", "lnN", SystMap<>::init(1.05));
 
  cb.cp().process({"Zothers"})
      .AddSyst(cb, "norm_zothers", "lnN", SystMap<>::init(1.12));
 
  cb.cp().process({"Zothers"})
      .AddSyst(cb,"norm_zothers_$BIN", "lnN", SystMap<>::init(1.05));
 
  cb.cp().process({"Diboson"})
      .AddSyst(cb, "norm_Diboson ", "lnN", SystMap<>::init(1.05));
 
  cb.cp().process({"TT"})
      .AddSyst(cb, "norm_TT ", "lnN", SystMap<>::init(1.10));

  cb.cp().process({"Diboson"})
      .AddSyst(cb,"norm_Diboson_$BIN", "lnN", SystMap<>::init(1.05));
 
  cb.cp().process({"TT"})
      .AddSyst(cb,
        "norm_TT_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"T"})
      .AddSyst(cb, "norm_T ", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"T"})
      .AddSyst(cb,
        "norm_T_$BIN", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"Zothers"})//Diboson
      .AddSyst(cb, "CMS_scale_efaketau_1prong_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Zothers"})//Diboson
      .AddSyst(cb, "CMS_scale_efaketau_1prong1pizero_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm0_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm0_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm1_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm1_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm10_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm10_B_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm0_E_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm0_E_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm1_E_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm1_E_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p0_dm10_E_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process({"Fakes"})//Diboson
      .AddSyst(cb, "CMS_TauFakeRate_p1_dm10_E_13TeV", "shape", SystMap<>::init(1.00));
  
  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))//Diboson
      .AddSyst(cb, "CMS_EES_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))//Diboson
      .AddSyst(cb, "CMS_Pileup_13TeV", "shape", SystMap<>::init(1.00));

  //cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))//Diboson
  //    .AddSyst(cb, "CMS_resRho_e_13TeV", "shape", SystMap<>::init(1.00));

  //cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))//Diboson
  //    .AddSyst(cb, "CMS_resPhi_e_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetTimePtEta_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetSinglePionHCAL_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetSinglePionECAL_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeStatHF_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeStatFSR_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeStatEC_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativePtHF_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativePtEC2_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativePtEC1_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativePtBB_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeJERHF_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeJEREC2_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeJEREC1_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeFSR_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetRelativeBal_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpPtRef_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpPtHF_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpPtEC2_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpPtEC1_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpPtBB_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetPileUpDataMC_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetFragmentation_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetFlavorQCD_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetAbsoluteStat_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetAbsoluteScale_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetAbsoluteMPFBias_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_Jes_JetAbsoluteFlavMap_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_scale_t_1prong_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_scale_t_1prong1pizero_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_scale_t_3prong_13TeV", "shape", SystMap<>::init(1.00));

  //cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
  //    .AddSyst(cb, "CMS_scale_met_UES_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_MET_hfUes_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_MET_chargedUes_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_MET_hcalUes_13TeV", "shape", SystMap<>::init(1.00));

  cb.cp().process(ch::JoinStr({sig_procs, {"Fakes","ZTauTau", "Zothers", "TT","T", "ggH_htt", "qqH_htt", "Diboson"}}))
      .AddSyst(cb, "CMS_MET_chargedUes_13TeV", "shape", SystMap<>::init(1.00));


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
  bbb.AddBinByBin(cb.cp().process({"TT","T"}), cb);
  bbb.AddBinByBin(cb.cp().process({"QCD"}), cb);
  bbb.AddBinByBin(cb.cp().process({"W"}), cb);
  bbb.AddBinByBin(cb.cp().process({"Diboson"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZTauTau"}), cb);
  bbb.AddBinByBin(cb.cp().process({"ZothersL"}), cb);
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
  TFile output("lfv_et.input.root", "RECREATE");

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
