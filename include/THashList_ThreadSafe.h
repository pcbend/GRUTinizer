#ifndef THASHLIST_THREADSAFE_H
#define THASHLIST_THREADSAFE_H

#ifndef __CINT__
#include <mutex>
#endif

#include "THashList.h"

class THashList_ThreadSafe : public THashList {
public:
  THashList_ThreadSafe(Int_t capacity=TCollection::kInitHashTableCapacity, Int_t rehash=0)
    : THashList(capacity, rehash) { }
  THashList_ThreadSafe(TObject *parent, Int_t capacity=TCollection::kInitHashTableCapacity, Int_t rehash=0)
    : THashList(parent, capacity, rehash) { }

  // All the virtual functions from THashList.
  void       AddFirst(TObject *obj);
  void       AddFirst(TObject *obj, Option_t *opt);
  void       AddLast(TObject *obj);
  void       AddLast(TObject *obj, Option_t *opt);
  void       AddAt(TObject *obj, Int_t idx);
  void       AddAfter(const TObject *after, TObject *obj);
  void       AddAfter(TObjLink *after, TObject *obj);
  void       AddBefore(const TObject *before, TObject *obj);
  void       AddBefore(TObjLink *before, TObject *obj);
  void       RecursiveRemove(TObject *obj);
  void       Rehash(Int_t newCapacity);
  TObject*   Remove(TObject *obj);
  TObject*   Remove(TObjLink *lnk);

private:
#ifndef __CINT__
  std::mutex mutex;
#endif

  ClassDef(THashList_ThreadSafe, 1);
};

#endif /* THASHLIST_THREADSAFE_H */
