#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <algorithm>



#define PRINT(x) std::cout << #x"=" << x << std::endl
#define STR(x) #x << '=' << x

#define GEB_HEADER_SIZE 16



using namespace std;

// template <class T>
// void endswap(T *objp)
// {
//   unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
//   reverse(memp, memp + sizeof(T));
// }

void endswap(uint16_t* datum) {
  uint16_t temp = 0;
  temp = (*datum&0x00ff);
  *datum =  (temp<<8) + ((*datum)>>8);
}
void endswap(uint32_t* datum) {
  uint32_t t1 = 0, t2 = 0, t3 = 0;
  t1 = ((*datum)&0x000000ff);
  t2 = ((*datum)&0x0000ff00);
  t3 = ((*datum)&0x00ff0000);
  *datum =  (t1<<24) + (t2<<8) + (t3>>8) + ((*datum)>>24);
}


struct GEB_HEADER {
  uint32_t type;
  uint32_t length;
  uint64_t timestamp;
}__attribute__((__packed__));

struct ANL_LED_v11 {
  uint32_t type;
  uint32_t length;
  uint64_t timestamp;
  uint16_t ga_packetlength;
  uint16_t ud_channel;
  uint32_t led_low;
  uint16_t hdrlength_evttype_hdrtype;

  uint16_t led_high;
  uint16_t led_low_prev;
  uint16_t flags;
  uint32_t led_high_prev;
  uint32_t sampled_baseline;
  uint32_t blank;
  uint32_t postrise_sum_low_prerise_sum;
  uint16_t timestamp_peak_low;
  uint16_t postrise_sum_high;
  uint32_t timestamp_peak_high;
  uint16_t postrise_end_sample;
  uint16_t postrise_begin_sample;
  uint16_t prerise_end_sample;
  uint16_t prerise_begin_sample;
  uint16_t base_sample;
  uint16_t peak_sample;
}__attribute__((__packed__));



