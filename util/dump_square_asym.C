void dump_square(char * rootfile = "master",char * hname = "gg1")
{
  char fname[500];
  sprintf(fname,"%s.root",rootfile);
  TFile *fin = new TFile(fname);
  // sprintf(fname,"gg1.sqr",hname);
  sprintf(fname,"%s_%s.sqr",rootfile,hname);
  cout << "File " << fname << endl;

  ofstream fout(fname,ios::out|ios::binary);

  TH2F *h;
  h = (TH2F *)fin->Get(hname);

  if(h) {

    cout << "Found spectrum " << hname << endl;
    int x = h->GetNbinsX();
    int y = h->GetNbinsY();
    float gbin;
    cout << "Array dimensions x=" << x << " y=" << y <<  endl;
    //root histograms start with bin 0, which has underflows (not interested)
    //and 'nbins' has the last bin of data with nbins+1 the overflow 
    //(so the histogram is an array nbins+2)
    //if( x == y ) {
      int bufsiz=sizeof(int)*3;
      int code=0;

      int maxxy=x;
      if (y>maxxy) maxxy=y;

      fout.write((char *)&bufsiz,sizeof(int));
      fout.write((char *)&maxxy,sizeof(int));
      fout.write((char *)&code,sizeof(int)); //unused
      fout.write((char *)&code,sizeof(int)); //type, 0 for square
      fout.write((char *)&bufsiz,sizeof(int));

      //      bufsiz=sizeof(float)*x*y;
      bufsiz=sizeof(float)*maxxy*maxxy;
      fout.write((char *)&bufsiz,sizeof(int));

      for(int i=1; i<=maxxy; i++){
	for(int j=1; j<=maxxy; j++){
	  gbin = h->GetBinContent(j,i);
	  fout.write((char *)&gbin,sizeof(float));
	}
      }
      fout.write((char *)&bufsiz,sizeof(int));
      /*} else {
      cout << "Not a square array x=" << x << " y=" << y <<  endl;
      }*/

  } else cout << "Failed to find spectrum" << endl;

  fout.close();
  fin->Close();
      
  return;
      
}
