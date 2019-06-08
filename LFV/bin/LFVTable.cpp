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
#include "TH2F.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TLine.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TLatex.h"
#include <TROOT.h>
#include <TStyle.h>
#include "TMath.h"

using namespace std;

double ComputeSumYLimit(double BranchingRatio);

int main(int argc, char* argv[]){

 string folder="$CMSSW_BASE/src/limitsAN441/CB";
 string  label="SUMMARYPLOT";
 string muelabel="mue";
 string mutaulabel="mutau";
 string lumi="35.9";

 if (argc > 1)
 { int count=0;
  for (count = 1; count < argc; count++)
  {
   if(strcmp(argv[count] ,"--l")==0) label=argv[count+1];
   if(strcmp(argv[count] ,"--f")==0) folder=argv[count+1];
   if(strcmp(argv[count] ,"--e")==0) muelabel=argv[count+1];
   if(strcmp(argv[count] ,"--h")==0) mutaulabel=argv[count+1];
   if(strcmp(argv[count] ,"--L")==0) lumi=argv[count+1];
  }
 }
 double intLumi=atof(lumi.c_str());
 printf("Luminosity: %.2f",intLumi);


 double MEDIAN[3][5];
 double SIGNIFICANCE[3][5];
 double ERR2DOWN[3][5];
 double ERRDOWN[3][5];
 double ERR2UP[3][5];
 double ERRUP[3][5];

 double OBSERVED[3][5];
 double MU[3][5];
 double MUERR[3][5];
 double MUHIERR[3][5];
 double MULOERR[3][5];

 string flavour[3]={muelabel.c_str(),mutaulabel.c_str(),"comb"};
 string flavourlatex[3]={"$\\mu\\tau_{\\textrm{e}}$","$\\mu\\tau_{h}$","$\\mu\\tau$"};

 string appendix[5]={"0JET","1JET","21JET","22JET","COMBINED"};
 printf("Extended Table with uncertainties \n");
 printf("================================= \n");
 printf("Channel -2s -1s Median +1s +2s Significance \n");
 printf("================================= \n");

 for (int j=0; j<3; j++){
  printf("%s \n ",flavour[j].c_str());
  for (int i=0; i<5; i++){
   string inputFile=flavour[j]+"_limit_"+appendix[i]+".root";
   string inputFileSig=flavour[j]+"_sig_"+appendix[i]+".root";
   TFile input((folder + "/"+appendix[i]+"/"+inputFile).c_str(), "READONLY");
   TFile inputSig((folder + "/"+appendix[i]+"/"+inputFileSig).c_str(), "READONLY");

   TTree* limitTree=(TTree*)input.Get("limit"); limitTree->SetName("limitTree");
   Float_t  quantileExpected;
   Double_t limit; 
   Double_t limitForCout[5];
   limitTree->SetBranchAddress("limit",&limit);
   limitTree->SetBranchAddress("quantileExpected",&quantileExpected);
   Long64_t nentries = limitTree->GetEntries();
   for (Long64_t k=0;k<nentries;k++) {
    limitTree->GetEntry(k);
    limitForCout[k]=limit;           

    if(k==2) MEDIAN[j][i]=limit;
    if(k==0) {ERR2DOWN[j][i]=limit; }
    if(k==1) {ERRDOWN[j][i]=limit;  }
    if(k==3) {ERRUP[j][i]=limit;    }
    if(k==4) {ERR2UP[j][i]=limit;   }
    if(k==5) OBSERVED[j][i]=limit;

   }


   TTree* limitTreeSig=(TTree*)inputSig.Get("limit"); limitTreeSig->SetName("limitTree");
   Double_t sig; 
   limitTreeSig->SetBranchAddress("limit",&sig);
   limitTreeSig->GetEntry(0); 
   SIGNIFICANCE[j][i]=sig;                        

   printf("%s \t",appendix[i].c_str()); 
   for (Long64_t i=0;i<nentries;i++) { printf("\t %2.2f",limitForCout[i]);}
   printf("\t %2.2f",sig);
   printf("\n");

   string inputFileFit="fit_"+flavour[j]+"_"+appendix[i]+".root";
   if(j==2) inputFileFit="fit_"+appendix[i]+".root";
   TFile inputFit((folder + "/"+appendix[i]+"/"+inputFileFit).c_str(), "READONLY");
   TTree* fitTree=(TTree*)inputFit.Get("tree_fit_sb"); fitTree->SetName("fitTree");
   Double_t mu, muLoErr, muHiErr;
   fitTree->SetBranchAddress("mu",&mu);
   fitTree->SetBranchAddress("muLoErr",&muLoErr);
   fitTree->SetBranchAddress("muHiErr",&muHiErr);
   fitTree->GetEntry(0);
   Double_t muErr=(muLoErr+muHiErr)/2;

   MU[j][i]=mu;
   MUERR[j][i]=muErr;
   MUHIERR[j][i]=muHiErr;
   MULOERR[j][i]=muLoErr;



  }
 }

 printf("==============\n");

 printf ("BEST FIT WITH FULL ERRORS FOR CHECK \n");

 for (int j=0; j<3; j++){
  printf("%s \n ",flavour[j].c_str());
  for (int i=0; i<5; i++){


      printf("%s \t",appendix[i].c_str());
   printf("\t  %2.2f +-  %2.2f (- %2.2f,+ %2.2f)",MU[j][i],MUERR[j][i],MULOERR[j][i],MUHIERR[j][i]);
   printf("\n");

  }
 }




 printf(" \n\n\n SUMMARY TABLE\n");
 printf("==============\n");


 printf("Expected 95%% CL limits on BR \n");
 printf("-----------------------------\n");
 for (int i=0; i<5; i++){
  printf("\t %s",appendix[i].c_str());
 }
 printf(" \n");

 for (int j=0; j<3; j++){
  if(j==2) printf("%%%%");
  printf("%s ",flavourlatex[j].c_str());  
  for (int i=0; i<5; i++){
   printf("\t &  $<$ %2.2f   ", MEDIAN[j][i]);
  }
  printf(" \\\\ \n");
 }
  printf("\\cline{2-6}  \n $\\mu\\tau$  & \\multicolumn{5}{c}{ $<$ %2.2f  } \\\\ \\hline ", MEDIAN[2][4]);

 printf("Observed 95%% CL limits on BR \n");
 printf("-----------------------------\n");
 for (int i=0; i<5; i++){
  printf("\t %s",appendix[i].c_str());
 }
 printf(" \n");

 for (int j=0; j<3; j++){
  if(j==2) printf("%%%%");
  printf("%s   \t",flavourlatex[j].c_str());
  for (int i=0; i<5; i++){
   printf("\t & $<$ %2.2f   ", OBSERVED[j][i]);
  }
  printf(" \\\\ \n");
  }
  printf("\\cline{2-6} \n  $\\mu\\tau$  & \\multicolumn{5}{c}{ $<$ %2.2f  } \\\\ \\hline ", OBSERVED[2][4]);



 printf("Expected significance for BR=1%%  \n");
 printf("-----------------------------\n");
 for (int i=0; i<5; i++){
  printf("\t %s",appendix[i].c_str());
 }
 printf(" \n");

 for (int j=0; j<3; j++){
  if(j==2) printf("%%%%"); 
  printf("%s   ",flavourlatex[j].c_str());  
  for (int i=0; i<5; i++){
   printf("\t & %2.2f   ", SIGNIFICANCE[j][i]);
  }
  printf(" \\\\ \n");
  }
  printf("\\cline{2-6} \n $\\mu\\tau$  & \\multicolumn{5}{c}{ %2.2f  } \\\\ \\hline ", SIGNIFICANCE[2][4]);


 printf("==============\n");

 printf("Best Fit BR=1%%  \n");
 printf("-----------------------------\n");
 for (int i=0; i<5; i++){
  printf("\t %s",appendix[i].c_str());
 }
 printf(" \n");

 for (int j=0; j<3; j++){
  if(j==2) printf("%%%%"); 
  printf("%s    \t",flavourlatex[j].c_str());
  for (int i=0; i<5; i++){
   printf("\t &  %2.2f+- %2.2f (-%2.2f +%2.2f)  ", MU[j][i], MUERR[j][i],  MULOERR[j][i],  MUHIERR[j][i]);
  }
  printf("\\\\  \n");
 }


 printf("Best Fit BR=1%% (V2) \n");
 printf("-----------------------------\n");
 for (int i=0; i<5; i++){
  printf("\t %s",appendix[i].c_str());
 }
 printf(" \n");

 for (int j=0; j<3; j++){
  if(j==2) printf("%%%%");
  printf("%s    \t",flavourlatex[j].c_str());
  for (int i=0; i<5; i++){
   printf("\t & %2.2f $\\pm$ %2.2f  ", MU[j][i], MUERR[j][i]);
  }
  printf("\\\\  \n");
  }
   printf("\\cline{2-6} \n $\\mu\\tau$  & \\multicolumn{5}{c}{ %2.2f $\\pm$ %2.2f } \\\\ \\hline ", MU[2][4],MUERR[2][4]); 


 printf(" \n\n\n YUKAWA LIMIT\n");
 printf("==============\n");

 double yukawa=sqrt(ComputeSumYLimit(MEDIAN[2][4]/100));
 double yukawa2=sqrt(ComputeSumYLimit(OBSERVED[2][4]/100));

 printf("\t EXPECTED: Branching Ratio: <%2.2f :  Yukawa limit: < %2.6f \n", MEDIAN[2][4], yukawa);
 printf("\t OBSERVED: Branching Ratio: <%2.2f :  Yukawa limit: < %2.6f \n", OBSERVED[2][4], yukawa2);

  double yukawaOLD=sqrt(ComputeSumYLimit(0.69/100));

 printf("\t for reference, 0.69%% : < %2.6f \n", yukawaOLD);





 //// Run I STYLE PLOT MAKING 

 gStyle->SetOptFit(1);
 gStyle->SetOptStat(0);
 gStyle->SetOptTitle(0);

 TCanvas *PLOTMU = new TCanvas("PLOTMU", "BEST FIT",600,22,664,763);
 PLOTMU->Range(-1.298202,-1.436207,2.706292,7.434483);
 PLOTMU->SetFillColor(0);
 PLOTMU->SetBorderMode(0);
 PLOTMU->SetBorderSize(2);
 PLOTMU->SetTickx(1);
 PLOTMU->SetTicky(1);
 PLOTMU->SetLeftMargin(0.2742424);
 PLOTMU->SetRightMargin(0.05151515);
 PLOTMU->SetTopMargin(0.04897959);
 PLOTMU->SetBottomMargin(0.1619048);
 PLOTMU->SetFrameFillStyle(0);
 PLOTMU->SetFrameLineWidth(2);
 PLOTMU->SetFrameBorderMode(0);
 PLOTMU->SetFrameFillStyle(0);
 PLOTMU->SetFrameLineWidth(2);
 PLOTMU->SetFrameBorderMode(0);

 TCanvas *PLOTLIMIT = new TCanvas("PLOTLIMIT", "BEST FIT",600,22,664,763);
 PLOTLIMIT->Range(-1.298202,-1.436207,2.706292,7.434483);
 PLOTLIMIT->SetFillColor(0);
 PLOTLIMIT->SetBorderMode(0);
 PLOTLIMIT->SetBorderSize(2);
 PLOTLIMIT->SetTickx(1);
 PLOTLIMIT->SetTicky(1);
 PLOTLIMIT->SetLeftMargin(0.2742424);
 PLOTLIMIT->SetRightMargin(0.05151515);
 PLOTLIMIT->SetTopMargin(0.04897959);
 PLOTLIMIT->SetBottomMargin(0.1619048);
 PLOTLIMIT->SetFrameFillStyle(0);
 PLOTLIMIT->SetFrameLineWidth(2);
 PLOTLIMIT->SetFrameBorderMode(0);
 PLOTLIMIT->SetFrameFillStyle(0);
 PLOTLIMIT->SetFrameLineWidth(2);
 PLOTLIMIT->SetFrameBorderMode(0);

 TCanvas *PLOTSUMMARY = new TCanvas("PLOTSUMMARY", "SUMMARY",600,22,664,763);
 PLOTSUMMARY->Range(-1.298202,-1.436207,2.706292,7.434483);
 PLOTSUMMARY->SetFillColor(0);
 PLOTSUMMARY->SetBorderMode(0);
 PLOTSUMMARY->SetBorderSize(2);
 PLOTSUMMARY->SetTickx(1);
 PLOTSUMMARY->SetTicky(1);
 PLOTSUMMARY->SetLeftMargin(0.2742424);
 PLOTSUMMARY->SetRightMargin(0.05151515);
 PLOTSUMMARY->SetTopMargin(0.04897959);
 PLOTSUMMARY->SetBottomMargin(0.1619048);
 PLOTSUMMARY->SetFrameFillStyle(0);
 PLOTSUMMARY->SetFrameLineWidth(2);
 PLOTSUMMARY->SetFrameBorderMode(0);
 PLOTSUMMARY->SetFrameFillStyle(0);
 PLOTSUMMARY->SetFrameLineWidth(2);
 PLOTSUMMARY->SetFrameBorderMode(0);


 const Int_t nx = 1; 
 const Int_t ny = 9; 
 string month[nx]  = {"Best Fit"}; 
 string channels[ny]={"H#rightarrow#mu#tau", "#mu#tau_{e}, VBF","#mu#tau_{e}, 2 Jets","#mu#tau_{e}, 1 Jet", "#mu#tau_{e}, 0 Jets","#mu#tau_{h}, VBF","#mu#tau_{h}, 2 Jets","#mu#tau_{h}, 1 Jet", "#mu#tau_{h}, 0 Jets"}; 

 double y[9]={0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5};
 double erry[9]={0,0,0,0,0,0,0};


 // Reordering to keep most of the old macro
 double MEDIANplot[9]=       {MEDIAN[2][4],MEDIAN[0][3],MEDIAN[0][2],MEDIAN[0][1],MEDIAN[0][0], MEDIAN[1][3],MEDIAN[1][2],MEDIAN[1][1],MEDIAN[1][0]};
 double ERR2DOWNplot[9]=       {ERR2DOWN[2][4],ERR2DOWN[0][3],ERR2DOWN[0][2],ERR2DOWN[0][1],ERR2DOWN[0][0], ERR2DOWN[1][3],ERR2DOWN[1][2],ERR2DOWN[1][1],ERR2DOWN[1][0]};
 double ERRDOWNplot[9]=       {ERRDOWN[2][4],ERRDOWN[0][3],ERRDOWN[0][2],ERRDOWN[0][1],ERRDOWN[0][0], ERRDOWN[1][3],ERRDOWN[1][2],ERRDOWN[1][1],ERRDOWN[1][0]};
 double ERR2UPplot[9]=       {ERR2UP[2][4],ERR2UP[0][3],ERR2UP[0][2],ERR2UP[0][1],ERR2UP[0][0], ERR2UP[1][3],ERR2UP[1][2],ERR2UP[1][1],ERR2UP[1][0]};
 double ERRUPplot[9]=       {ERRUP[2][4],ERRUP[0][3],ERRUP[0][2],ERRUP[0][1],ERRUP[0][0], ERRUP[1][3],ERRUP[1][2],ERRUP[1][1],ERRUP[1][0]};

 double R2DOWNplot[9]=       {ERR2DOWN[2][4],ERR2DOWN[0][3],ERR2DOWN[0][2],ERR2DOWN[0][1],ERR2DOWN[0][0], ERR2DOWN[1][3],ERR2DOWN[1][2],ERR2DOWN[1][1],ERR2DOWN[1][0]};
 double RDOWNplot[9]=       {ERRDOWN[2][4],ERRDOWN[0][3],ERRDOWN[0][2],ERRDOWN[0][1],ERRDOWN[0][0], ERRDOWN[1][3],ERRDOWN[1][2],ERRDOWN[1][1],ERRDOWN[1][0]};
 double R2UPplot[9]=       {ERR2UP[2][4],ERR2UP[0][3],ERR2UP[0][2],ERR2UP[0][1],ERR2UP[0][0], ERR2UP[1][3],ERR2UP[1][2],ERR2UP[1][1],ERR2UP[1][0]};
 double RUPplot[9]=       {ERRUP[2][4],ERRUP[0][3],ERRUP[0][2],ERRUP[0][1],ERRUP[0][0], ERRUP[1][3],ERRUP[1][2],ERRUP[1][1],ERRUP[1][0]};

 for (int i=0; i<9; i++){
  ERR2DOWNplot[i]=-ERR2DOWNplot[i]+MEDIANplot[i];
  ERRDOWNplot[i]=-ERRDOWNplot[i]+MEDIANplot[i];
  ERRUPplot[i]=ERRUPplot[i]-MEDIANplot[i];
  ERR2UPplot[i]=ERR2UPplot[i]-MEDIANplot[i];
 }


 double OBSERVEDplot[9]=       {OBSERVED[2][4],OBSERVED[0][3],OBSERVED[0][2],OBSERVED[0][1],OBSERVED[0][0], OBSERVED[1][3],OBSERVED[1][2],OBSERVED[1][1],OBSERVED[1][0]};

 double MUplot[9]=       {MU[2][4],MU[0][3],MU[0][2],MU[0][1],MU[0][0], MU[1][3],MU[1][2],MU[1][1],MU[1][0]};
 double MUHIERRplot[9]=       {MUHIERR[2][4],MUHIERR[0][3],MUHIERR[0][2],MUHIERR[0][1],MUHIERR[0][0], MUHIERR[1][3],MUHIERR[1][2],MUHIERR[1][1],MUHIERR[1][0]};
 double MULOERRplot[9]=       {MULOERR[2][4],MULOERR[0][3],MULOERR[0][2],MULOERR[0][1],MULOERR[0][0], MULOERR[1][3],MULOERR[1][2],MULOERR[1][1],MULOERR[1][0]};


 TString channels2[9]; TString channels3[9];
 for(int i=0; i<9; i++) {
  channels2[i]=Form("#splitline{%s}{    #scale[0.8]{%1.2f%% (%1.2f%%)}} ", channels[i].c_str(),OBSERVEDplot[i],MEDIANplot[i]);
  channels3[i]=Form("#splitline{%s}{    #scale[0.8]{%1.2f_{-%1.2f}^{+%1.2f}%%}} ", channels[i].c_str(),MUplot[i],MULOERRplot[i],MUHIERRplot[i]);
 }

 PLOTLIMIT->cd(); 

 TH2F *h = new TH2F("h","test",1,-1.5,14.5,9,0,9);
 for (int j=0; j<ny; j++)      h->Fill(1,channels2[j],0);
 h->SetXTitle("95% CL limit on #bf{#it{#Beta}}(H#rightarrow#mu#tau), %");
 h->GetXaxis()->SetLabelSize(0.04);
 h->GetXaxis()->SetTitleSize(0.05);
 h->GetYaxis()->SetLabelSize(0.05);
 h->Draw();
 h->GetYaxis()->CenterLabels();


 for (int i=0; i<9; i++){
  TPave *pave = new TPave(R2DOWNplot[i],i+0.75,R2UPplot[i],i+0.25,4,"br");
  pave->Draw();
  pave->SetBorderSize(0);
  pave->SetFillColor(kOrange);
  TPave *pave2 = new TPave(RDOWNplot[i],i+0.75,RUPplot[i],i+0.25,4,"br");
  pave2->Draw();
  pave2->SetBorderSize(0);
  pave2->SetFillColor(kGreen+1);
 }

 TGraphAsymmErrors *GRAPHMEDIAN=new TGraphAsymmErrors(9,MEDIANplot,y,0,0,erry,erry);
 TGraphAsymmErrors *GRAPHOBSERVED=new TGraphAsymmErrors(9,OBSERVEDplot,y,0,0,0,0);

 GRAPHMEDIAN->SetLineColor(kBlue); GRAPHMEDIAN->SetMarkerStyle(5); GRAPHMEDIAN->SetMarkerColor(kBlue+2); GRAPHMEDIAN->SetMarkerSize(1.5); GRAPHMEDIAN->SetLineWidth(1);
 GRAPHOBSERVED->SetMarkerStyle(20);
 GRAPHMEDIAN->Draw("P,sames");
 GRAPHOBSERVED->Draw("P,sames");

 TLine  *line0 = new TLine(0,0,0,9);
 line0->SetLineStyle(kDotted);
 line0->SetLineColor(kGray);
 line0->Draw();

 /*
    TLine  *line8TeV = new TLine(1.51,0,1.51,9);
    line8TeV->SetLineColor(kRed);
    line8TeV->Draw();

    TLine  *line8TeVExpected = new TLine(0.75,0,0.75,9);
    line8TeVExpected->SetLineColor(kBlack);
    line8TeVExpected->SetLineStyle(kDashed);
    line8TeVExpected->Draw();
    */

 TLine  *lineH = new TLine(-1.5,5,14.5,5);
 lineH->SetLineWidth(1);
 lineH->Draw();

 TLine  *lineH2 = new TLine(-1.5,1,14.5,1);
 lineH2->SetLineWidth(2);
 lineH2->Draw();

 TLegend *leg = new TLegend(0.645,0.70,0.98,0.90,NULL,"brNDC");
 leg->SetTextFont(62);
 leg->SetTextSize(0.02721088);
 leg->SetFillColor(0);
 leg->SetFillStyle(0);
 leg->SetBorderSize(0);
 TLegendEntry *entry;
 leg->SetHeader(label.c_str()); 
 entry=leg->AddEntry(GRAPHOBSERVED,"Observed","p");
 entry=leg->AddEntry(GRAPHMEDIAN,"Median expected","p");
 entry=leg->AddEntry("NULL","68% expected","f");
 entry->SetFillColor(kGreen+1);
 entry->SetFillStyle(1001);
 entry->SetLineStyle(1);
 entry=leg->AddEntry("NULL","95% expected","f");
 entry->SetFillColor(kOrange);
 entry->SetFillStyle(1001);
 entry->SetLineStyle(1);
 leg->Draw();


 /*
    TLegend*  leg2 = new TLegend(0.5772727,0.4938776,0.9969697,0.629932,NULL,"brNDC");
    leg2->SetBorderSize(0);
    leg2->SetTextFont(62);
    leg2->SetTextSize(0.02176871);
    leg2->SetFillColor(0);
    leg2->SetFillStyle(0);
    leg2->SetBorderSize(0);
    entry=leg2->AddEntry("NULL","8 TeV [Phys. Lett. B 749 (2015) 337]:","h");
    entry=leg2->AddEntry(line8TeV,"Observed","l");
    entry=leg2->AddEntry(line8TeVExpected,"Expected","l");
    leg2->Draw();
    */

 TLatex latex;
 latex.SetNDC();
 latex.SetTextSize(0.04);

/* latex.SetTextAlign(31); // align right
 latex.DrawLatex(0.95,0.96,Form("%.1f fb^{-1} (13 TeV)",intLumi));

 latex.SetTextAlign(11); // align left
 latex.DrawLatex(0.27,0.96,"CMS");
*/

 latex.SetTextSize(0.04*0.80);
 latex.SetTextFont(42);
 latex.SetTextAlign(31); // align right
 latex.DrawLatex(0.95,0.96,Form("%.1f fb^{-1} (13 TeV)",intLumi));

 latex.SetTextSize(0.04);
 latex.SetTextFont(61);
 latex.SetTextAlign(11); // align left
 latex.DrawLatex(0.27,0.96,"CMS");


 string inputFile=folder+"/HMUTAU_"+folder+"_SUMMARY.pdf";
 PLOTLIMIT->SaveAs(inputFile.c_str());
 inputFile=folder+"/HMUTAU_"+folder+"_SUMMARY.C";
 PLOTLIMIT->SaveAs(inputFile.c_str());
 PLOTMU->cd();

 TH2F *h2 = new TH2F("h2","test",1,-3.5,3.5,9,0,9);
 for (int j=0; j<ny; j++)      h2->Fill(1,channels3[j],0);
 h2->SetXTitle("Best Fit to #bf{#it{#Beta}}(H#rightarrow #mu#tau), %");
 h2->GetXaxis()->SetLabelSize(0.04);
 h2->GetXaxis()->SetTitleSize(0.05);
 h2->GetYaxis()->SetLabelSize(0.05);
 h2->Draw();
 h2->GetYaxis()->CenterLabels();

 TGraphAsymmErrors *GRAPHMU=new TGraphAsymmErrors(9,MUplot,y,MULOERRplot,MUHIERRplot,erry,erry);

 GRAPHMU->SetLineColor(kBlack); GRAPHMU->SetMarkerStyle(20); GRAPHMU->SetMarkerColor(kBlack); GRAPHMU->SetMarkerSize(1.5); GRAPHMU->SetLineWidth(1);
 GRAPHMU->Draw("P");

 line0->Draw();

 TLine  *lineH3 = new TLine(-3.5,5,3.5,5);
 lineH3->SetLineWidth(1);
 lineH3->Draw();

 TLine  *lineH4 = new TLine(-3.5,1,3.5,1);
 lineH4->SetLineWidth(2);
 lineH4->Draw();

 TLatex latex2;
 latex2.SetNDC();
/*
 latex2.SetTextAlign(31); // align right
 latex2.DrawLatex(0.95,0.96,Form("%.1f fb^{-1} (13 TeV)",intLumi));
*/

 latex2.SetTextSize(0.04*0.80);
 latex2.SetTextFont(42);
 latex2.SetTextAlign(31); // align right
 latex2.DrawLatex(0.95,0.96,Form("%.1f fb^{-1} (13 TeV)",intLumi));

 latex2.SetTextSize(0.04);
 latex2.SetTextFont(61);
 latex2.SetTextAlign(11); // align left
 latex2.DrawLatex(0.27,0.96,"CMS");



// latex2.SetTextAlign(11); // align left
// latex2.DrawLatex(0.25,0.96,"CMS preliminary");


 string inputFile2=folder+"/HMUTAU_"+folder+"_BESTFIT.pdf";
 PLOTMU->SaveAs(inputFile2.c_str());






}

double ComputeSumYLimit(double BranchingRatio=0.1){
 //    cout<<"Computing Ratio for "<<BranchingRatio<<endl;
 if (BranchingRatio == 1) { cout<<"Branching Ratio cannot be 1"<<endl;  return 0;}

 // Higgs Mass
 double mh=125;

 // Higgs Width at 125 GeV is 4.1 MeV
 double gammah= 4.1/1000; //  gammah =  SM_Higgs_Width

 // Magic Formulas, #26 and #27 from
 // BR (h->mutau)  = Width(h->mutau) / ( Width (h->mutau) + SM_Higgs_Width)
 // Width (h->mutau) = mh/8Pi * (|Y(mutau)|**2 + |Y(taumu)|**2)
 // (|Y(mutau)|**2 + |Y(taumu)|**2) =  (BR * SM_Higgs_Width)  / ()*(1- BR)

 double LimitOnSumY = 8*TMath::Pi()/mh * BranchingRatio * gammah / (1 -BranchingRatio);
 //    cout<<sqrt(LimitOnSumY)<<endl;
 return LimitOnSumY;
}




