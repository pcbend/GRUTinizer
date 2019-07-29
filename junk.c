

{
  //TBank88 *bank = 0;
  TUML    *uml  = 0;  

  //gChain->SetBranchAddress("TBank88",&bank);
  gChain->SetBranchAddress("TUML",&uml);

  //ofstream ofile("bank88.txt");
  ofstream ofile("uml.txt");
  long x=0;
  for(int i=0;i<5000;) {
    gChain->GetEntry(x++);
    //if(bank->Timestamp()>0 && (fabs(bank->GetMode3Hit(0).Charge()*0.261594-1332)<20)) {
    if(uml->Timestamp()>0  && (fabs(uml->GetUMLHit(0).Charge()*0.384744-1332)<20)) {
      //ofile << bank->Timestamp();
      ofile << uml->Timestamp();
      ofile << "\t";
      //ofile << bank->GetMode3Hit(0).Charge()*0.261594;
      ofile << uml->GetUMLHit(0).Charge()*0.384744;
      ofile << endl;
      i++;
    }
  }  




}

////////// run 120 //////////////////

uml gap1
23951640928     28035802208

bank88 gap1
24218840951     28303002231


uml gap2
38304475016    41217381872

bank88 gap2
38571675037    41484581895 

/////////////////////////////////////
//
//
//
/////////// run 121 /////////////////

uml gap1 
12938549376      22067382816

bank88 gap1
13211744181      22340577625



uml gap2
26126774920     28143513568

bank88 gap2
26399969729     28416708369



uml gap3 
35311576616    37819705584

bank88 gap3
35584771419    38092900387

////////////// run 131 //////////////

uml gap1 
20928068992   22915804896

bank88 gap1                              269656835
21197725827   23185461729


////////////// run 132 //////////////

uml gap1 
13731945544    15462447144

bank88 gap1                              
13994865491    15896862547
13994865491    15896862547



14716318000 
