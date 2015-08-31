#include "TRootOutfile.h"

#include "Globals.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TJanus.h"
#include "TSega.h"
#include "TNSCLEvent.h"

TRootOutfile::TRootOutfile() {
  outfile     = NULL;
}

TRootOutfile::~TRootOutfile() {
  if(outfile){
    FinalizeFile();
    CloseFile();
    //outfile->Close();
    //delete outfile;
  }
}

TTree *TRootOutfile::AddTree(const char* tname) {
  if(!outfile) {
    fprintf(stderr,"%s, attempting to make tree with out associated file.\n",__PRETTY_FUNCTION__);
  }
  outfile->cd();
  TTree *t = new TTree(tname,tname);
  tree_list.Add(t);
  return t;
};

TTree *TRootOutfile::FindTree(const char *tname) {
  TTree *t = (TTree*)tree_list.FindObject(tname);
  return t;
}

void TRootOutfile::AddRawData(const TRawEvent& event, kDetectorSystems det_type){
  try{
    TDetector* det = det_list.at(det_type);
    det->AddRawData(event);
  } catch (std::out_of_range& e) { }
}

void TRootOutfile::FillTree() {
  TIter iter(&tree_list);
  
  for(auto& item : det_list) {
    item.second->Build();
  }
  while(TTree *tree = (TTree*)iter.Next()) {
    tree->Fill();
  }
}

void TRootOutfile::Clear(Option_t *opt) {
  for(auto& item : det_list) {
    item.second->Clear();
  }
}

void TRootOutfile::FinalizeFile(){
  if(outfile){
    outfile->cd();
  } else {
    return;
  }

  int counter =0;
  printf("Writing trees.\n"); fflush(stdout);
  TIter treeiter(&tree_list);
  while(TTree *tree = (TTree*)treeiter.Next()) {
    printf("\t%s[%i] written...",tree->GetName(),tree->GetEntries());  fflush(stdout);
    tree->Write();
    printf(" done.\n"); fflush(stdout);
    counter++;
  }
  printf("done. %i trees written.\n",counter); fflush(stdout);
  counter=0;
  printf("Writing hists...\n"); fflush(stdout);
  for(auto& item:hist_list) {
    printf("\t%s:%s...",outfile->GetName(),item.first.c_str()); fflush(stdout);
    outfile->cd();
    outfile->mkdir(item.first.c_str());
    outfile->cd(item.first.c_str());
    item.second.Sort();
    item.second.Write(); 
    printf(" %i objects written.i\n",item.second.GetSize()); fflush(stdout);
    counter++;
  }
  printf("done. %i dirs written.\n",counter); fflush(stdout);
  CloseFile();
}

void TRootOutfile::CloseFile(){
  outfile->Close();
  outfile->Delete();
  outfile = NULL;
}

void TRootOutfile::Print(Option_t* opt) const {

  return;
}


