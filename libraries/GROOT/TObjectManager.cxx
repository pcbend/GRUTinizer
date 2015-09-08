#include "TObjectManager.h"

#include <iostream>

#include "TClass.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TFile.h"


TObjectManager* gManager = NULL;
TObjectManager* gBaseManager = NULL;
TList TObjectManager::objectmanagers;

TObjectManager* TObjectManager::Get(const char* name, Option_t* opt){
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  TObjectManager *test = (TObjectManager*)objectmanagers.FindObject(name);
  if(!test){
    test = TObjectManager::Open(name,opt);
  }
  if(test) {
    gManager = test;
    gDirectory = gManager;
  }

  return gManager;
}

TObjectManager::TObjectManager(const char* name, Option_t *opt)
  : TFile(name,opt){
  objectmanagers.SetOwner(false);
  //gROOT->GetListOfCleanups()->Add(this);
}

TObjectManager *TObjectManager::Open(const char *fname,Option_t *opt) {
  std::cout << __PRETTY_FUNCTION__ << "\t" << std::endl;
  gROOT->GetListOfFiles()->Print();

  std::string strname = fname;
  std::string stropt  = opt;
  bool is_base = false;
  if(!strcmp(fname,"GRUT_Manager")) { //default file name, recreate.
    strname  = "current.root";
    stropt   = "recreate";
    is_base = true;
  }

  TObjectManager *current = new TObjectManager(strname.c_str(),stropt.c_str());
  objectmanagers.Add(current);
  gManager = current;
  gDirectory = current;
  if(is_base){
    gBaseManager = current;
  }
}

void TObjectManager::SaveAndClose(Option_t* option){
  std::cout << __PRETTY_FUNCTION__ << ", " << fList->GetSize() << " items known" << std::endl;
  int num_objects = fList->GetSize();


  TString options = this->GetOption();
  if(options.Contains("CREATE")) {
    TObjectManager *current = gManager;
    TFile::cd("/");
    ParentChildMap::iterator it;
    for(it=fParentChildren.begin();it!=fParentChildren.end();it++) {
       TFile::cd("/");
       printf("it->first = 0x%08x\n",it->first);
       printf("getname() = %s\n",it->first->GetName());
       it->first->Write();
       if(it->second.size()){
         TFile::mkdir(it->first->GetName());
         TFile::cd(it->first->GetName());
         for(int x=0;x<it->second.size();x++) {
           it->second.at(x)->Write();
         }
       }
    }
    gManager   = current;
    gDirectory = gManager;
  }

  TFile::Close(option);

  if(!strcmp(GetName(),"current.root")){
    std::string name = "last.root";
    while(objectmanagers.FindObject(name.c_str())){
      name = "last_" + name;
    }
    if(num_objects > 0){
      gInterpreter->ProcessLine(Form(".!mv current.root %s",name.c_str()));
    } else {
      gInterpreter->ProcessLine(Form(".!rm current.root",name.c_str()));
    }
  }
}


TObjectManager *TObjectManager::cd() {
  gManager = this;
  TFile::cd();

  return this;
}

TObjectManager::~TObjectManager() {

}

void TObjectManager::SaveParent(TObject *parent) {
  printf("%s %s\n",__PRETTY_FUNCTION__,this->GetName());
  TString options = this->GetOption();
  if(options.Contains("CREATE")) {
    TObjectManager *current = gManager;

    if(fParentChildren.count(parent)){
      auto& children = fParentChildren[parent];

      TFile::cd("/");
      printf("getname = %s",parent->GetName());
      parent->Write();
      if(children.size()){
        TFile::mkdir(parent->GetName());
        TFile::cd(parent->GetName());
        for(auto child : children){
          child->Write();
        }
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


void TObjectManager::RecursiveRemove(TObject* obj) {
  std::cout << __PRETTY_FUNCTION__ << "\tObj: " << obj
            << "\tClass: " << obj->Class()->GetName()
            << std::endl;
  std::vector<TObject*> parents_to_remove;

  for(auto p_it = fParentChildren.rbegin(); p_it != fParentChildren.rend(); p_it++){
    if(obj == p_it->first){
      SaveParent(p_it->first);
      parents_to_remove.push_back(p_it->first);
    } else {
      for(auto c_it = p_it->second.end()-1; c_it != p_it->second.begin()-1; c_it--){
        if(*c_it == obj){
          p_it->second.erase(c_it);
        }
      }
    }
  }

  // Parents must be removed while not looping over the map.
  // Otherwise, the iterators become invalid.
  for(auto parent : parents_to_remove){
    fParentChildren.erase(parent);
  }


  TDirectory::RecursiveRemove(obj);
}
