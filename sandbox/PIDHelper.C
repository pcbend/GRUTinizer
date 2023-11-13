/*******************************************************************************/
/* Draws gates from .pid file created by TGates on current canvas **************/
/* and gets number of events in gates. *****************************************/
/* Requires as argument path to .pid file **************************************/
/* At Present supports 2D gates only *******************************************/
/*******************************************************************************/
using namespace std;
void drawPID(std::string filepath) {
  //Gets current Canvas, if unable to find stops exexution
  TH2F *tmpHist = NULL;
  TPad *pad = (TPad*)gPad;
  if(pad == NULL) {
    cout << "Need To Open (G/T)Canvas" << endl;
    return;
  }

  //Gets current histogram from Canvas, if unable to find stops exexution
  TList *newlist = pad->GetListOfPrimitives();
  for(auto obj: *newlist) {
    if(strcmp(obj->ClassName(), "TH2F") == 0) {
      tmpHist = (TH2F*) newlist->FindObject(obj->GetName());
      break;
    }
  }

  if(tmpHist == NULL) {
    cout << "TH2F not found" << endl;
    return;
  }


  //Opens .pid text file aborts if file not found
  int Nlines = 0;
  ifstream zFile;
  zFile.open(filepath);
  if(!zFile.is_open()) {
    cout << "File not Found Aborting" << endl;
    return;
  }
  zFile >> Nlines;
  TCutG *zline[22];
  char name[10];
  //Loads in text file and defines TCutG
  cout << "Nucleus\tN(InGate)" << endl;
  for (int i = 0; i < Nlines; i++) {
    int Npoints, iZ, iA;
    zFile >> iZ >> iA;
    zFile >> Npoints;
    sprintf(name,"zline_%i_%i",iA,iZ);
    zline[i] = new TCutG(name,Npoints);
    for (int j = 0; j < Npoints; j++) {
      double x,y;
      zFile >> x >> y;
      zline[i]->SetPoint(j,x,y);
    }
    //Draw TCutG
    zline[i]->Draw("same");
    //Find out integral of histogram and output to terminal
    int gcount = zline[i]->IntegralHist(tmpHist);
    TNucleus *tmpN = new TNucleus(iZ, iA - iZ);
    cout << tmpN->GetSymbol() << "\t" << gcount << endl;
  }

  zFile.close();
}


/*******************************************************************************/
/* Used to make a PID file by drawing gates on the current canvas **************/
/* Requires as argument the name of the new .pid file **************************/
/*******************************************************************************/
void makePID(std::string filename) {

  //Gets current Canvas, if unable to find stops exexution
  TH2F *tmpHist = NULL;
  TH1F *tmpHist1D = NULL;
  TPad *pad = (TPad*)gPad;
  if(pad == NULL) {
    cout << "Need To Open (G/T)Canvas" << endl;
    return;
  }
  bool gate2d = false;
  //Gets current histogram from Canvas, if unable to find stops exexution
  TList *newlist = pad->GetListOfPrimitives();
  for(auto obj: *newlist) {
    if(strcmp(obj->ClassName(), "TH2F") == 0) {
      tmpHist = (TH2F*) newlist->FindObject(obj->GetName());
      gate2d = true;
      break;
    }
    if(strcmp(obj->ClassName(), "TH1F") == 0) {
      tmpHist1D = (TH1F*) newlist->FindObject(obj->GetName());
      gate2d = false;
      break;
    }
  }

  if(tmpHist == NULL && tmpHist1D == NULL) {
    cout << "TH1F or TH2F not found" << endl;
    return;
  }

  TGates *g = new TGates();
  cout << "Draw Gates on Canvas" << endl;
  if(gate2d) g->MakePIDFile(filename.c_str());
  else g->Make1DPIDFile(filename.c_str());
}

/*******************************************************************************/
/* Draws gates from .gate file created by TGates on current canvas *************/
/* and gets number of events in gates. *****************************************/
/* Requires as argument path to .gate file *************************************/
/* At Present supports 2D gates only *******************************************/
/*******************************************************************************/
void drawGate(std::string filepath) {
  //Gets current Canvas, if unable to find stops execution
  TH2F *tmpHist = NULL;
  TPad *pad = (TPad*)gPad;
  if(pad == NULL) {
    cout << "Need To Open (G/T)Canvas" << endl;
    return;
  }

  //Gets current histogram from Canvas, if unable to find stops exexution
  TList *newlist = pad->GetListOfPrimitives();
  for(auto obj: *newlist) {
    if(strcmp(obj->ClassName(), "TH2F") == 0) {
      tmpHist = (TH2F*) newlist->FindObject(obj->GetName());
      break;
    }
  }

  if(tmpHist == NULL) {
    cout << "TH2F not found" << endl;
    return;
  }


  //Opens .pid text file aborts if file not found
  ifstream zFile;
  zFile.open(filepath);
  if(!zFile.is_open()) {
    cout << "File not Found Aborting" << endl;
    return;
  }
  TCutG *zline;
  char name[10];
  //Loads in text file and defines TCutG
  cout << "N(InGate) = ";
  int Npoints;
  zFile >> Npoints;
  sprintf(name,"zline");
  zline = new TCutG(name,Npoints);
  for (int j = 0; j < Npoints; j++) {
    double x,y;
    zFile >> x >> y;
    zline->SetPoint(j,x,y);
  }
  //Draw TCutG
  zline->Draw("same");
  //Find out integral of histogram and output to terminal
  int gcount = zline->IntegralHist(tmpHist);
  cout << gcount << endl;
  zFile.close();
}

/*******************************************************************************/
/* Used to make a gate file by drawing gates on the current canvas *************/
/* Requires as argument the name of the .gate file *****************************/
/*******************************************************************************/
void makeGate(std::string filename) {

  //Gets current Canvas, if unable to find stops exexution
  TH2F *tmpHist = NULL;
  TPad *pad = (TPad*)gPad;
  if(pad == NULL) {
    cout << "Need To Open (G/T)Canvas" << endl;
    return;
  }

  //Gets current histogram from Canvas, if unable to find stops exexution
  TList *newlist = pad->GetListOfPrimitives();
  for(auto obj: *newlist) {
    if(strcmp(obj->ClassName(), "TH2F") == 0) {
      tmpHist = (TH2F*) newlist->FindObject(obj->GetName());
      break;
    }
  }

  if(tmpHist == NULL) {
    cout << "TH2F not found" << endl;
    return;
  }

  TGates *g = new TGates();
  cout << "Draw Gates on Canvas" << endl;
  g->MakeGateFile(filename.c_str());
}

