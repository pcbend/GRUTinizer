void CalibrateRunWise(int Ri=0,int Rf=20) {

/*
  std::vector<std::array<int,2>> ranges_2613 = {{9000,9500},{9400,10300},{8700,9250},{8900,9600},
						{8700,9250},{7200,8100},{8700,9250},{8700,9250},
						{8500,9000},{8500,9500},{8400,8900},{8500,9000},
						{9450,10000},{8500,9000},{8500,9000},{8100,9000}};

  std::vector<std::array<int,2>> ranges_1461 = {{5000,5500},{5250,5650},{4800,5150},{4950,5350},
						{4800,5150},{3800,4650},{4800,5150},{4800,5150},
						{4750,5100},{4950,5400},{2650,5050},{4800,5150},
						{5050,5800},{4750,5100},{4750,5100},{4500,5400}};

  std::vector<std::array<int,2>> ranges_352 = {{1150,1350},{1150,1450},{1050,1350},{1100,1400},{1050,1350},
                                               {900,1100},{1050,1350},{1050,1350},{1100,1300},{1100,1350},
                                               {1000,1300},{1100,1350},{1100,1450},{1100,1300},{1100,1300},
                                               {1050,1260}};

  std::vector<std::array<int,2>> ranges_511 = {{1550,1950},{1700,2075},{1500,1900},{1600,2000},{1550,1950},
                                               {1200,1600},{1550,1950},{1550,1950},{1500,1850},{1550,1900},
                                               {1500,1800},{1500,1800},{1600,2000},{1500,1900},{1500,1900},
                                               {1400,1800}};

*/
  std::vector<std::array<int,2>> ranges_2613 = {{19500,20500},{20800,22800},{20000,21000},{19800,21200},
						{19500,20500},{16000,18000},{19500,20500},{19500,20500},
						{19000,20000},{19000,21000},{18700,19700},{19500,20500},
						{21000,22000},{19000,20000},{19000,20000},{18000,20000}};

  std::vector<std::array<int,2>> ranges_1461 = {{10800,11500},{11700,12500},{11000,12000},{11000,11800},
						{10800,11500},{8500,10400},{10800,11500},{10800,11500},
						{10600,11300},{11000,12000},{10400,11200},{10800,11500},
						{11200,12800},{10600,11300},{10600,11300},{10000,12000}};

  std::vector<std::array<int,2>> ranges_352 = {{2400,3000},{2600,3200},{2400,3000},{2500,3100},{2400,3000},
                                               {2000,2400},{2400,3000},{2400,3000},{2300,2900},{2400,3000},
                                               {2300,2900},{2500,3000},{2500,3200},{2300,2900},{2300,2900},
                                               {2400,2800}};

  std::vector<std::array<int,2>> ranges_511 = {{3500,4300},{3800,4600},{3400,4200},{3600,4400},{3500,4300},
                                               {2700,3500},{3500,4300},{3500,4300},{3300,4100},{3400,4200},
                                               {3300,4100},{3400,4200},{3600,4400},{3400,4200},{3400,4200},
                                               {3200,4000}};

  double y1 = 351.8;
  double y2 = 511.0;
  double y3 = 1460.8;
  double y4 = 2614.5;

  std::ifstream ref_file("config/channels.cal");
  
  std::string mrk = "EnergyCoeff:";
  std::string line;
  
  for(int i=Ri;i<Rf+1;i++) {
    
    std::string n = Form("hist%i.root",i);
    TFile* f = new TFile(n.c_str());
    if(f->IsZombie()) {
      continue; 
    }
    
    GH2* h2 = (GH2*)f->Get("SeGA/CoreCharge_Summary");
    
    std::string cal_name = Form("channels%i.cal",i);
    std::ofstream cal_file(cal_name.c_str());

    for(int j=0;j<16;j++) {
      
      GH1* h = (GH1*)h2->ProjectionY("",j+2,j+2);
      h->Rebin(2);

      int b1 = h->FindBin(ranges_352.at(j)[0]);
      int b2 = h->FindBin(ranges_352.at(j)[1]);
      
      int bmax;
      int cmax = 0;
      for(int k=b1;k<b2+1;k++) {
	int tmp = h->GetBinContent(k);
	if(tmp > cmax) {
	  bmax = k;
	  cmax = tmp;
	}
      }
      double x1 = h->GetBinCenter(bmax);

      b1 = h->FindBin(ranges_511.at(j)[0]);
      b2 = h->FindBin(ranges_511.at(j)[1]);
      
      bmax;
      cmax = 0;
      for(int k=b1;k<b2+1;k++) {
	int tmp = h->GetBinContent(k);
	if(tmp > cmax) {
	  bmax = k;
	  cmax = tmp;
	}
      }
      double x2 = h->GetBinCenter(bmax);

      b1 = h->FindBin(ranges_1461.at(j)[0]);
      b2 = h->FindBin(ranges_1461.at(j)[1]);

      cmax = 0;
      for(int k=b1;k<b2+1;k++) {
	int tmp = h->GetBinContent(k);
	if(tmp > cmax) {
	  bmax = k;
	  cmax = tmp;
	}
      }
      double x3 = h->GetBinCenter(bmax);

      b1 = h->FindBin(ranges_2613.at(j)[0]);
      b2 = h->FindBin(ranges_2613.at(j)[1]);

      cmax = 0;
      for(int k=b1;k<b2+1;k++) {
	int tmp = h->GetBinContent(k);
	if(tmp > cmax) {
	  bmax = k;
	  cmax = tmp;
	}
      }
      double x4 = h->GetBinCenter(bmax);
      
      //std::cout << x1 << "," << x2 << "," << x3 << "," << x4 << std::endl;
      double xbar = (x1 + x2 + x3 + x4)/4;
      double ybar = (351.8 + 511 + 1460.8 + 2614.5)/4;
      double numerator_sum = (x1 - xbar)*(y1 - ybar) + (x2 - xbar)*(y2 - ybar) + (x3 - xbar)*(y3 - ybar) + (x4 - xbar)*(y4 - ybar);
      double denominator_sum = pow(x1 - xbar,2.0) + pow(x2 - xbar,2.0) + pow(x3 - xbar,2.0) + pow(x4 - xbar,2.0);
      double slope = numerator_sum/denominator_sum;
      double offset = ybar - slope*xbar;

      while(std::getline(ref_file,line)) {
	
	if(line.find(mrk) != std::string::npos) {
	  cal_file << "  " << "EnergyCoeff: " << offset << " " << slope << "\n";
	  break;
	}

	cal_file << line << "\n";
	
      }
      
    }

    while(std::getline(ref_file,line)) {
      cal_file << line << "\n";
    }
    cal_file.close();

    ref_file.clear();
    ref_file.seekg(0);

  }

  return;
}
