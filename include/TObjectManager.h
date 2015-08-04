#ifndef _TOBJECTMANAGER_H_
#define _TOBJECTMANAGER_H_

#include <map>
#include <vector>

#include "TQObject.h"
#include "TDirectory.h"

class TObjectManager : public TDirectory, public TQObject {
public:
  static void Init(const char* name, const char* title);
  virtual ~TObjectManager();

  virtual void Add(TObject* obj, Bool_t replace = kFALSE);
  virtual void Append(TObject* obj, Bool_t replace = kFALSE);

  virtual void Print(Option_t* opt = "") const;

  void ObjectAppended(TObject* obj); //*SIGNAL*

  void AddRelationship(TObject* parent, TObject* child);

  virtual void RecursiveRemove(TObject* obj);

private:
  TObjectManager(const char* name, const char* title);

  typedef std::map<TObject*,std::vector<TObject*> > ParentChildMap;
  ParentChildMap fParentChildren;



  ClassDef(TObjectManager, 0);
};

extern TObjectManager* gManager;

#endif /* _TOBJECTMANAGER_H_ */
