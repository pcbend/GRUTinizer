#include "TObjectManager.h"

#include <iostream>

#include "TROOT.h"

TObjectManager* gManager = NULL;

void TObjectManager::Init(const char* name, const char* title){
  if(!gManager){
    gManager = new TObjectManager(name, title);
  }

  gDirectory = gManager;
}

TObjectManager::TObjectManager(const char* name, const char* title)
  : TDirectory(name, title, "", NULL){
  gROOT->GetListOfCleanups()->Add(this);
}

TObjectManager::~TObjectManager() { }

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
  for(auto p_it = fParentChildren.rbegin(); p_it != fParentChildren.rend(); p_it++){
    if(obj == p_it->first){
      fParentChildren.erase(p_it->first);
    } else {
      for(auto c_it = p_it->second.end()-1; c_it != p_it->second.begin()-1; c_it--){
        if(*c_it == obj){
          p_it->second.erase(c_it);
        }
      }
    }
  }

  TDirectory::RecursiveRemove(obj);
}
