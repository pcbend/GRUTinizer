vector<double> CRDC1_Y_Centroid;
vector<double> CRDC2_Y_Centroid;
vector<double> YTA_Centroid;
vector<double> ATA_Offsets;
vector<double> BTA_Offsets;
vector<int> ATABTA_Run;

bool GetATABTAOffsets(const char* outfile){

  if(!(ATA_Offsets.size()) || !(BTA_Offsets.size())) return 0;
  ofstream outFile;
  outFile.open(outfile);
  for(int i=0;i<ATA_Offsets.size();i++){
    std::cout << " --- Run : " << ATABTA_Run.at(i) << std::endl;
    std::cout << "  ATA Offset = " << ATA_Offsets.at(i) << " radians " << std::endl;
    std::cout << "  BTA Offset = " << BTA_Offsets.at(i) << " radians " << std::endl;

    outFile << " --- Run : " << ATABTA_Run.at(i) << std::endl;
    outFile << "  ATA Offset = " << ATA_Offsets.at(i) << " radians " << std::endl;
    outFile << "  ATA Offset = " << ATA_Offsets.at(i)*TMath::RadToDeg() << " degrees " << std::endl;
    outFile << "  BTA Offset = " << BTA_Offsets.at(i) << " radians " << std::endl;
    outFile << "  BTA Offset = " << BTA_Offsets.at(i)*TMath::RadToDeg() << " degrees " << std::endl << std::endl;
    outFile << " ATA_SHIFT { " << std::endl;
    outFile << "  Value: " << ATA_Offsets.at(i)*TMath::RadToDeg() << std::endl;
    outFile << "}" << std::endl << std::endl;
    outFile << " BTA_SHIFT { " << std::endl;
    outFile << "  Value: " << BTA_Offsets.at(i)*TMath::RadToDeg() << std::endl;
    outFile << "}" << std::endl << std::endl;
    

  }
  outFile.close();
  return 1;
}

