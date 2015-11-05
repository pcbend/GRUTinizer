#include "THashList_ThreadSafe.h"


void THashList_ThreadSafe::AddFirst(TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddFirst(obj);
}

void THashList_ThreadSafe::AddFirst(TObject *obj, Option_t *opt) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddFirst(obj,opt);
}
void THashList_ThreadSafe::AddLast(TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddLast(obj);
}
void THashList_ThreadSafe::AddLast(TObject *obj, Option_t *opt) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddLast(obj, opt);
}
void THashList_ThreadSafe::AddAt(TObject *obj, Int_t idx) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddAt(obj, idx);
}
void THashList_ThreadSafe::AddAfter(const TObject *after, TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddAfter(after, obj);
}
void THashList_ThreadSafe::AddAfter(TObjLink *after, TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddAfter(after, obj);
}
void THashList_ThreadSafe::AddBefore(const TObject *before, TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddBefore(before, obj);
}
void THashList_ThreadSafe::AddBefore(TObjLink *before, TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::AddBefore(before, obj);
}
void THashList_ThreadSafe::RecursiveRemove(TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::RecursiveRemove(obj);
}
void THashList_ThreadSafe::Rehash(Int_t newCapacity) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::Rehash(newCapacity);
}
TObject* THashList_ThreadSafe::Remove(TObject *obj) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::Remove(obj);
}
TObject* THashList_ThreadSafe::Remove(TObjLink *lnk) {
  std::lock_guard<std::recursive_mutex> lock(mutex);
  THashList::Remove(lnk);
}
