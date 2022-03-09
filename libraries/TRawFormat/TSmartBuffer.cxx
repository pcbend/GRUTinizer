#include "TSmartBuffer.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "TString.h"
#include "TPRegexp.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "Globals.h"

TSmartBuffer::TSmartBuffer()
  : fData(NULL), fSize(0), fAllocatedData(nullptr) { }

TSmartBuffer::TSmartBuffer(char* buffer, size_t size)
  : fData(buffer), fSize(size) {

  fAllocatedData = std::shared_ptr<char>(
    buffer,
    [](char* buffer) { free(buffer); }
  );
}

TSmartBuffer::~TSmartBuffer() { }

void TSmartBuffer::Clear(){
  *this = TSmartBuffer();
}

void TSmartBuffer::SetBuffer(char* buffer, size_t size){
  *this = TSmartBuffer(buffer, size);
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
  //TRegexp regexp2("0x[0-9a-f][0-9a-f][0-9a-f][0-9a-f]");
  if(!options.Contains("bodyonly")) {
    std::cout << "TSmartBuffer allocated at " << (void*)fAllocatedData.get() << ", "
              << "currently pointed at " << (void*)fData << ", "
              << "with a size of " << fSize << " bytes."
              << std::endl;
  }

  if(fData){
    std::cout << "There are " << fAllocatedData.use_count() << " TSmartBuffers sharing this C-buffer"
              << std::endl;

    // Find all regexes in the option string
    std::vector<TPRegexp> highlight_regexes;
    {
      TPRegexp regexp("0x[0-9a-f?*]{1,4}");
      TObjArray* all_matches = regexp.MatchS(options);
      TIter iter(all_matches);
      while(TObject* obj = iter()) {
        TString tstring = ((TObjString*)obj)->GetString();
        options.ReplaceAll(tstring, "");

        // Convert bash-style wildcard to regex.
        tstring.ReplaceAll("?", "[0-9a-f]");
        tstring.ReplaceAll("*", "[0-9a-f]*");
        tstring.Append("$");
        highlight_regexes.push_back(tstring);
      }
      delete all_matches;
    }

    // Full hexdump of the buffer contents
    //TString highlight_string = options(regexp);
    if(options.Contains("all")){
      std::cout << "\t";
      for(unsigned int x=0; x<GetSize()-1; x+=2) {
        if((x%16 == 0) &&
           (x!=GetSize())){
          std::cout << "\n\t";
        }

        unsigned int value = *(unsigned short*)(GetData()+x);
        std::stringstream stream;
        stream << "0x"
               << std::hex << std::setfill('0') << std::setw(4) << value;
        TString value_to_print = stream.str();

        bool needs_highlight = false;
        for(auto& regex : highlight_regexes) {
          if(regex.Match(value_to_print)) {
            needs_highlight = true;
            break;
          }
        }

        if(needs_highlight) { std::cout << DRED; }
        std::cout << value_to_print << "  ";
        if(needs_highlight) { std::cout << RESET_COLOR; }
      }


      if(GetSize()%2 == 1){
        printf("0x%02x ", *(unsigned char*)(GetData()+GetSize()-1));
      }

      printf("\n--------------------------\n");
    }
  }
}
