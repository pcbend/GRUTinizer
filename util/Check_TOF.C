//std::vector<GH1D*> Proj_vect;

TList *CheckTOF_vs_Time(double lineX1=0,double lineX2=6000,double lineY1=-1050,double lineY2=-1050){
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TLegend *leg = new TLegend(0.2,0.2,0.5,0.5);
  TLegend *leg2 = new TLegend(0.2,0.2,0.5,0.5);

  int MostEvents_Location = 0;
  int MostEvents = 0;
  
  TCanvas *TwoDCanvas = new TCanvas("TwoDCanvas","2D Canvas TOF vs S800 Timestamp");
  TCanvas *ProjCanvas = new TCanvas("ProjCanvas","1D TOF Projection");
  TCanvas *StackCanvas = new TCanvas("StackCanvas","1D TOF Projection Stacked");
  TCanvas *ScaledCanv = new TCanvas("ScaledCanv","Scaled Canvas");
  if(NumFiles%4==0){
    int FirstDivide = NumFiles/4;
    TwoDCanvas->Divide(FirstDivide,4);
    ProjCanvas->Divide(FirstDivide,4);
  }else if(NumFiles%3==0 && NumFiles%4!=0){ 
    int FirstDivide = NumFiles/3;
    TwoDCanvas->Divide(FirstDivide,3);
    ProjCanvas->Divide(FirstDivide,3);
  }else if(NumFiles%2==0 && NumFiles%4!=0 && NumFiles%3!=0){
    int FirstDivide = NumFiles/2;
    TwoDCanvas->Divide(FirstDivide,2);
    ProjCanvas->Divide(FirstDivide,2);
  }else{
    TwoDCanvas->Divide(NumFiles);
    ProjCanvas->Divide(NumFiles);
  }

  for(int i=0;i<NumFiles;i++){
    TFile *WorkingFile = (TFile*)FileList->At(i);

    string FileName = WorkingFile->GetName();
    cout << " File Name = " << FileName << endl;
    size_t found = FileName.find_last_of(".");
    string run = FileName.substr(found-4,4);
    stringstream runSS;
    double runNumb =0;
    runSS << run;
    runSS >> runNumb;    
    if(runNumb==0){
      run = FileName.substr(found-3,3);
      stringstream runSS2;
      runSS2 << run;
      runSS2 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-2,2);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    if(runNumb==0){
      run = FileName.substr(found-1,1);
      stringstream runSS3;
      runSS3 << run;
      runSS3 >> runNumb;
    }

    
    
    WorkingFile->cd("CorrelatedMESY");
    TwoDCanvas->cd(i+1);
    TOF_vs_S800Timestamp->SetTitle(Form("Corrected OBJ TOF vs S800 Timestamp Run %s",run.c_str()));
    TOF_vs_S800Timestamp->GetYaxis()->SetRangeUser(-1050,-900);
    TOF_vs_S800Timestamp->Draw();
    TLine *line1 = new TLine(lineX1,lineY1,lineX2,lineY2);
    line1->SetLineStyle(kDashed);
    line1->SetLineWidth(3);
    line1->Draw();
    GH1D *Proj1 = TOF_vs_S800Timestamp->ProjectionY(Form("Corrected OBJ TOF Run %s",run.c_str()));
    Proj1->SetTitle(Form("Corrected OBJ TOF Run %s",run.c_str()));
    double Proj1_yMax = Proj1->GetMaximum();
    ProjCanvas->cd(i+1);
    Proj1->Draw();

    StackCanvas->cd();
    Proj1->SetLineColor(i+1);
    Proj1->SetTitle("");
    leg->AddEntry(Proj1,Form("Run%s",run.c_str()),"l");
    if(i==0)
      Proj1->Draw();
    else
      Proj1->Draw("same");

    GH1D *Proj_Clone  = (GH1D*)Proj1->Clone();
    if(i<4){
      Proj_Clone->SetLineColor(i%4+1);
      
    }else if(i>3 && i<8){
      Proj_Clone->SetLineColor(i%4+1);
      Proj_Clone->SetLineStyle(2);
    }else if(i>7 && i<12){
      Proj_Clone->SetLineColor(i%4+1);
      Proj_Clone->SetLineStyle(3);
    }else{
      Proj_Clone->SetLineColor(i%4+1);
      Proj_Clone->SetLineStyle(4);
    }
    
    ScaledCanv->cd();
    Proj_Clone->Scale(1.0/Proj_Clone->Integral());
    if(i==0)
      Proj_Clone->Draw();
    else
      Proj_Clone->Draw("same");

    leg2->AddEntry(Proj_Clone,Form("Run%s",run.c_str()),"l");
  }
  
  
  StackCanvas->cd();
  leg->Draw("same");
  ScaledCanv->cd();
  leg2->Draw("same");
}
