void CalibrateRunWise(int Ri=0,int Rf=20) {

/*
  std::vector<std::array<int,2>> ranges_1461 = {{4400,5200},{4800,5650},{4400,5650},{4500,5300},
						{4400,5200},{4400,7500},{4400,5200},{4400,5650},
						{4750,6000},{4650,5100},{4400,5500},{4400,5200},
						{5000,6200},{5000,6200},{4650,5500},{4000,5000}};

*/
  std::vector<std::array<int,2>> ranges_1461 = {{9800,11500},{10700,12500},{9800,12500},{10000,11800},
						{9800,11500},{9800,16500},{9800,11500},{9800,12500},
						{10600,13300},{10600,11300},{9400,12200},{9800,11500},
						{11200,13800},{11200,13800},{10600,12300},{9100,11000}};

  double y = 1460.8;

  for(int i=Ri;i<Rf+1;i++) {
    
    std::string n = Form("hist%i.root",i);

    TFile* f = new TFile(n.c_str());
    if(f->IsZombie()) {
      continue; 
    }
    
    std::string cal_name = Form("cal_output%i.cal",i);
    std::ofstream cal_file(cal_name.c_str());

    for(int det=0;det<16;det++) {
      GH2* h2 = (GH2*)f->Get(Form("SeGA/SegChargeSummary_det%02i",det+1));
      std::cout << det << std::endl;
    
      for(int j=0;j<32;j++) {
      
        GH1* h = (GH1*)h2->ProjectionY("",j+2,j+2);
        h->Rebin(2);

        int b1 = h->FindBin(6000); //ranges_1461.at(det)[0]);
        int b2 = h->FindBin(15000); //ranges_1461.at(det)[1]);

        int bmax;
        int cmax = 0;
        for(int k=b1;k<b2+1;k++) {
	  int tmp = h->GetBinContent(k);
	  if(tmp > cmax) {
	    bmax = k;
	    cmax = tmp;
	  }
        }
        double x = h->GetBinCenter(bmax);

        double slope = y/x;
        double offset = 0;

        cal_file << Form("SEG%02iAN%02i",det+1,j+1) << "\n";
        cal_file << "  " << "EnergyCoeff: " << offset << " " << slope << "\n";
      }
    }

  }

  return;
}
