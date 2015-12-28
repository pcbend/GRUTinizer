#include "TSmartBuffer.h"
#include "TString.h"
#include "TRegexp.h"
#include "Globals.h"

#include <algorithm>
#include <iostream>

TSmartBuffer::TSmartBuffer()
  : fAllocatedLocation(NULL), fData(NULL), fSize(0),
    fReferenceCount(NULL) { }

TSmartBuffer::TSmartBuffer(char* buffer, size_t size)
  : fAllocatedLocation(buffer), fData(buffer), fSize(size) {
  fReferenceCount = new std::atomic_int(1);
}

TSmartBuffer::~TSmartBuffer() {
  // This buffer didn't point to anything, so no need to clean anything up.
  if(!fReferenceCount){
    return;
  }

  (*fReferenceCount)--;

  if(*fReferenceCount==0){
    free(fAllocatedLocation);
    delete fReferenceCount;
  }
}

TSmartBuffer::TSmartBuffer(const TSmartBuffer& other)
  : TObject(other),
    fAllocatedLocation(other.fAllocatedLocation), fData(other.fData),
    fSize(other.fSize), fReferenceCount(other.fReferenceCount) {


  if(fReferenceCount){
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
  TString options(opt);
  TRegexp regexp("0x[0-9a-f][0-9a-f][0-9a-f][0-9a-f]");
  //TRegexp regexp2("0x[0-9a-f][0-9a-f][0-9a-f][0-9a-f]");
  if(!options.Contains("bodyonly")) {
    std::cout << "TSmartBuffer allocated at " << (void*)fAllocatedLocation << ", "
              << "currently pointed at " << (void*)fData << ", "
              << "with a size of " << fSize << "bytes."
              << std::endl;
  }
  if(fReferenceCount){
    std::cout << "There are " << *fReferenceCount << " TSmartBuffers sharing this C-buffer"
              << std::endl;

    // Full hexdump of the buffer contents
    //TString highlight_string = options(regexp);
    std::vector<TString> highlight_strings;
    while(options.Contains(regexp)) {
      highlight_strings.push_back(options(regexp));
      options.ReplaceAll(highlight_strings.back(),"");
    }
    //unsigned short highlight = 0;
    std::vector<unsigned short> highlight;
    if(highlight_strings.size()) { //Length()) {
      //highlight = strtol(highlight_string.Data(),0,0);
      for(unsigned int j=0;j<highlight_strings.size();j++) {
        highlight.push_back(strtol(highlight_strings.at(j).Data(),0,0));
      }
    }
    if(options.Contains("all")){
      printf("\t");
      for(unsigned int x=0; x<GetSize()-1; x+=2) {
        if((x%16 == 0) &&
           (x!=GetSize())){
          printf("\n\t");
        }
        unsigned int value = *(unsigned short*)(GetData()+x);
        //if(highlight>0 && highlight==value) {printf(DRED);}
        bool found = false;
        for(unsigned int j=0;j<highlight.size();j++) {
          if(highlight.at(j) == value)
            found = true;
        }
        if(found) {printf(DRED);}
        printf("0x%04x  ",value);
        if(found) {printf(RESET_COLOR);}
        //if(highlight>0 && highlight==value) {printf(RESET_COLOR);}

      }
      if(GetSize()%2 == 1){
        printf("0x%02x ", *(unsigned char*)(GetData()+GetSize()-1));
      }

      printf("\n--------------------------\n");
    }
  }
}