TList *CheckATABTA(float lowa=0,float higha=0,float lowb=0,float highb=0){
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();

  TGraph *ATAGraph = new TGraph(NumFiles);
  TGraph *BTAGraph = new TGraph(NumFiles);
  ATAGraph->SetMarkerColor(kRed);
  BTAGraph->SetMarkerColor(kRed);

  TCanvas *TACanv = new TCanvas("TACanv","Target Angle Canvas"); // 2D plots
  TCanvas *ATACanv = new TCanvas("ATACanv","ATA Canvas");        // 1d ATA 
  TCanvas *BTACanv = new TCanvas("BTACanv","BTA Canvas");        // 1d BTA
  
  if(NumFiles%2==0){
    int FirstDivide = NumFiles/2;
    TACanv->Divide(FirstDivide,2);
    ATACanv->Divide(FirstDivide,2);
    BTACanv->Divide(FirstDivide,2);
  }else if(NumFiles%3==0){ 
    int FirstDivide = NumFiles/3;
    TACanv->Divide(FirstDivide,3);
    ATACanv->Divide(FirstDivide,3);
    BTACanv->Divide(FirstDivide,3);
  } else{
    TACanv->Divide(NumFiles);
    ATACanv->Divide(NumFiles);
    BTACanv->Divide(NumFiles);
  }
  
  if(lowa==0 && higha==0){
    lowa=-0.2;
    higha=0.2;
  }

  if(lowb==0 && highb==0){
    lowb=-0.2;
    highb=0.2;
  }
  double lowCentATA=0;
  double lowCentBTA=0;
  double highCentATA=0;
  double highCentBTA=0;
  double lowRun=0,highRun=0;
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

    WorkingFile->cd("InverseMap");
    
    
    TACanv->cd(i+1);
    TLine *LineX = new TLine(-0.2,0,0.2,0);
    TLine *LineY = new TLine(0,-0.2,0,0.2);
    LineX->SetLineColor(kBlack);
    LineX->SetLineStyle(kDashed);
    LineX->SetLineWidth(3);
    LineY->SetLineColor(kBlack);
    LineY->SetLineStyle(kDashed);
    LineY->SetLineWidth(3);
    ATA_vs_BTA->SetTitle(Form("ATA vs BTA Run %s",run.c_str()));
    ATA_vs_BTA->Draw();
    LineX->Draw("same");
    LineY->Draw("same");
    
    TH1D *ProjATA = ATA_vs_BTA->ProjectionX(Form("ATA_Run%s",run.c_str()));
    TH1D *ProjBTA = ATA_vs_BTA->ProjectionY(Form("BTA_Run%s",run.c_str()));


    double ProjATA_yMax = ProjATA->GetMaximum();
    double ProjBTA_yMax = ProjBTA->GetMaximum();
    
    // std::cout << " Proj1 Y Axis Max = " << Proj1_yMax << endl;
    // std::cout << " Proj2 Y Axis Max = " << Proj2_yMax << endl;


    ATACanv->cd(i+1);
    ProjATA->Draw();
    TLine *LineATA = new TLine(0,0,0,ProjATA_yMax);
    LineATA->SetLineColor(kBlack);
    LineATA->SetLineStyle(kDashed);
    LineATA->SetLineWidth(3);
    LineATA->Draw("Same");
    GausFit(ProjATA,lowa,higha);
    double CentATA = GetCentroid(ProjATA);
    ATA_Offsets.push_back(-CentATA); // Do I want to do this?
    ProjATA->SetTitle(Form("ATA Run%s Centoid = %.4f",run.c_str(),CentATA));
    ATAGraph->SetPoint(i,runNumb,CentATA);
    ATABTA_Run.push_back(runNumb);

    BTACanv->cd(i+1);
    ProjBTA->Draw();
    TLine *LineBTA = new TLine(0,0,0,ProjBTA_yMax);
    LineBTA->SetLineColor(kBlack);
    LineBTA->SetLineStyle(kDashed);
    LineBTA->SetLineWidth(3);
    LineBTA->Draw("Same");
    GausFit(ProjBTA,lowb,highb);
    double CentBTA = GetCentroid(-ProjBTA);
    BTA_Offsets.push_back(CentBTA); // Do I want to do this?
    ProjBTA->SetTitle(Form("BTA Run%s Centoid = %.4f",run.c_str(),CentBTA));
    BTAGraph->SetPoint(i,runNumb,CentBTA);


    HistList->Add(ProjATA);
    HistList->Add(ProjBTA);

    if(i==0) {
      lowCentATA=CentATA;
      lowCentBTA=CentBTA;
      lowRun=runNumb;
    } else{
      if(runNumb<lowRun)
	lowRun=runNumb;
      
      if(runNumb>highRun)
	highRun=runNumb;

      if(CentATA<lowCentATA)
	lowCentATA=CentATA;
      
      if(CentATA>highCentATA)
	highCentATA=CentATA;

      if(CentBTA<lowCentBTA)
	lowCentBTA=CentBTA;
      
      if(CentBTA>highCentBTA)
	highCentBTA=CentBTA;
    }

    if(NumFiles==1){
      highRun=lowRun;
      highCentATA=lowCentATA;
      highCentBTA=lowCentBTA;
    }

							     
  }// end loop over number of files
  TCanvas *GraphCanv = new TCanvas("GraphCanv","ATA / BTA Graph Canvas");
  GraphCanv->Divide(2);
  TH2F *ATAHolder = new TH2F("ATAHolder","ATA Summary;Run Number;ATA Centroid [radians]",
			     10,lowRun-5,highRun+5,10,lowCentATA-0.01,highCentATA+0.01);
  TH2F *BTAHolder = new TH2F("BTAHolder","BTA Summary;Run Number;BTA Centroid [radians]",
			     10,lowRun-5,highRun+5,10,lowCentBTA-0.01,highCentBTA+0.01);

  TLine *Line3 = new TLine(lowRun-5,0,highRun+5,0);
  Line3->SetLineColor(kBlack);
  Line3->SetLineStyle(kDashed);
  Line3->SetLineWidth(3);
  
  GraphCanv->cd(1);
  ATAHolder->Draw();
  ATAGraph->Draw("*same");
  Line3->Draw("same");
  GraphCanv->cd(2);
  BTAHolder->Draw();
  BTAGraph->Draw("*same");
  Line3->Draw("same");

  HistList->Add(ATAGraph);
  HistList->Add(BTAGraph);

  return HistList;
}


