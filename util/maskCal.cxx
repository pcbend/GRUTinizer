#include <iostream>
#include <iomanip>
#include "TH1.h"
#include "TS800.h"
#include "TFile.h"
#include "TChain.h"
#include "TH2.h"
using namespace std;
void sort(char const * infile, int maskN, char const * outfile) {

  TFile * inputfile = new TFile(infile, "READ");
  if (!inputfile->IsOpen()) {
    printf("Opening file failed, aborting\n");
    return;
  }
  TChain * EventTree = (TChain * ) inputfile->Get("EventTree");
  printf("%i tree files, details:\n", EventTree->GetNtrees());
  EventTree->ls();
  Int_t nentries = EventTree->GetEntries();

  TS800 *s800 = 0;
  TS800Hit s800_hit;
  if (EventTree->FindBranch("TS800")) {
    EventTree->SetBranchAddress("TS800", & s800);
  } else {
    cout << "No TS800 Branch Found" << endl;
    return;
  }
  TList *list = new TList;
  TH2F *CRDCXY = new TH2F("CRDCXY", Form("CRDCXY%i", maskN), 600, -300, 300, 5000, -5000, 5000); list->Add(CRDCXY);

  for (int jentry = 0; jentry < nentries; jentry++) {
    EventTree->GetEntry(jentry);
    if(s800) {
      float xfp = s800->GetCrdc(maskN-1).GetDispersiveX()*2.54 - 281.94;
      float yfp = s800->GetCrdc(maskN-1).GetNonDispersiveY();
      CRDCXY->Fill(xfp, yfp);
    }
    if (jentry % 10000 == 0)cout << setiosflags(ios::fixed) << "Entry " << jentry << " of " << nentries << ", " << 100 * jentry / nentries << "% complete" << "\r" << flush;
  }
  TFile * myfile = new TFile(outfile, "RECREATE");
  myfile->cd();
  list->Write();
  myfile->Close();
}
int main(int argc, char ** argv) {

  char const * efile = "";
  char const * outfile = "";
  int maskN = 1;
  printf("Starting sortcode\n");
  // Input-chain-file, output-histogram-file
  if (argc < 2) {
    cout << "Insufficient arguments, provide event tree file and mask number" << endl;
    return 0;
  } else if (argc == 3) {
    efile = argv[1];
    maskN = stoi(argv[2]);
    outfile = "maskcal.root";
  } else if (argc == 4) {
    efile = argv[1];
    maskN = stoi(argv[2]);
    outfile = argv[3];
  } else if (argc > 4) {
    printf("Too many arguments\n");
    return 0;
  }

  printf("Input file:%s\nCalibrate Mask: %i:Output file: %s\n", efile, maskN, outfile);
  sort(efile, maskN, outfile);
  return 0;
}
