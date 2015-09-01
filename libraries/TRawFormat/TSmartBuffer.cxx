#include "TSmartBuffer.h"
#include "TString.h"
#include "TRegexp.h"
#include "Globals.h"

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

void TSmartBuffer::Advance(size_t dist){
  dist = std::min(dist,fSize);
  fData += dist;
  fSize -= dist;
}

void TSmartBuffer::Print(Option_t* opt) const {
  TString options(opt);
  TRegexp regexp("0x[0-9a-f][0-9a-f][0-9a-f][0-9a-f]");
  if(!options.Contains("bodyonly")) {
    std::cout << "TSmartBuffer allocated at " << (void*)fAllocatedLocation << ", "
              << "currently pointed at " << (void*)fData << ", "
              << "with a size of " << fSize << "bytes."
              << std::endl;
  }
  if(fReferenceMutex){
    std::unique_lock<std::mutex> lock(*fReferenceMutex);
    std::cout << "There are " << *fReferenceCount << " TSmartBuffers sharing this C-buffer"
              << std::endl;

    // Full hexdump of the buffer contents
    TString highlight_string = options(regexp);   
    unsigned short highlight = 0;
    if(highlight_string.Length()) {
      highlight = strtol(highlight_string.Data(),0,0);
    }
    if(options.Contains("all")){
      printf("\t");
      for(int x=0; x<GetSize()-1; x+=2) {
        if((x%16 == 0) &&
           (x!=GetSize())){
          printf("\n\t");
        }
        unsigned int value = *(unsigned short*)(GetData()+x);
        if(highlight>0 && highlight==value) {printf(DRED);}
        printf("0x%04x  ",value);
        if(highlight>0 && highlight==value) {printf(RESET_COLOR);}
        
      }
      if(GetSize()%2 == 1){
        printf("0x02x ", *(unsigned char*)(GetData()+GetSize()-1));
      }

      printf("\n--------------------------\n");
    }
  }
}
