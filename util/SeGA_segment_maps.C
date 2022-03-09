void SeGA_segment_maps() {
  TSega sega;
  
  std::vector<std::array<double,2>> th_phi;
  std::map<std::array<double,2>,std::array<int,2>> map;
  
  //std::cout << "Segment Coordinates:" << std::endl;
  for(int det=1;det<17;det++) {
    for(int seg=1;seg<33;seg++) {
      
      double theta = sega.GetSegmentPosition(det,seg).Theta()*TMath::RadToDeg();
      double phi = sega.GetSegmentPosition(det,seg).Phi();
      if(phi<0)
	{phi+=TMath::TwoPi();}
      phi*=TMath::RadToDeg();
      
      map[{theta,phi}] = {det,seg};
      th_phi.push_back({theta,phi});

      //std::cout << "Det " << det << ", Seg " << seg << ": (Theta,Phi) = (" << th_phi.at(th_phi.size()-1)[0] << ","
      //<< th_phi.at(th_phi.size()-1)[1] << ")" << std::endl;
    }
   }

  //sort thetas
  std::sort(th_phi.begin(),th_phi.end(),
	    [](const std::array<double,2>& a, const std::array<double,2>& b){ return a[0] < b[0]; }
	    );

  std::vector<std::array<double,2>> sort_th_phi;

  //32 unique theta values
  for(int i=0;i<32;i++) {
    std::vector<std::array<double,2>> tmp;

    //16 unique phi values
    for(int j=0;j<16;j++) {
      tmp.push_back(th_phi.at(i*16 + j));
    }

    //sort phis
    std::sort(tmp.begin(),tmp.end(),
  	      [](const std::array<double,2>& a, const std::array<double,2>& b){ return a[1] < b[1]; }
    	      );

    //theta-phi sorted coordintates
    for(int k=0;k<16;k++) {
      sort_th_phi.push_back(tmp.at(k));
    }
  }

  
  //std::cout << "\nTheta-Phi Ordered Coordinates:" << std::endl;
  //for(auto coord : sort_th_phi) {
    //std::cout << "(Theta,Phi) = (" << coord[0] << "," << coord[1] << ")" << std::endl;
  //}
  

  std::ofstream f("segment_map.txt",std::ios::app);
  std::ofstream f2("pair_map.txt",std::ios::app);
  std::ofstream f4("slice_map.txt",std::ios::app);
  std::map<std::array<int,2>,int> my_map;
  
  //std::cout << "\nTheta-Phi Ordered Segments:" << std::endl;
  int i=1;
  for(auto coord : sort_th_phi) {
    my_map[{map[coord][0],map[coord][1]}] = i;

    f << "{{" << map[coord][0] << "," << map[coord][1] << "}," << i << "}\n";
    
    //std::cout << "(Det " << map[coord][0] << ", Seg " << map[coord][1] << ") Theta_Phi Position "<< i 
    //<< ": (Theta,Phi) = (" << coord[0] << "," << coord[1] << ")" << std::endl;
    i++;
  }

  double num=0;
  std::map<std::array<int,2>,int> seg2_map;
  //std::cout << "\nTheta-Phi Ordered Segments Pairs:" << std::endl;
  for(int j=0;j<sort_th_phi.size();j++) {
    if(j%2 == 0) {
      seg2_map[{map[sort_th_phi.at(j)][0],map[sort_th_phi.at(j)][1]}] = j+1 - num;
      f2 << "{{" << map[sort_th_phi.at(j)][0] << "," <<  map[sort_th_phi.at(j)][1]<< "}," << j+1 - num << "}\n";
    }
    else if(j%2 == 1) {
      seg2_map[{map[sort_th_phi.at(j)][0],map[sort_th_phi.at(j)][1]}] = j - num;
      f2 << "{{" << map[sort_th_phi.at(j)][0] << "," <<  map[sort_th_phi.at(j)][1]<< "}," << j - num << "}\n";
      num++;
    }
    else
      {std::cout << "j neither even nor odd" << std::endl;}

    //std::cout << "(Det " << map[sort_th_phi.at(j)][0] << ", Seg " << map[sort_th_phi.at(j)][1] 
    //<< ") Theta_Phi Pair " << seg2_map[{map[sort_th_phi.at(j)][0],map[sort_th_phi.at(j)][1]}] 
    //<< ": (Theta,Phi) = ("<< sort_th_phi.at(j)[0] << "," << sort_th_phi.at(j)[1] << ")" << std::endl;
  }

  std::map<std::array<int,2>,int> slice_map;
  int slice = 0;
  int mapnum = 1;

  //std::cout << "\nTheta-Phi Ordered Slices:" << std::endl;
  //Downstream Half
  while(slice < 8) {
    for(int det=10;det>8;det--) {
      for(int seg=0;seg<4;seg++) {
	//double theta = sega.GetSegmentPosition(det,32 - 4*slice - seg).Theta()*TMath::RadToDeg();
	//double phi = sega.GetSegmentPosition(det,32 - 4*slice - seg).Phi();
        //if(phi<0)
	  //{phi+=TMath::TwoPi();}
        //phi*=TMath::RadToDeg();
        slice_map[{det,32 - 4*slice - seg}] = mapnum;
	//std::cout << "(Det " << det << ", Seg " << 32 - 4*slice - seg << ") Theta-Phi Slice " << mapnum
	//	  << ": (Theta,Phi) = (" << theta << "," << phi << ")" << std::endl;
	f4 << "{{" << det << "," << 32 - 4*slice - seg << "}," << mapnum << "}\n";
      }
      mapnum++;
    }

    for(int det=16;det>10;det--) {
      for(int seg=0;seg<4;seg++) {
	//double theta = sega.GetSegmentPosition(det,32 - 4*slice - seg).Theta()*TMath::RadToDeg();
	//double phi = sega.GetSegmentPosition(det,32 - 4*slice - seg).Phi();
        //if(phi<0)
	  //{phi+=TMath::TwoPi();}
        //phi*=TMath::RadToDeg();
        slice_map[{det,32 - 4*slice - seg}] = mapnum;
	//std::cout << "(Det " << det << ", Seg " << 32 - 4*slice - seg << ") Theta-Phi Slice " << mapnum
	//	  << ": (Theta,Phi) = (" << theta << "," << phi << ")" << std::endl;
	f4 << "{{" << det << "," << 32 - 4*slice - seg << "}," << mapnum << "}\n";
      }
      mapnum++;
    }

    slice++;
  }

  //Upstream
  slice = 0;
  while(slice < 8) {
    for(int det=7;det<9;det++) {
      for(int seg=1;seg<5;seg++) {
	//double theta = sega.GetSegmentPosition(det,seg + 4*slice).Theta()*TMath::RadToDeg();
	//double phi = sega.GetSegmentPosition(det,seg + 4*slice).Phi();
        //if(phi<0)
	  //{phi+=TMath::TwoPi();}
        //phi*=TMath::RadToDeg();
	slice_map[{det,seg + 4*slice}] = mapnum;
        //std::cout << "(Det " << det << ", Seg " << seg + 4*slice << ") Theta-Phi Slice " << mapnum
	//	  << ": (Theta,Phi) = (" << theta << "," << phi << ")" << std::endl;
	f4 << "{{" << det << "," << seg + 4*slice << "}," << mapnum << "}\n";
      }
      mapnum++;
    }

    for(int det=1;det<7;det++) {
      for(int seg=1;seg<5;seg++) {
	//double theta = sega.GetSegmentPosition(det,seg + 4*slice).Theta()*TMath::RadToDeg();
	//double phi = sega.GetSegmentPosition(det,seg + 4*slice).Phi();
        //if(phi<0)
	  //{phi+=TMath::TwoPi();}
        //phi*=TMath::RadToDeg();
	slice_map[{det,seg + 4*slice}] = mapnum;
	//std::cout << "(Det " << det << ", Seg " << seg + 4*slice << ") Theta-Phi Slice " << mapnum
	//	  << ": (Theta,Phi) = (" << theta << "," << phi << ")" << std::endl;
	f4 << "{{" << det << "," << seg + 4*slice << "}," << mapnum << "}\n";
      }
      mapnum++;
    }
    slice++;   
  }
    
  f.close();
  f2.close();
  f4.close();
}