TList *CheckYTA(float low=0,float high=0){
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TGraph *YTAGraph = new TGraph(NumFiles);
  YTAGraph->SetMarkerColor(kRed);
  YTAGraph->SetName("YTA_Graph");
  TCanvas *YTACanv = new TCanvas("YTACanv","YTA Canvas");
  
  if(NumFiles%2==0){
    int FirstDivide = NumFiles/2;
    YTACanv->Divide(FirstDivide,2);
  }else if(NumFiles%3==0){ 
    int FirstDivide = NumFiles/3;
    YTACanv->Divide(FirstDivide,3);
  } else{
    YTACanv->Divide(NumFiles);
  }
  
  if(low==0 && high==0){
    low=-20;
    high=20;
  }
  
  double lowRun=0,highRun=0;
  double lowCent1=0,highCent1=0;
  cout << " Low = " << low << " High = " << high << endl;
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


    WorkingFile->cd("InverseMap");
    
    TH1I *Proj1 = S800_YTA;
    Proj1->GetXaxis()->SetRangeUser(low-10,high+10);
    double Proj1_yMax = Proj1->GetMaximum();
    
    YTACanv->cd(i+1);
    Proj1->Draw();
    TLine *Line1 = new TLine(0,0,0,Proj1_yMax);
    Line1->SetLineColor(kBlack);
    Line1->SetLineStyle(kDashed);
    Line1->SetLineWidth(3);
    Line1->Draw("Same");
    TF1 *gfit = new TF1("gfit","gaus",low,high);
    Proj1->Fit(gfit);
    double Cent1 = gfit->GetParameter("Mean");
    YTA_Centroid.push_back(Cent1);
    Proj1->SetTitle(Form("YTA Run%s Centoid = %.6f",run.c_str(),Cent1));
    YTAGraph->SetPoint(i,runNumb,Cent1);

    HistList->Add(Proj1);

    if(i==0) {
      lowCent1=Cent1;
      lowRun=runNumb;
    } else{
      if(runNumb<lowRun)
	lowRun=runNumb;
      
      if(runNumb>highRun)
	highRun=runNumb;

      if(Cent1<lowCent1)
	lowCent1=Cent1;
      
      if(Cent1>highCent1)
	highCent1=Cent1;
    }

    if(NumFiles==1){
      highRun=lowRun;
      highCent1=lowCent1;
    }

							     
  }
  TCanvas *GraphCanv = new TCanvas("GraphCanv","YTA Graph Canvas");
  TH2F *YTAHolder = new TH2F("YTAHolder","YTA Summary;Run Number;YTA Value [mm]",
			       10,lowRun-5,highRun+5,10,lowCent1-10,highCent1+10);

  TLine *Line3 = new TLine(lowRun-5,0,highRun+5,0);
  Line3->SetLineColor(kBlack);
  Line3->SetLineStyle(kDashed);
  Line3->SetLineWidth(3);
  
  GraphCanv->cd(1);
  YTAHolder->Draw();
  YTAGraph->Draw("*same");
  Line3->Draw("same");
  
  HistList->Add(YTAGraph);
  
  return HistList;
}

bool GetNewSlopes(double slope1=1,double offset1=0,double slope2=0,double offset2=0){
  if((slope1==1 && offset1==0) || (slope1==2 && offset2==0) ){
    cout << " Input a slope and offset please" << endl;
    return 0;
  }

  double NewSlope1;
  double NewSlope2;
  double UnCalVal1=0;
  double UnCalVal2=0;
  int Centroids = CRDC1_Y_Centroid.size();
  printf("------ CRDC 1 Old Slope = %.5f -- Old Offset = %.5f ------\n",slope1,offset1);
  printf("------ CRDC 2 Old Slope = %.5f -- Old Offset = %.5f ------\n",slope2,offset2);
  
  for(int i=0;i<Centroids;i++){
    UnCalVal1 = (CRDC1_Y_Centroid.at(i)-offset1)/(slope1);
    UnCalVal2 = (CRDC2_Y_Centroid.at(i)-offset2)/(slope2);
    
    NewSlope1 = (-offset1/UnCalVal1);
    NewSlope2 = (-offset2/UnCalVal2);

    printf(" - - - Run %f - - - \n",i); 
    printf(" CRDC1 : New Slope  = %.5f \n",NewSlope1);
    printf(" CRDC2 : New Slope  = %.5f \n",NewSlope2);
 
  }
  
  return 1;
}


