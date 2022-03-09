void CalibrateJanus(int Ri=0,int Rf=20) {

//Define approximate ranges where we expect to find the scattered projectile kinematic line in both the upstream and downstream detectors, for each ring. 
  std::vector<std::array<int,2>> ranges_downstream_rings = {{15000,25000},{15000,25000},{15000,25000},{15000,25000},
                                                            {15000,25000},{15000,25000},{15000,25000},{15000,25000},
                                                            {10000,20000},{10000,20000},{10000,20000},{10000,20000},
                                                            {10000,20000},{10000,20000},{10000,20000},{10000,20000},
                                                            {10000,20000},{10000,20000},{10000,20000},{10000,20000},
                                                            {10000,25000},{10000,25000},{10000,25000},{10000,20000}};

  std::vector<std::array<int,2>> ranges_upstream_rings = {{0,5000},{0,5000},{0,5000},{0,5000},
						{0,5000},{0,5000},{0,5000},{0,5000},
						{0,5000},{0,5000},{0,5000},{0,5000},
						{0,5000},{0,5000},{0,5000},{0,5000},
						{0,5000},{0,5000},{0,5000},{0,5000},
						{0,5000},{0,5000},{0,5000},{0,5000}};

//Define approximate ranges where we expect to find the scattered projectiles in both the upstream and downstream sectors. Doesn't need to be set for each individual sector, since the values shouldn't change much.
  int ranges_downstream_sectors [] = {10000,25000};
  int ranges_upstream_sectors [] = {0,5000};

  int badSectors0 [] = {10,11,12,26,27,28};
  int badRings0 [] = {14,15,16};
  int badSectors1 [] = {10,26,27};
  int badRings1 [] = {16};

  std::ifstream ref_file("config/channels.cal");
  
  std::string mrk = "EnergyCoeff:";
  std::string line;
  
  for(int i=Ri;i<Rf+1;i++) {
    
    std::string n = Form("hist%i.root",i);
    TFile* f = new TFile(n.c_str());
    if(f->IsZombie()) {
      continue; 
    }
    
    GH2* h_s0 = (GH2*)f->Get("Primary_Janus_Ungated/All_SectorCharge_v_Sector_det0");
    GH2* h_s1 = (GH2*)f->Get("Primary_Janus_Ungated/All_SectorCharge_v_Sector_det1");
    GH2* h_r0 = (GH2*)f->Get("Primary_Janus_Ungated/All_RingCharge_v_Ring_det0");
    GH2* h_r1 = (GH2*)f->Get("Primary_Janus_Ungated/All_RingCharge_v_Ring_det1");
    
    std::string cal_name = Form("cal_output%i.cal",i);
    std::ofstream cal_file(cal_name.c_str());

//upstream sectors
    int maxCharges_sec [32];
    double offsets_sec [32];
    for(int j=0;j<32;j++) {
      
      GH1* h = (GH1*)h_s0->ProjectionY("",j+2,j+2);
      h->Rebin(10);

      int b1 = h->FindBin(ranges_upstream_sectors[0]);
      int b2 = h->FindBin(ranges_upstream_sectors[1]);
      
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

      maxCharges_sec[j] = x;
//Check if the current sector is a bad sector
      if(std::find(std::begin(badSectors0), std::end(badSectors0), j+1) == std::end(badSectors0)) {
        offsets_sec[j] = 0;
      }
      else {
        offsets_sec[j] = -1;
      }

    }

//leftBound will store the last good charge value that we can use for the left bound of the interpolation
    int leftBound = -1;
    int rightBound = -1;
    int fittedCharge;
    for(int j=0;j<32;j++) {
      if(offsets_sec[j] == -1) {
        if(leftBound == -1) {
          std::cout << "ERROR: No good left bound for upstream sector " << j+1 << std::endl;
        }
        //here we find the right bound for the interpolation
        rightBound = -1;
        for(int k=j+1;k<32;k++) {
          if(offsets_sec[k] == 0 && rightBound == -1) {
            rightBound = k;
          }
        }
        if(rightBound == -1) {
          std::cout << "ERROR: No good right bound for upstream sector " << j+1 << std::endl;
        }
        fittedCharge = (maxCharges_sec[rightBound] - maxCharges_sec[leftBound])*(j - leftBound)/(rightBound - leftBound) + maxCharges_sec[leftBound];
        offsets_sec[j] = fittedCharge - maxCharges_sec[j];
      }
      else {
        leftBound = j;
      }
      cal_file << Form("JAN00BN%02i",j+1) << "\n";
      cal_file << "  " << "EnergyCoeff: " << offsets_sec[j] << " 1.0" << "\n";
    }

//upstream rings
    int maxCharges_ring [24];
    double offsets_ring [24];
    for(int j=0;j<24;j++) {
      
      GH1* h = (GH1*)h_r0->ProjectionY("",j+2,j+2);
      h->Rebin(10);

      int b1 = h->FindBin(ranges_upstream_rings.at(j)[0]);
      int b2 = h->FindBin(ranges_upstream_rings.at(j)[1]);
      
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

      maxCharges_ring[j] = x;
//Check if the current sector is a bad sector
      if(std::find(std::begin(badRings0), std::end(badRings0), j+1) == std::end(badRings0)) {
        offsets_ring[j] = 0;
      }
      else {
        offsets_ring[j] = -1;
      }

    }

//leftBound will store the last good charge value that we can use for the left bound of the interpolation
    leftBound = -1;
    rightBound = -1;
    for(int j=0;j<24;j++) {
      if(offsets_ring[j] == -1) {
        if(leftBound == -1) {
          std::cout << "ERROR: No good left bound for upstream ring " << j+1 << std::endl;
        }
        //here we find the right bound for the interpolation
        rightBound = -1;
        for(int k=j+1;k<24;k++) {
          if(offsets_ring[k] == 0 && rightBound == -1) {
            rightBound = k;
          }
        }
        if(rightBound == -1) {
          std::cout << "ERROR: No good right bound for upstream ring " << j+1 << std::endl;
        }
        fittedCharge = (maxCharges_ring[rightBound] - maxCharges_ring[leftBound])*(j - leftBound)/(rightBound - leftBound) + maxCharges_ring[leftBound];
        offsets_ring[j] = fittedCharge - maxCharges_ring[j];
      }
      else {
        leftBound = j;
      }
      cal_file << Form("JAN00FN%02i",j+1) << "\n";
      cal_file << "  " << "EnergyCoeff: " << offsets_ring[j] << " 1.0" << "\n";
    }

//downstream sectors
    for(int j=0;j<32;j++) {
      
      GH1* h = (GH1*)h_s1->ProjectionY("",j+2,j+2);
      h->Rebin(20);

      int b1 = h->FindBin(ranges_downstream_sectors[0]);
      int b2 = h->FindBin(ranges_downstream_sectors[1]);
      
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

      maxCharges_sec[j] = x;
//Check if the current sector is a bad sector
      if(std::find(std::begin(badSectors1), std::end(badSectors1), j+1) == std::end(badSectors1)) {
        offsets_sec[j] = 0;
      }
      else {
        offsets_sec[j] = -1;
      }

    }

//leftBound will store the last good charge value that we can use for the left bound of the interpolation
    leftBound = -1;
    rightBound = -1;
    for(int j=0;j<32;j++) {
      if(offsets_sec[j] == -1) {
        if(leftBound == -1) {
          std::cout << "ERROR: No good left bound for downstream sector " << j+1 << std::endl;
        }
        //here we find the right bound for the interpolation
        rightBound = -1;
        for(int k=j+1;k<32;k++) {
          if(offsets_sec[k] == 0 && rightBound == -1) {
            rightBound = k;
          }
        }
        if(rightBound == -1) {
          std::cout << "ERROR: No good right bound for downstream sector " << j+1 << std::endl;
        }
        fittedCharge = (maxCharges_sec[rightBound] - maxCharges_sec[leftBound])*(j - leftBound)/(rightBound - leftBound) + maxCharges_sec[leftBound];
        offsets_sec[j] = fittedCharge - maxCharges_sec[j];
      }
      else {
        leftBound = j;
      }
      cal_file << Form("JAN01BN%02i",j+1) << "\n";
      cal_file << "  " << "EnergyCoeff: " << offsets_sec[j] << " 1.0" << "\n";
    }

//downstream rings
    for(int j=0;j<24;j++) {
      
      GH1* h = (GH1*)h_r1->ProjectionY("",j+2,j+2);
      h->Rebin(20);

      int b1 = h->FindBin(ranges_downstream_rings.at(j)[0]);
      int b2 = h->FindBin(ranges_downstream_rings.at(j)[1]);
      
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

      maxCharges_ring[j] = x;
//Check if the current sector is a bad sector
      if(std::find(std::begin(badRings1), std::end(badRings1), j+1) == std::end(badRings1)) {
        offsets_ring[j] = 0;
      }
      else {
        offsets_ring[j] = -1;
      }

    }

//leftBound will store the last good charge value that we can use for the left bound of the interpolation
    leftBound = -1;
    rightBound = -1;
    for(int j=0;j<24;j++) {
      if(offsets_ring[j] == -1) {
        if(leftBound == -1) {
          std::cout << "ERROR: No good left bound for downstream ring " << j+1 << std::endl;
        }
        //here we find the right bound for the interpolation
        rightBound = -1;
        for(int k=j+1;k<24;k++) {
          if(offsets_ring[k] == 0 && rightBound == -1) {
            rightBound = k;
          }
        }
        if(rightBound == -1) {
          std::cout << "ERROR: No good right bound for downstream ring " << j+1 << std::endl;
        }
        fittedCharge = (maxCharges_ring[rightBound] - maxCharges_ring[leftBound])*(j - leftBound)/(rightBound - leftBound) + maxCharges_ring[leftBound];
        offsets_ring[j] = fittedCharge - maxCharges_ring[j];
      }
      else {
        leftBound = j;
      }
      cal_file << Form("JAN01FN%02i",j+1) << "\n";
      cal_file << "  " << "EnergyCoeff: " << offsets_ring[j] << " 1.0" << "\n";
    }

    cal_file.close();

    ref_file.clear();
    ref_file.seekg(0);

  }

  return;
}
