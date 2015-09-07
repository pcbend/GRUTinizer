#include "TObjectManager.h"

#include <iostream>

#include "TROOT.h"
#include "TFile.h"


TObjectManager* gManager = NULL;
TList TObjectManager::objectmanagers;

void TObjectManager::Get(const char* name, Option_t* opt){

  TObjectManager *test = (TObjectManager*)objectmanagers.FindObject(name);  
  if(!test){
    //gManager = new TObjectManager(name, title);
    test = TObjectManager::Open(name,opt);
  }
  if(test) {
    gManager = test;
    gDirectory = gManager;
    //objectmanagers.Add(gManager);
  }
}

TObjectManager::TObjectManager(const char* name, Option_t *opt)
  : TFile(name,opt){
  objectmanagers.SetOwner(false);
  //gROOT->GetListOfCleanups()->Add(this);
}

TObjectManager *TObjectManager::Open(const char *fname,Option_t *opt) {
  std::string strname = fname;
  std::string stropt  = opt;
  if( "GRUT_Manager") { //default file name, recreate.
    strname = "LastSession.root";   
    stropt   = "recreate";
  }

  TObjectManager *current = new TObjectManager(strname.c_str(),stropt.c_str());
  //current = TFile::Open(fname,opt);  
  objectmanagers.Add(current);
  gManager = current;
  gDirectory = gManager;
}

TObjectManager *TObjectManager::cd() {
  gManager = this;
  this->cd();
}

TObjectManager::~TObjectManager() {

  printf("%s %s\n",__PRETTY_FUNCTION__,this->GetName());
  TString options = this->GetOption();
  if(options.Contains("CREATE")) {
    TObjectManager *current = gManager;
    //tfile::cd("/");
    ParentChildMap::iterator it;
    for(it=fParentChildren.begin();it!=fParentChildren.end();it++) {
       TFile::cd("/");
       printf("it->first = 0x%08x\n",it->first);
       printf("getname() = %s\n",it->first->GetName());
       it->first->Write();
       TFile::mkdir(it->first->GetName());
       TFile::cd(it->first->GetName());
       for(int x=0;x<it->second.size();x++) {
         it->second.at(x)->Write();
       }     
    }
    gManager   = current;
    gDirectory = gManager;
  }

}

void TObjectManager::SaveParent(TObject *parent) {
  printf("%s %s\n",__PRETTY_FUNCTION__,this->GetName());
  TString options = this->GetOption();
  if(options.Contains("CREATE")) {
    TObjectManager *current = gManager;
    ParentChildMap::iterator it;
    for(it=fParentChildren.begin();it!=fParentChildren.end();it++) {
      if(it->first==parent) { 
        TFile::cd("/");
        printf("getname = %s",it->first->GetName());
        it->first->Write();
        if(it->second.size()>0) {
          TFile::mkdir(it->first->GetName());
          TFile::cd(it->first->GetName());
          for(int x=0;x<it->second.size();x++) {
            it->second.at(x)->Write();
          }     
        }
        break;
      }
    }
    gManager   = current;
    gDirectory = gManager;
  }
}

void TObjectManager::Print(Option_t* opt) const {
  std::cout << "I am our custom object manager (" << fName << ", " << fTitle << ")" << std::endl;
  for(auto& element : fParentChildren){
    std::cout << "Parent: " << element.first->GetName() << " @ " << (void*)element.first << std::endl;
    for(auto& child : element.second){
      std::cout << "\t" << child->GetName() << " @ " << (void*)child << std::endl;
    }

    std::cout << "-----------------------" << std::endl;
  }
}

void TObjectManager::Add(TObject* obj, Bool_t replace){
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  TDirectory::Add(obj, replace);
}

void TObjectManager::Append(TObject* obj, Bool_t replace){
  if(replace){
    RecursiveRemove(obj);
  }

  TDirectory::Append(obj, replace);
  fParentChildren[obj];
  ObjectAppended(obj);
}

void TObjectManager::ObjectAppended(TObject* obj) {
  std::cout << __PRETTY_FUNCTION__ << "\t" << (void*)obj << std::endl;
  Emit("ObjectAppended(TObject*)",(long)obj);
}

void TObjectManager::AddRelationship(TObject* parent, TObject* child){
  if(parent && child){
    fParentChildren[parent].push_back(child);
  }
}

/*
void TObjectManager::RecursiveRemove(TObject* obj) {
  for(auto p_it = fParentChildren.rbegin(); p_it != fParentChildren.rend(); p_it++){
    if(obj == p_it->first){
      SaveParent(p_it->first);
      //fParentChildren.erase(p_it->first);
    } else {
      for(auto c_it = p_it->second.end()-1; c_it != p_it->second.begin()-1; c_it--){
        if(*c_it == obj){
          //p_it->second.erase(c_it);
        }
      }
    }
  }

  TDirectory::RecursiveRemove(obj);
}
*/
