void hist_243()
{
//=========Macro generated from canvas: c1/c1
//=========  (Fri Jul 26 16:05:10 2019) by ROOT version6.06/09
   TCanvas *c1 = new TCanvas("c1", "c1",1253,489,700,547);
   gStyle->SetOptFit(1);
   c1->ToggleEventStatus();
   c1->SetHighLightColor(2);
   c1->Range(34.875,-1.96875,186.125,17.71875);
   c1->SetBorderSize(2);
   c1->SetFrameFillColor(0);
   
   GH1D *dt_z3_2_totalx__1 = new GH1D("dt_z3_2_totalx__1","dt_z3_2_totalx",121,50,171);
   dt_z3_2_totalx__1->SetBinContent(46,1);
   dt_z3_2_totalx__1->SetBinContent(48,1);
   dt_z3_2_totalx__1->SetBinContent(58,1);
   dt_z3_2_totalx__1->SetBinContent(68,1);
   dt_z3_2_totalx__1->SetBinContent(70,1);
   dt_z3_2_totalx__1->SetBinContent(74,2);
   dt_z3_2_totalx__1->SetBinContent(76,1);
   dt_z3_2_totalx__1->SetBinContent(78,2);
   dt_z3_2_totalx__1->SetBinContent(80,4);
   dt_z3_2_totalx__1->SetBinContent(82,7);
   dt_z3_2_totalx__1->SetBinContent(84,7);
   dt_z3_2_totalx__1->SetBinContent(86,10);
   dt_z3_2_totalx__1->SetBinContent(88,13);
   dt_z3_2_totalx__1->SetBinContent(90,15);
   dt_z3_2_totalx__1->SetBinContent(92,15);
   dt_z3_2_totalx__1->SetBinContent(94,9);
   dt_z3_2_totalx__1->SetBinContent(96,4);
   dt_z3_2_totalx__1->SetBinContent(98,4);
   dt_z3_2_totalx__1->SetBinContent(100,2);
   dt_z3_2_totalx__1->SetEntries(100);
   dt_z3_2_totalx__1->SetDirectory(0);
   
   TPaveStats *ptstats = new TPaveStats(0.78,0.715,0.98,0.995,"brNDC");
   ptstats->SetName("stats");
   ptstats->SetBorderSize(2);
   ptstats->SetTextAlign(12);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#0000ff");
   ptstats->SetTextColor(ci);
   TText *AText = ptstats->AddText("dt_z3_2_totalx");
   AText->SetTextSize(0.02862222);
   AText = ptstats->AddText("Entries = 100    ");
   AText = ptstats->AddText("Mean  =  136.6");
   AText = ptstats->AddText("Integral =    100");
   AText = ptstats->AddText("#chi^{2} / ndf = 2.091e+26 / 56");
   AText = ptstats->AddText("Prob  =     0");
   AText = ptstats->AddText("p0       = -9.905 #pm 0.849 ");
   AText = ptstats->AddText("p1       = 0.1425 #pm 1.0000 ");
   AText = ptstats->AddText("p2       = -1.991e+12 #pm 1.000e+00 ");
   ptstats->SetOptStat(1000111);
   ptstats->SetOptFit(1111);
   ptstats->Draw();
   dt_z3_2_totalx__1->GetListOfFunctions()->Add(ptstats);
   ptstats->SetParent(dt_z3_2_totalx__1);
   
   TF1 *PrevFitTMP2 = new TF1("PrevFitTMP","expo(0)+pol0(2)",81,140);
   PrevFitTMP2->SetFillColor(19);
   PrevFitTMP2->SetFillStyle(0);

   ci = TColor::GetColor("#ff0000");
   PrevFitTMP2->SetLineColor(ci);
   PrevFitTMP2->SetLineWidth(3);
   PrevFitTMP2->SetChisquare(2.091446e+26);
   PrevFitTMP2->SetNDF(56);
   PrevFitTMP2->GetXaxis()->SetLabelFont(42);
   PrevFitTMP2->GetXaxis()->SetLabelSize(0.035);
   PrevFitTMP2->GetXaxis()->SetTitleSize(0.035);

   ci = TColor::GetColor("#0000ff");
   PrevFitTMP2->GetXaxis()->SetTitleColor(ci);
   PrevFitTMP2->GetXaxis()->SetTitleFont(42);
   PrevFitTMP2->GetYaxis()->SetLabelFont(42);
   PrevFitTMP2->GetYaxis()->SetLabelSize(0.035);
   PrevFitTMP2->GetYaxis()->SetTitleSize(0.035);
   PrevFitTMP2->GetYaxis()->SetTitleFont(42);
   PrevFitTMP2->SetParameter(0,-9.904755);
   PrevFitTMP2->SetParError(0,0.8485281);
   PrevFitTMP2->SetParLimits(0,0,0);
   PrevFitTMP2->SetParameter(1,0.1425182);
   PrevFitTMP2->SetParError(1,1);
   PrevFitTMP2->SetParLimits(1,0,0);
   PrevFitTMP2->SetParameter(2,-1.990634e+12);
   PrevFitTMP2->SetParError(2,1);
   PrevFitTMP2->SetParLimits(2,0,0);
   dt_z3_2_totalx__1->GetListOfFunctions()->Add(PrevFitTMP2);
   dt_z3_2_totalx__1->GetXaxis()->SetLabelFont(42);
   dt_z3_2_totalx__1->GetXaxis()->SetLabelSize(0.035);
   dt_z3_2_totalx__1->GetXaxis()->SetTitleSize(0.035);

   ci = TColor::GetColor("#0000ff");
   dt_z3_2_totalx__1->GetXaxis()->SetTitleColor(ci);
   dt_z3_2_totalx__1->GetXaxis()->SetTitleFont(42);
   dt_z3_2_totalx__1->GetYaxis()->SetLabelFont(42);
   dt_z3_2_totalx__1->GetYaxis()->SetLabelSize(0.035);
   dt_z3_2_totalx__1->GetYaxis()->SetTitleSize(0.035);
   dt_z3_2_totalx__1->GetYaxis()->SetTitleFont(42);
   dt_z3_2_totalx__1->GetZaxis()->SetLabelFont(42);
   dt_z3_2_totalx__1->GetZaxis()->SetLabelSize(0.035);
   dt_z3_2_totalx__1->GetZaxis()->SetTitleSize(0.035);
   dt_z3_2_totalx__1->GetZaxis()->SetTitleFont(42);
   dt_z3_2_totalx__1->Draw("");
   
   TPaveText *pt = new TPaveText(0.01,0.9393038,0.247765,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   AText = pt->AddText("dt_z3_2_totalx");
   pt->Draw();
   c1->Modified();
   c1->cd();
   c1->SetSelected(c1);
   c1->ToggleToolBar();
}