TList *CheckCalibratedCRDCY(int low=0,int high=0){
  
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TGraph *CRDC1Graph = new TGraph(NumFiles);
  TGraph *CRDC2Graph = new TGraph(NumFiles);
  CRDC1Graph->SetMarkerColor(kRed);
  CRDC2Graph->SetMarkerColor(kRed);
  CRDC1Graph->SetName("CRDC1_Calibrated_Y_Values");
  CRDC1Graph->SetName("CRDC2_Calibrated_Y_Values");
  

  TCanvas *TwoDCanvas1 = new TCanvas("TwoDCanvas1","2D Canvas CRDC1");
  TCanvas *TwoDCanvas2 = new TCanvas("TwoDCanvas2","2D Canvas CRDC 2");
  TCanvas *CRDC1 = new TCanvas("CRDC1","CRDC1");
  TCanvas *CRDC2 = new TCanvas("CRDC2","CRDC2");
  
  if(NumFiles%2==0){
    int FirstDivide = NumFiles/2;
    TwoDCanvas1->Divide(FirstDivide,2);
    TwoDCanvas2->Divide(FirstDivide,2);
    CRDC1->Divide(FirstDivide,2);
    CRDC2->Divide(FirstDivide,2);
  }else if(NumFiles%3==0){ 
    int FirstDivide = NumFiles/3;
    TwoDCanvas1->Divide(FirstDivide,3);
    TwoDCanvas2->Divide(FirstDivide,3);
    CRDC1->Divide(FirstDivide,3);
    CRDC2->Divide(FirstDivide,3);
  } else{
    TwoDCanvas1->Divide(NumFiles);
    TwoDCanvas2->Divide(NumFiles);
    CRDC1->Divide(NumFiles);
    CRDC2->Divide(NumFiles);
  }
  
  if(low==0 && high==0){
    low=-400;
    high=400;
  }
  
  double lowRun=0,highRun=0;
  double lowCent1=0,highCent1=0;
  double lowCent2=0,highCent2=0;
  
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

    WorkingFile->cd("GetYOffset");
    TwoDCanvas1->cd(i+1);
    TLine *Line0 = new TLine(0,0,6000,0);
    Line0->SetLineColor(kBlack);
    Line0->SetLineStyle(kDashed);
    Line0->SetLineWidth(3);
    CRDC1_Y_vs_S800Timestamp->SetTitle(Form("CRDC1 Calibrated Y Run%s vs Timestamp",run.c_str()));
    CRDC1_Y_vs_S800Timestamp->Draw();
    Line0->Draw("same");
    TwoDCanvas2->cd(i+1);
    CRDC2_Y_vs_S800Timestamp->SetTitle(Form("CRDC2 Calibrated Y Run%s vs Timestamp",run.c_str()));
    CRDC2_Y_vs_S800Timestamp->Draw();
    Line0->Draw("same");
    
    TH1D *Proj1 = CRDC1_Y_vs_S800Timestamp->ProjectionY(Form("CRDC1_Calibrated_TotalY_Run%s",run.c_str()));
    TH1D *Proj2 = CRDC2_Y_vs_S800Timestamp->ProjectionY(Form("CRDC2_Calibrated_TotalY_Run%s",run.c_str()));


    double Proj1_yMax = Proj1->GetMaximum();
    double Proj2_yMax = Proj2->GetMaximum();
    
    // std::cout << " Proj1 Y Axis Max = " << Proj1_yMax << endl;
    // std::cout << " Proj2 Y Axis Max = " << Proj2_yMax << endl;


    CRDC1->cd(i+1);
    Proj1->Draw();
    TLine *Line1 = new TLine(0,0,0,Proj1_yMax);
    Line1->SetLineColor(kBlack);
    Line1->SetLineStyle(kDashed);
    Line1->SetLineWidth(3);
    Line1->Draw("Same");
    GausFit(Proj1,low,high);
    double Cent1 = GetCentroid(Proj1);
    CRDC1_Y_Centroid.push_back(Cent1);
    Proj1->SetTitle(Form("CRDC1 Calibrated Y Run%s Centoid = %.4f",run.c_str(),Cent1));
    CRDC1Graph->SetPoint(i,runNumb,Cent1);

    CRDC2->cd(i+1);
    Proj2->Draw();
    TLine *Line2 = new TLine(0,0,0,Proj2_yMax);
    Line2->SetLineColor(kBlack);
    Line2->SetLineStyle(kDashed);
    Line2->SetLineWidth(3);
    Line2->Draw("same");
    GausFit(Proj2,low,high);
    double Cent2 = GetCentroid(Proj2);
    CRDC2_Y_Centroid.push_back(Cent2);
    Proj2->SetTitle(Form("CRDC2 Calibrated Y Run%s Centoid = %.4f",run.c_str(),Cent2));
    CRDC2Graph->SetPoint(i,runNumb,Cent2);

    HistList->Add(Proj1);
    HistList->Add(Proj2);

    if(i==0) {
      lowCent1=Cent1;
      lowCent2=Cent2;
      lowRun=runNumb;
    } else{
      if(runNumb<lowRun)
	lowRun=runNumb;
      
      if(runNumb>highRun)
	highRun=runNumb;

      if(Cent1<lowCent1)
	lowCent1=Cent1;
      
      if(Cent1>highCent1)
	highCent1=Cent1;

      if(Cent2<lowCent2)
	lowCent2=Cent2;
      
      if(Cent2>highCent2)
	highCent2=Cent2;
    }

    if(NumFiles==1){
      highRun=lowRun;
      highCent1=lowCent1;
      highCent2=lowCent2;
    }

							     
  }
  TCanvas *GraphCanv = new TCanvas("GraphCanv","Calibrated Graph Canvas");
  GraphCanv->Divide(2);
  TH2F *CRDC1Holder = new TH2F("CRDC1Holder","CRDC 1 Calibrated Y Summary;Run Number;Calibrated CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent1-50,highCent1+50);
  TH2F *CRDC2Holder = new TH2F("CRDC2Holder","CRDC 2 Y Calibrated Summary;Run Number;Calibrated CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent2-50,highCent2+50);

  TLine *Line3 = new TLine(lowRun-5,0,highRun+5,0);
  Line3->SetLineColor(kBlack);
  Line3->SetLineStyle(kDashed);
  Line3->SetLineWidth(3);
  
  GraphCanv->cd(1);
  CRDC1Holder->Draw();
  CRDC1Graph->Draw("*same");
  Line3->Draw("same");
  GraphCanv->cd(2);
  CRDC2Holder->Draw();
  CRDC2Graph->Draw("*same");
  Line3->Draw("same");

  HistList->Add(CRDC1Graph);
  HistList->Add(CRDC2Graph);

  return HistList;
}

TList *CheckUnShiftedCRDCY(int low=0,int high=0){
  
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TGraph *CRDC1Graph = new TGraph(NumFiles);
  TGraph *CRDC2Graph = new TGraph(NumFiles);
  CRDC1Graph->SetMarkerColor(kRed);
  CRDC2Graph->SetMarkerColor(kRed);
  CRDC1Graph->SetName("CRDC1_UnShifted_Y_Values");
  CRDC1Graph->SetName("CRDC2_UnShifted_Y_Values");
  

  TCanvas *TwoDCanvas1 = new TCanvas("TwoDCanvas1","2D Canvas CRDC 1");
  TCanvas *TwoDCanvas2 = new TCanvas("TwoDCanvas2","2D Canvas CRDC 2");
  TCanvas *CRDC1 = new TCanvas("CRDC1","CRDC1");
  TCanvas *CRDC2 = new TCanvas("CRDC2","CRDC2");
  
  if(NumFiles%2==0){
    int FirstDivide = NumFiles/2;
    TwoDCanvas1->Divide(FirstDivide,2);
    TwoDCanvas2->Divide(FirstDivide,2);
    CRDC1->Divide(FirstDivide,2);
    CRDC2->Divide(FirstDivide,2);
  }else if(NumFiles%3==0){ 
    int FirstDivide = NumFiles/3;
    TwoDCanvas1->Divide(FirstDivide,3);
    TwoDCanvas2->Divide(FirstDivide,3);
    CRDC1->Divide(FirstDivide,3);
    CRDC2->Divide(FirstDivide,3);
  } else{
    TwoDCanvas1->Divide(NumFiles);
    TwoDCanvas2->Divide(NumFiles);
    CRDC1->Divide(NumFiles);
    CRDC2->Divide(NumFiles);
  }
  

  int low1,low2,high1,high2;
  if(low==0 && high==0){
    low1=-400;
    high1=400;
    low2=-400;
    high2=400;
  }else{
    low1=-high;
    high1=-low;
    low2=low;
    high2=high;
  }

  double lowRun=0,highRun=0;
  double lowCent1=0,highCent1=0;
  double lowCent2=0,highCent2=0;
  
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

    std::cout << " Run Number = " << runNumb << std::endl;
    
    WorkingFile->cd("GetYOffset");
    TwoDCanvas1->cd(i+1);
    CRDC1_Y_vs_S800Timestamp->SetTitle(Form("CRDC1 UnShifted Y Run%s vs Timestamp",run.c_str()));
    CRDC1_Y_vs_S800Timestamp->Draw();    
    TLine *Line0 = new TLine(0,0,6000,0);
    Line0->SetLineColor(kBlack);
    Line0->SetLineStyle(kDashed);
    Line0->SetLineWidth(3);
    Line0->Draw("same");
    TwoDCanvas2->cd(i+1);
    CRDC2_Y_vs_S800Timestamp->SetTitle(Form("CRDC2 UnShifted Y Run%s vs Timestamp",run.c_str()));
    CRDC2_Y_vs_S800Timestamp->Draw();    
    Line0->Draw("same");

    TH1D *Proj1 = CRDC1_Y_vs_S800Timestamp->ProjectionY(Form("CRDC1_UnShifted_TotalY_Run%s",run.c_str()));
    TH1D *Proj2 = CRDC2_Y_vs_S800Timestamp->ProjectionY(Form("CRDC2_UnShifted_TotalY_Run%s",run.c_str()));

    double Proj1_yMax = Proj1->GetMaximum();
    double Proj2_yMax = Proj2->GetMaximum();
  
    CRDC1->cd(i+1);
    Proj1->Draw();
    TLine *Line1 = new TLine(0,0,0,Proj1_yMax);
    Line1->SetLineColor(kBlack);
    Line1->SetLineStyle(kDashed);
    Line1->SetLineWidth(3);
    Line1->Draw("Same");
    GausFit(Proj1,low1,high1);
    double Cent1 = GetCentroid(Proj1);
    Proj1->SetTitle(Form("CRDC1 UnShifted Y Run%s Centoid = %.2f",run.c_str(),Cent1));
    CRDC1Graph->SetPoint(i,runNumb,Cent1);

    CRDC2->cd(i+1);
    Proj2->Draw();
    TLine *Line2 = new TLine(0,0,0,Proj2_yMax);
    Line2->SetLineColor(kBlack);
    Line2->SetLineStyle(kDashed);
    Line2->SetLineWidth(3);
    Line2->Draw("same");
    GausFit(Proj2,low2,high2);
    double Cent2 = GetCentroid(Proj2);
    Proj2->SetTitle(Form("CRDC2 UnShifted Y Run%s Centoid = %.2f",run.c_str(),Cent2));
    CRDC2Graph->SetPoint(i,runNumb,Cent2);

    HistList->Add(Proj1);
    HistList->Add(Proj2);

    if(i==0) {
      lowCent1=Cent1;
      lowCent2=Cent2;
      lowRun=runNumb;
    } else{
      if(runNumb<lowRun)
	lowRun=runNumb;
      
      if(runNumb>highRun)
	highRun=runNumb;

      if(Cent1<lowCent1)
	lowCent1=Cent1;
      
      if(Cent1>highCent1)
	highCent1=Cent1;

      if(Cent2<lowRun)
	lowCent2=Cent2;
      
      if(Cent2>highCent2)
	highCent2=Cent2;
    }

    if(NumFiles==1){
      highRun=lowRun;
      highCent1=lowCent1;
      highCent2=lowCent2;
    }

							     
  }
  TCanvas *GraphCanv = new TCanvas("GraphCanv","UnShifted Graph Canvas");
  GraphCanv->Divide(2);
  TH2F *CRDC1Holder = new TH2F("CRDC1Holder","CRDC 1 UnShifted Y Summary;Run Number;UnShifted CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent1-50,highCent1+50);
  TH2F *CRDC2Holder = new TH2F("CRDC2Holder","CRDC 2 Y UnShifted Summary;Run Number;UnShifted CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent2-50,highCent2+50);

  TLine *Line3 = new TLine(lowRun-5,0,highRun+5,0);
  Line3->SetLineColor(kBlack);
  Line3->SetLineStyle(kDashed);
  Line3->SetLineWidth(3);

  GraphCanv->cd(1);
  CRDC1Holder->Draw();
  CRDC1Graph->Draw("*same");
  //  Line3->Draw("same");
  GraphCanv->cd(2);
  CRDC2Holder->Draw();
  CRDC2Graph->Draw("*same");
  //Line3->Draw("same");

  HistList->Add(CRDC1Graph);
  HistList->Add(CRDC2Graph);

  return HistList;
}


TList *CheckRawCrdcY(int low=0,int high=0){
  
  TList *HistList = new TList;
  TList *FileList = gROOT->GetListOfFiles();
  int NumFiles = FileList->GetSize();
  TGraph *CRDC1Graph = new TGraph(NumFiles);
  TGraph *CRDC2Graph = new TGraph(NumFiles);
  CRDC1Graph->SetMarkerColor(kRed);
  CRDC2Graph->SetMarkerColor(kRed);
  CRDC1Graph->SetName("CRDC1_Raw_Y_Values");
  CRDC1Graph->SetName("CRDC2_Raw_Y_Values");
  
  TCanvas *TwoDCanvas1 = new TCanvas("TwoDCanvas1","2D Canvas CRDC1");
  TCanvas *TwoDCanvas2 = new TCanvas("TwoDCanvas2","2D Canvas CRDC2");
  TCanvas *CRDC1 = new TCanvas("CRDC1","CRDC1");
  TCanvas *CRDC2 = new TCanvas("CRDC2","CRDC2");
  
  if(NumFiles%2==0){
    int FirstDivide = NumFiles/2;
    TwoDCanvas1->Divide(FirstDivide,2);
    TwoDCanvas2->Divide(FirstDivide,2);
    CRDC1->Divide(FirstDivide,2);
    CRDC2->Divide(FirstDivide,2);
  }else if(NumFiles%3==0){ 
    int FirstDivide = NumFiles/3;
    TwoDCanvas1->Divide(FirstDivide,3);
    TwoDCanvas2->Divide(FirstDivide,3);
    CRDC1->Divide(FirstDivide,3);
    CRDC2->Divide(FirstDivide,3);
  } else{
    TwoDCanvas1->Divide(NumFiles);
    TwoDCanvas2->Divide(NumFiles);
    CRDC1->Divide(NumFiles);
    CRDC2->Divide(NumFiles);
  }
  
  if(low==0 && high==0){
    low=-4000;
    high=-4000;
  }

  double lowRun=0,highRun=0;
  double lowCent1=0,highCent1=0;
  double lowCent2=0,highCent2=0;
  
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
    
    WorkingFile->cd("GetYOffset");
    TwoDCanvas1->cd(i+1);
    CRDC1_Y_vs_S800Timestamp_UnCal->SetTitle(Form("CRDC1 Raw Y Run%s vs Timestamp",run.c_str()));
    CRDC1_Y_vs_S800Timestamp_UnCal->Draw();
    TLine *Line0 = new TLine(0,0,6000,0);
    Line0->SetLineColor(kBlack);
    Line0->SetLineStyle(kDashed);
    Line0->SetLineWidth(3);
    Line0->Draw("same");
    TwoDCanvas2->cd(i+1);
    CRDC2_Y_vs_S800Timestamp_UnCal->SetTitle(Form("CRDC2 Raw Y Run%s vs Timestamp",run.c_str()));
    CRDC2_Y_vs_S800Timestamp_UnCal->Draw();
    Line0->Draw("same");
    
    TH1D *Proj1 = CRDC1_Y_vs_S800Timestamp_UnCal->ProjectionY(Form("CRDC1_Raw_TotalY_Run%s",run.c_str()));
    TH1D *Proj2 = CRDC2_Y_vs_S800Timestamp_UnCal->ProjectionY(Form("CRDC2_Raw_TotalY_Run%s",run.c_str()));
  
    CRDC1->cd(i+1);
    Proj1->Draw();
    TLine *Line1 = new TLine(0,0,0,Proj1_yMax);
    Line1->SetLineColor(kBlack);
    Line1->SetLineStyle(kDashed);
    Line1->SetLineWidth(3);
    //Line1->Draw("Same");
    GausFit(Proj1,low,high);
    double Cent1 = GetCentroid(Proj1);
    Proj1->SetTitle(Form("CRDC1 Raw Y Run%s Centoid = %.2f",run.c_str(),Cent1));
    CRDC1Graph->SetPoint(i,runNumb,Cent1);

    CRDC2->cd(i+1);
    Proj2->Draw();
    TLine *Line2 = new TLine(0,0,0,Proj2_yMax);
    Line2->SetLineColor(kBlack);
    Line2->SetLineStyle(kDashed);
    Line2->SetLineWidth(3);
    //Line2->Draw("same");
    GausFit(Proj2,low,high);
    double Cent2 = GetCentroid(Proj2);
    Proj2->SetTitle(Form("CRDC2 Raw Y Run%s Centoid = %.2f",run.c_str(),Cent2));
    CRDC2Graph->SetPoint(i,runNumb,Cent2);

    HistList->Add(Proj1);
    HistList->Add(Proj2);

    if(i==0) {
      lowCent1=Cent1;
      lowCent2=Cent2;
      lowRun=runNumb;
    } else{
      if(runNumb<lowRun)
	lowRun=runNumb;
      
      if(runNumb>highRun)
	highRun=runNumb;

      if(Cent1<lowCent1)
	lowCent1=Cent1;
      
      if(Cent1>highCent1)
	highCent1=Cent1;

      if(Cent2<lowRun)
	lowCent2=Cent2;
      
      if(Cent2>highCent2)
	highCent2=Cent2;
    }

    if(NumFiles==1){
      highRun=lowRun;
      highCent1=lowCent1;
      highCent2=lowCent2;
    }

							     
  }
  TCanvas *GraphCanv = new TCanvas("GraphCanv","Raw CRDC Canvas");
  GraphCanv->Divide(2);
  TH2F *CRDC1Holder = new TH2F("CRDC1Holder","CRDC 1 Raw Y Summary;Run Number;Raw CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent1-100,highCent1+100);
  TH2F *CRDC2Holder = new TH2F("CRDC2Holder","CRDC 2 Raw Y Summary;Run Number;Raw CRDC Y Centroid",
			       10,lowRun-5,highRun+5,10,lowCent2-100,highCent2+100);

  TLine *Line3 = new TLine(lowRun-5,0,highRun+5,0);
  Line3->SetLineColor(kBlack);
  Line3->SetLineStyle(kDashed);
  Line3->SetLineWidth(3);

  GraphCanv->cd(1);
  CRDC1Holder->Draw();
  CRDC1Graph->Draw("*same");
  //Line3->Draw("same");
  GraphCanv->cd(2);
  CRDC2Holder->Draw();
  CRDC2Graph->Draw("*same");
  //Line3->Draw("same");

  HistList->Add(CRDC1Graph);
  HistList->Add(CRDC2Graph);

  return HistList;
}

double GetCentroid(TH1* CentHist){
  GGaus *peak=0;
  double Centroid=0;
  TList *CentList = CentHist->GetListOfFunctions();
  int NumFuncs = CentList->GetSize();
  for(int x=0;x<NumFuncs;x++){
    TObject *obj = CentList->At(x);
    if(!(obj->InheritsFrom("GGaus")))
      continue;
    
    peak=(GGaus*)obj;
    break;
  }
  Centroid = peak->GetCentroid();
  return Centroid;
}
