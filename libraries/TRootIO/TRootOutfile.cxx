#include "TRootOutfile.h"

#include "Globals.h"

#include "TDetectorEnv.h"
#include "TGRUTOptions.h"
#include "TJanus.h"
#include "TSega.h"
#include "TNSCLEvent.h"
#include "TOnlineTree.h"

ClassImp(TRootOutfile)

TRootOutfile::TRootOutfile() {
  outfile     = NULL;
  preexisting_tree = NULL;
}

TRootOutfile::~TRootOutfile() {
  for(auto list : hist_list){
    delete list.second;
  }

  if(outfile){
    //FinalizeFile();
    CloseFile();
    //outfile->Close();
    //delete outfile;
  }

  if(TGRUTOptions::Get()->IsOnline()){
    for(auto& elem : trees){
      delete elem.second.tree;
    }
  }
}

void TRootOutfile::InitFile(const char* output_filename){
  bool is_online = TGRUTOptions::Get()->IsOnline();
  if(is_online){
    SetOutfile(NULL);
  } else {
    if(output_filename==NULL){
      output_filename = "my_output.root";
    }
    SetOutfile(output_filename);
  }

  Init();
}

void TRootOutfile::InitTree(TTree* tree) {
  preexisting_tree = tree;
  Init();
}

void TRootOutfile::AddTree(const char* tname,const char* ttitle,
                           bool build, int build_window,
                           int circular_size) {
  if(preexisting_tree){
    return;
  }

  bool is_online = TGRUTOptions::Get()->IsOnline();

  if(!outfile && !is_online) {
    fprintf(stderr,"%s, attempting to make tree with out associated file.\n",__PRETTY_FUNCTION__);
  }
  if(!ttitle)
    ttitle = tname;

  if(outfile){
    outfile->cd();
  }

  tree_element elem;

  if(is_online){
    elem.tree = new TOnlineTree(tname, ttitle, circular_size);
  } else {
    elem.tree = new TTree(tname,ttitle);
  }

  //elem.tree->SetMaxTreeSize(1000000000); //outfile limited to 1gb, than outfle_%i opened.
  elem.build_det = build;
  elem.build_window = build_window;
  elem.event_build_window_close = build_window;
  elem.has_data = false;
  trees[tname] = elem;
};

TTree *TRootOutfile::FindTree(const char *tname) {
  try{
    return trees.at(tname).tree;
  } catch (std::out_of_range& e) {
    return NULL;
  }
}

void TRootOutfile::AddRawData(const TRawEvent& event, kDetectorSystems det_type){
  try{
    auto det = det_list.at(det_type);
    det.det->AddRawData(event);
    det.tree_elem->has_data = true;
  } catch (std::out_of_range& e) {  }
}

void TRootOutfile::UpdateDetList(kDetectorSystems det_system, TDetector* detector, const char* tree_name){
  det_list_element new_det;
  new_det.det = detector;
  new_det.tree_elem = FindTreeElement(tree_name);
  det_list[det_system] = new_det;
}

void TRootOutfile::AddBranch(const char* treename, const char* branchname,
                             const char* classname, TDetector** obj,
                             kDetectorSystems det_system) {
  if(std::string(treename)=="EventTree" && preexisting_tree){
    preexisting_tree->SetBranchAddress(branchname, obj);
    compiled_histograms.RegisterDetector(*obj);
    return;
  }

  TTree* tree = FindTree(treename);

  if(!tree){
    std::cerr << "Could not find ttree: " << treename << std::endl;
    return;
  }

  tree->Branch(branchname, classname, obj);
  UpdateDetList(det_system, *obj, treename);
  compiled_histograms.RegisterDetector(*obj);
}

TRootOutfile::tree_element* TRootOutfile::FindTreeElement(const char* tname){
  try{
    return &trees.at(tname);
  } catch (std::out_of_range& e) {
    return NULL;
  }
}

void TRootOutfile::FillTree(const char *tname, long next_timestamp) {
  tree_element* elem = FindTreeElement(tname);
  if(!elem) {
    fprintf(stderr,"%s: trying to fill nonexisting tree %s\n.",__PRETTY_FUNCTION__,tname);
    return;
  }

  if(!elem->has_data){
    return;
  }

  // If there is no data added, don't fill
  // If there is a build window, don't fill unless the new timestamp is outside the window.
  // if(!elem->has_data ||
  //    (next_timestamp >= 0 &&
  //     elem->build_window >= 0 &&
  //     next_timestamp < elem->event_build_window_close)) {
  //   return;
  // }

  if(elem->build_det){
    for(auto& item : det_list) {
      item.second.det->Build();
    }
  }
  FillHistograms();
  elem->tree->Fill();
  Clear();
  elem->has_data = false;
  elem->event_build_window_close = next_timestamp + elem->build_window;
}


void TRootOutfile::FillAllTrees() {
  for(auto& val : trees){
    tree_element& elem = val.second;

    if(elem.has_data){
      if(elem.build_det){
	for(auto& item : det_list) {
	  item.second.det->Build();
	}
      }

      elem.event_build_window_close = elem.build_window;
      elem.tree->Fill();
    }
  }
}


void TRootOutfile::Clear(Option_t *opt) {
  for(auto& item : det_list) {
    item.second.det->Clear();
  }
}

void TRootOutfile::SetOutfile(const char* fname){
  if(!TGRUTOptions::Get()->IsOnline()){
    outfile = new TFile(fname,"recreate");
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
  for(auto& elem : trees){
    TTree* tree = elem.second.tree;
    printf("\t%s[%i] written...",tree->GetName(),tree->GetEntries());  fflush(stdout);
    TFile *f = tree->GetCurrentFile();
    outfile = f;
    f->cd();
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
    if(item.first.length()>0) {
      outfile->mkdir(item.first.c_str());
      outfile->cd(item.first.c_str());
    }
    item.second->Sort();
    item.second->Write();
    printf(" %i objects written.i\n",item.second->GetSize()); fflush(stdout);
    counter++;
  }
  printf("done. %i dirs written.\n",counter); fflush(stdout);
  //CloseFile();
}

void TRootOutfile::CloseFile(){
  if(outfile){
    if(trees.size()){
      TFile* curr_file = trees.begin()->second.tree->GetCurrentFile();
      curr_file->Close();
      curr_file->Delete();
    } else {
      outfile->Close();
      outfile->Delete();
    }
    outfile = NULL;
  }
}



void TRootOutfile::Print(Option_t* opt) const {
  return;
}

void TRootOutfile::LoadCompiledHistogramFile(const std::string& filename) {
  compiled_histograms.Load(filename);
}
