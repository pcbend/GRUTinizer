#include "TSmartBuffer.h"

#include <algorithm>
#include <iostream>

TSmartBuffer::TSmartBuffer()
  : fAllocatedLocation(NULL), fData(NULL), fSize(0),
    fReferenceCount(NULL), fReferenceMutex(NULL) { }

TSmartBuffer::TSmartBuffer(char* buffer, size_t size)
  : fAllocatedLocation(buffer), fData(buffer), fSize(size) {
  fReferenceCount = new int(1);
  fReferenceMutex = new std::mutex;
}

TSmartBuffer::~TSmartBuffer() {
  // This buffer didn't point to anything, so no need to clean anything up.
  if(!fReferenceMutex){
    return;
  }

  std::unique_lock<std::mutex> lock(*fReferenceMutex);
  (*fReferenceCount)--;

  if(*fReferenceCount==0){

    free(fAllocatedLocation);
    delete fReferenceCount;
    lock.unlock();
    delete fReferenceMutex;
  }
}

TSmartBuffer::TSmartBuffer(const TSmartBuffer& other)
  : fAllocatedLocation(other.fAllocatedLocation), fData(other.fData),
    fSize(other.fSize), fReferenceCount(other.fReferenceCount),
    fReferenceMutex(other.fReferenceMutex) {


  if(fReferenceMutex){
    std::unique_lock<std::mutex> lock(*fReferenceMutex);
    (*fReferenceCount)++;
  }
}


TSmartBuffer& TSmartBuffer::operator=(TSmartBuffer other) {
  swap(other);
  return *this;
}

TSmartBuffer::TSmartBuffer(TSmartBuffer&& other)
  : TSmartBuffer() {
  swap(other);
}

void TSmartBuffer::Clear(){
  *this = TSmartBuffer();
}

void TSmartBuffer::swap(TSmartBuffer& other){
  std::swap(fAllocatedLocation, other.fAllocatedLocation);
  std::swap(fData, other.fData);
  std::swap(fSize, other.fSize);
  std::swap(fReferenceCount, other.fReferenceCount);
  std::swap(fReferenceMutex, other.fReferenceMutex);
}

void TSmartBuffer::SetBuffer(char* buffer, size_t size){
  TSmartBuffer temp(buffer, size);
  swap(temp);
}

TSmartBuffer TSmartBuffer::BufferSubset(size_t pos, size_t length) const {
  TSmartBuffer output = *this;

  if(pos > fSize){
    output.fData += fSize;
    output.fSize = 0;
  } else {
    output.fData += pos;
    output.fSize = std::min(fSize - pos, length);
  }

  return output;
}

void TSmartBuffer::Advance(size_t dist) {
  dist = std::min(dist, fSize);
  fData += dist;
  fSize -= dist;
}

void TSmartBuffer::Print(Option_t* opt) const {
  std::cout << "TSmartBuffer allocated at " << (void*)fAllocatedLocation << ", "
            << "currently pointed at " << (void*)fData << ", "
            << "with a size of " << fSize << "bytes."
            << std::endl;

  if(fReferenceMutex){
    std::unique_lock<std::mutex> lock(*fReferenceMutex);
    std::cout << "There are " << *fReferenceCount << " TSmartBuffers sharing this C-buffer"
              << std::endl;

    // Full hexdump of the buffer contents
    if(!strcmp(opt,"all")){
      printf("\t");
      for(int x=0; x<GetSize()-1; x+=2) {
        if((x%16 == 0) &&
           (x!=GetSize())){
          printf("\n\t");
        }
        printf("0x%04x  ",*(unsigned short*)(GetData()+x));
      }
      if(GetSize()%2 == 1){
        printf("0x02x ", *(unsigned char*)(GetData()+GetSize()-1));
      }

      printf("\n--------------------------\n");
    }
  }
}
