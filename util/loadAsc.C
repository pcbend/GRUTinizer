
// Load 8k 0.5 keV/channel spectrum from an ASCII file
//   (counts in one channel per line, no header)
GH1D *loadSpec(TString fileName){

  TString specName = fileName.Copy();
  specName.Resize(specName.Length() - 4);
  specName = specName(4,specName.Length());
  specName.ReplaceAll(".","_"); // ROOT doesn't like '.' in object names.
  
  GH1D* spec = new GH1D(specName, specName, 8192, 0, 4096.);
 
  ifstream fp;
  fp.open(fileName);

  Int_t counts;
  Int_t channel = 1;
  while(fp >> counts){
    spec->SetBinContent(channel, counts);
    channel++;
  }
  
  return spec;

}

void loadAsc(TString rootFileName){

  TFile f1(rootFileName, "RECREATE");
  
  GH1D *spec[1000];
  
  TSystemDirectory dir(".", ".");

  TList *files = dir.GetListOfFiles();
  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    int i = 0;
    while ((file=(TSystemFile*)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(".asc")) {
	//	cout << fname.Data() << endl;
	spec[i] = loadSpec(fname);
	spec[i]->Write();
	i++;
      }
    }
  }

  return;

}
