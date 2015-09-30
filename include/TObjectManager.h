#ifndef _TOBJECTMANAGER_H_
#define _TOBJECTMANAGER_H_

#include <map>
#include <vector>

#include "TQObject.h"
#include "TDirectory.h"
#include "TFile.h"

class TH1;

//class TObjectManager : public TDirectory, public TQObject {
class TObjectManager : public TFile, public TQObject {
public:
  static TObjectManager* Get(const char* name, Option_t *opt="update");
  virtual ~TObjectManager();

  virtual void Add(TObject* obj, Bool_t replace = kFALSE);
  virtual void Append(TObject* obj, Bool_t replace = kFALSE);
  virtual void SaveAndClose(Option_t* option = "");

  virtual void Print(Option_t* opt = "") const;

  void ObjectAppended(TObject* obj); //*SIGNAL*

  void AddRelationship(TObject* parent, TObject* child);

  virtual void RecursiveRemove(TObject* obj);

  static TObjectManager *Open(const char *fname,Option_t *opt="read");
  static TList *GetListOfManagers() { return &objectmanagers; }
  TObjectManager *cd();

  TH1* GetNext1D(TH1* from, bool forward);

  bool Trackable(TObject* obj);

private:
  static TList objectmanagers;
  TObjectManager(const char* name, const char* title);

  typedef std::map<TObject*,std::vector<TObject*> > ParentChildMap;
  ParentChildMap fParentChildren;
  void SaveParent(TObject*);


  ClassDef(TObjectManager, 0);
};

extern TObjectManager* gManager;
extern TObjectManager* gBaseManager;

#endif /* _TOBJECTMANAGER_H_ */
