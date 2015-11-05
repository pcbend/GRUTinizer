
#define __ROOTMACRO__ 1

#include <cstdio>
#include <string>
#include <map>

//#include <TObjectManager.h>
#include <TMode3.h>

#include <TList.h>
#include <TChain.h>
#include <TH2.h>
#include <TFile.h>

#ifndef GLOBALS_H
const char* RESET_COLOR = "\033[m";
const char* BLUE       = "\033[1;34m";
const char* YELLOW     = "\033[1;33m";
const char* GREEN      = "\033[1;32m";
const char* RED        = "\033[1;31m";
const char* BLACK      = "\033[1;30m";
const char* MAGENTA    = "\033[1;35m";
const char* CYAN       = "\033[1;36m";
const char* WHITE      = "\033[1;37m";

const char* DBLUE      = "\033[0;34m";
const char* DYELLOW    = "\033[0;33m";
const char* DGREEN     = "\033[0;32m";
const char* DRED       = "\033[0;31m";
const char* DBLACK     = "\033[0;30m";
const char* DMAGENTA   = "\033[0;35m";
const char* DCYAN      = "\033[0;36m";
const char* DWHITE     = "\033[0;37m";

const char* BG_WHITE   = "\033[47m";
const char* BG_RED     = "\033[41m";
const char* BG_GREEN   = "\033[42m";
const char* BG_YELLOW  = "\033[43m";
const char* BG_BLUE    = "\033[44m";
const char* BG_MAGENTA = "\033[45m";
const char* BG_CYAN    = "\033[46m";
#endif

#define Q1 15
#define Q2 7
#define Q3 8
#define Q4 16
#define Q5 9
#define Q6 14
#define Q7 17
#define Q8 6

#define INTEGRATION 128.0

std::map<int,int> HoleQMap;

void InitMap() {
  HoleQMap[Q1] = 1;
  HoleQMap[Q2] = 2;
  HoleQMap[Q3] = 3;
  HoleQMap[Q4] = 4;
  HoleQMap[Q5] = 5;
  HoleQMap[Q6] = 6;
  HoleQMap[Q7] = 7;
  HoleQMap[Q8] = 8;
}




TList *Mode3Hists(TChain *data,bool status=true) {
  InitMap();
  
  TList *list = new TList;
  //TTree *data = (TTree*)gDirectory->Get("Data");
  data->SetParallelUnzip(true);
  TMode3 *m3=0;
  data->SetBranchAddress("TMode3",&m3);

  Long_t nentries = data->GetEntries();
  Long_t i;
  for(i=0;i<nentries;i++) {
    data->GetEntry(i);
  
    //if(i>1000000)
    //  break;
    //int hole = m3->GetHole();
    //

    for(int j=0;j<m3->Size();j++) {
      const TMode3Hit &hit = m3->GetMode3Hit(j);
      int que = HoleQMap[hit.GetHole()]; 
      std::string histname = Form("Q%iEgamRaw",que);
      TH2 *hist = (TH2*)list->FindObject(histname.c_str());
      if(!hist) {
        hist = new TH2F(histname.c_str(),histname.c_str(),160,0,160,
                                                          32000,0,32000);
        list->Add(hist);
      }
      hist->Fill(hit.GetCrystal()*40 + hit.GetSegmentId(),(double)hit.Charge()/INTEGRATION);
    }
    if(status && (i%1000)==0) {
      std::string out = Form("looping over %s%s%s: on entry %s%lu%s/%s%lu%s         \r", 
                              BLUE,data->GetCurrentFile()->GetName(),
                              RESET_COLOR,DYELLOW,i,
                              BLUE,DGREEN,nentries,RESET_COLOR);
      printf(out.c_str()); fflush(stdout);
    }
   
  }
  if(status) {
    std::string out = Form("looping over %s%s%s: on entry %s%lu%s/%s%lu%s         \n", 
                      BLUE,data->GetCurrentFile()->GetName(),
                      RESET_COLOR,DYELLOW,i,
                      BLUE,DGREEN,nentries,RESET_COLOR);
    printf(out.c_str()); fflush(stdout);

  }
  return list;
}

#ifndef __CINT__

int main(int argc, char **argv) {

  TChain *chain = new TChain("EventTree");
  for(int x=1;x<argc;x++)
    chain->Add(argv[x]);

  TList *list = Mode3Hists(chain);
  list->Sort();
  TFile f("hist.root","recreate");
  list->Write();
  f.Close();

}

#endif
