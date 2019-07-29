{
  TUML *uml         = 0;
  TBank29 *gretina = 0;

  gChain->SetBranchAddress("TBank29",&gretina);
  gChain->SetBranchAddress("TUML",&uml);

  long ne = gChain->GetEntries();

  long last_gretina = -1;
  long last_umnl    = -1;

  long delta_gretina = -1;
  long delta_uml    = -1;


  for(int i=0;i<ne;i++) { 
      gChain->GetEntry(i);
      if(gretina->Timestamp()>0) {
         delta_gretina = gretina->Timestamp()-last_gretina;
         last_gretina  = gretina->Timestamp();
      }

      if(uml->Timestamp()>0) {
         delta_uml = uml->Timestamp()-last_uml;
         last_uml  = uml->Timestamp();
      }

      cout << i << "\t" << delta_gretina << "\t" << delta_uml << endl;

      if(i>50000) break;
  }

}












