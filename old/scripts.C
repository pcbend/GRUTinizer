

TTree *tree = NULL;
TMode3 *mode3 = new TMode3;

static Int_t evno=0;

TH1 *makeHisto(TMode3 *m3, const char *hname="waveHist") {
  TObject *obj = gROOT->FindObjectAny(hname);
  if(obj) 
    obj->Delete();
  string htitle = Form("Hole[%02i] Crystal[%i] Board[%i] Channel[%i]  ts:%lu",m3->GetHole(),m3->GetCrystal(),m3->GetVME(),m3->GetChannel(),m3->GetLed())  ;

  TH1 *his = new TH1I(hname,htitle.c_str(),m3->GetWaveSize(),0,m3->GetWaveSize());
  for(int i=0;i<m3->GetWaveSize();i++)
    his->SetBinContent(i+1,m3->GetWave()[i]);
  return his;
}

void DrawNext(int hole=-1,int crystal=-1,int vme=-1,int channel=-1)  {
  if (tree == NULL) {
    tree = (TTree*)gROOT->FindObject("Data");
  }
  long entmax = tree->GetEntries();
  tree->SetBranchAddress("TMode3", &mode3);
  tree->GetEntry(evno++);
  if(hole == -1) {
    hole = mode3->GetHole();
  }
  if(crystal == -1) {
    crystal = mode3->GetCrystal();
  }
  if(vme == -1)  {
    vme = mode3->GetVME();
  }
  if(channel == -1) {
    channel = mode3->GetChannel();
  }
  while ( (mode3->GetWaveSize()<1)       || 
          (hole!=mode3->GetHole())       || 
          (crystal!=mode3->GetCrystal()) || 
          (vme!=mode3->GetVME())         || 
          (channel!=mode3->GetChannel()) ) {
    tree->GetEntry(evno++);
    if(hole == -1) {
      hole = mode3->GetHole();
    }
    if(crystal == -1) {
      crystal = mode3->GetCrystal();
    }
    if(vme == -1)  {
      vme = mode3->GetVME();
    }
    if(channel == -1) {
      channel = mode3->GetChannel();
    }
    if(evno == entmax) {
      evno = 0;
      cout << "at end of tree, entry number reset to zero." << endl;
      return;
    }
  }
  cout << "Event number " << evno << endl;

  //frag->Print();
  //printf("wavebuffer.size() = %i\n",wavebuffer.size());
  TH1 *his = makeHisto(mode3);
  //TChannel *chan = TChannel::GetChannel(frag->ChannelAddress);
  //if(chan && (strncmp(chan->GetChannelName(),"DSC",3)==0))
  //  TH1 *his = makeDescantHisto(frag->wavebuffer);
//  if(chan)
//    his->SetTitle(chan->GetChannelName());
  his->DrawCopy();
}



void DrawPrev(int hole=-1,int crystal=-1,int vme=-1,int channel=-1)  {
  //static Int_t evno=0;
  if (tree == NULL) {
    tree = (TTree*)gROOT->FindObject("Data");
  }
  long entmax = tree->GetEntries();
  if(evno<1)
    evno = entmax;
  tree->SetBranchAddress("TMode3", &mode3);
  tree->GetEntry(evno--);
  if(hole == -1) {
    hole = mode3->GetHole();
  }
  if(crystal == -1) {
    crystal = mode3->GetCrystal();
  }
  if(vme == -1)  {
    vme = mode3->GetVME();
  }
  if(channel == -1) {
    channel = mode3->GetChannel();
  }
  while ( (mode3->GetWaveSize()<1) || 
          (hole!=mode3->GetHole()) || 
          (crystal!=mode3->GetCrystal()) || 
          (vme!=mode3->GetVME())   || 
          (channel!=mode3->GetChannel()) ) {
    tree->GetEntry(evno--);
    if(hole == -1) {
      hole = mode3->GetHole();
    }
    if(crystal == -1) {
      crystal = mode3->GetCrystal();
    }
    if(vme == -1)  {
      vme = mode3->GetVME();
    }
    if(channel == -1) {
      channel = mode3->GetChannel();
    }
    if(evno == 0) {
      evno = entmax;
      cout << "at tree entry zero, entry number reset to end." << endl;
      return;
    }
  }
  cout << "Event number " << evno << endl;

  //frag->Print();
  //printf("wavebuffer.size() = %i\n",wavebuffer.size());
  TH1 *his = makeHisto(mode3);
  //TChannel *chan = TChannel::GetChannel(frag->ChannelAddress);
  //if(chan && (strncmp(chan->GetChannelName(),"DSC",3)==0))
  //  TH1 *his = makeDescantHisto(frag->wavebuffer);
//  if(chan)
//    his->SetTitle(chan->GetChannelName());
  his->DrawCopy();
}