int main (int argc, char **argv) {
  int i;
  bool debug = false;

  if (argc<2) {
    printf("[PATH TO INPUTFILE LIST] [-v PRINT EVENT]\n");
    exit(-1);
  }
  if (argc>2){ // lazy
    if (strcmp(argv[2],"-v")==0) {
      debug = true;
    }
  }

  FILE *fd,*list;
  char fileName[255];
  char DataName[255];

  strcpy(fileName,argv[1]);
  list=fopen(fileName,"r");

  while(fscanf(list,"%s",DataName)!=EOF) {
    printf("%s\n",DataName);
    fd = fopen(DataName, "r");
    if (!fd) {
      perror("Failed to open");
      exit(1);
    }

    int nevents =0;
    int datasize = 64 + GEB_HEADER_SIZE;
    unsigned char* header = (unsigned char*)malloc(GEB_HEADER_SIZE);
    unsigned char* payload = (unsigned char*)malloc(datasize*4);

    while (fread(header,GEB_HEADER_SIZE,1,fd) == 1) {

      //if (nevents>10) break;

      auto head = (GEB_HEADER*)header;
      //cout << head->length << endl;
      //cin.get();
      datasize = head->length;

      if (fread(payload,datasize,1,fd) == 1) {

        auto event = (ANL_LED_v11*)payload;

        endswap(&event->ga_packetlength);
        uint16_t ga = ((event->ga_packetlength & 0xf800) >> 11);
        uint16_t length = (event->ga_packetlength & 0x7ff);
        endswap(&event->ud_channel);
        uint16_t userdefined = ((event->ud_channel & 0xfff0) >> 4);
        uint16_t channel = (event->ud_channel & 0xf);

        endswap(&event->led_low);
        endswap(&event->led_high);
        uint64_t ts = (((uint64_t)event->led_high) << 32) + ((uint64_t)event->led_low);

        endswap(&event->hdrlength_evttype_hdrtype);
        uint16_t headertype = ((event->hdrlength_evttype_hdrtype & 0xf) >> 0);
        uint16_t eventtype = ((event->hdrlength_evttype_hdrtype & 0x380) >> 7);
        uint16_t headerlength = ((event->hdrlength_evttype_hdrtype & 0xfc00) >> 10);

        endswap(&event->led_low_prev);
        endswap(&event->led_high_prev);
        uint64_t ts_prev = (((uint64_t)event->led_high_prev)  << 16) + ((uint64_t)event->led_low_prev);

        endswap(&event->flags);
        uint16_t external_disc = ((event->flags & 0x100)>>8);
        uint16_t peak_valid = ((event->flags & 0x200)>>9);
        uint16_t offset = ((event->flags & 0x400)>>10);
        uint16_t sync_error = ((event->flags & 0x1000)>>12);
        uint16_t general_error = ((event->flags & 0x2000)>>13);
        uint16_t pile_up_only = ((event->flags & 0x4000)>>14);
        uint16_t pile_up = ((event->flags & 0x8000)>>15);

        endswap(&event->sampled_baseline);
        uint32_t sampled_baseline = ((event->sampled_baseline & 0x00FFFFFF) >> 0);

        endswap(&event->postrise_sum_low_prerise_sum);
        endswap(&event->postrise_sum_high);
        uint32_t prerise_sum = (event->postrise_sum_low_prerise_sum & 0xffffff);
        uint32_t postrise_sum = ((event->postrise_sum_low_prerise_sum & 0xff000000)>>24);
        postrise_sum += (((uint32_t)event->postrise_sum_high) << 8);

        endswap(&event->timestamp_peak_low);
        endswap(&event->timestamp_peak_high);
        uint64_t peak_timestamp = ((uint64_t)event->timestamp_peak_low) + (((uint64_t)event->timestamp_peak_high)<<16);

        endswap(&event->postrise_end_sample);
        uint16_t postrise_end_sample = (event->postrise_end_sample & 0x3fff);

        endswap(&event->postrise_begin_sample);
        uint16_t postrise_begin_sample = (event->postrise_begin_sample & 0x3fff);

        endswap(&event->prerise_end_sample);
        uint16_t prerise_end_sample = (event->prerise_end_sample & 0x3fff);

        endswap(&event->prerise_begin_sample);
        uint16_t prerise_begin_sample = (event->prerise_begin_sample & 0x3fff);

        endswap(&event->base_sample);
        uint16_t base_sample = (event->base_sample & 0x3fff);

        endswap(&event->peak_sample);
        uint16_t peak_sample = (event->peak_sample & 0x3fff);

        //cout << head->timestamp << endl;

        if (debug) {
          cout << "\n\n   Event number: " << nevents+1 << endl;
          for (i=0;i<GEB_HEADER_SIZE;i++){
            if(i%2==0)cout << " ";
            if(i%16==0)cout << endl;
            cout << setfill('0') << setw(2) <<right << hex << (int)header[i];
          }
          for (i=0;i<datasize;i++){
            if(i%2==0)cout << " ";
            if(i%16==0)cout << endl;
            cout << setfill('0') << setw(2) <<right << hex << (int)payload[i];
          } cout << endl<<endl;
          cout << dec << STR(head->type) << endl;
          cout << STR(head->length) << endl;
          cout << STR(head->timestamp) << endl;
          cout << STR(ga) << endl;
          cout << STR(length) << endl;
          cout << STR(channel) << endl;
          cout << STR(userdefined) << endl; //boardid
          cout << STR(ts) << endl;
          cout << STR(headertype) << endl;
          cout << STR(eventtype) << endl;
          cout << STR(headerlength) << endl;
          cout << STR(ts_prev) << endl;
          cout << STR(external_disc) << endl;
          cout << STR(peak_valid) << endl;
          cout << STR(offset) << endl;
          cout << STR(sync_error) << endl;
          cout << STR(general_error) << endl;
          cout << STR(pile_up_only) << endl;
          cout << STR(pile_up) << endl;
          cout << dec << STR(sampled_baseline) << endl;
          cout << STR(prerise_sum) << endl;
          cout << STR(postrise_sum) << endl;
          cout << STR(peak_timestamp) << endl;
          PRINT(postrise_end_sample);
          PRINT(postrise_begin_sample);
          PRINT(prerise_end_sample);
          PRINT(prerise_begin_sample);
          PRINT(base_sample);
          PRINT(peak_sample);
        }

        nevents++;
      }
    }
    delete payload;
    delete header;
    fclose(fd);
    cout << "Read " << nevents << " events.\n";
  }
  fclose(list);
  exit(0);
}
