#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "global.h"
#include <vector>
#include <TROOT.h>
#include <TTree.h>
#include <TFile.h>
#include <TUML.h>

using namespace std;


typedef unsigned short WORD;
typedef unsigned char  BYTE;

typedef unsigned short UINT16;
typedef signed short INT16;



struct gebData
{
    int type;
    int length; // payload in bytes
    long long timestamp;
};


struct HFC_item
{
    gebData geb;
    BYTE*  data;
};

void printout(gebData aGeb) {
    cout<<"Type : "<<aGeb.type<<endl;
    cout<<"leng : "<<aGeb.length<<endl;
    cout<<"TS   : "<<aGeb.timestamp<<endl;
}

void search(FILE *in, int n=1){
    gebData aGeb;
    BYTE cbuf[8*1024];
    gebData gebs[5];
    int index = 0;
    bool flag=false;
    int cnt = 0;
    while(fread(&aGeb,sizeof(struct gebData),1,in)==1){
        int read = fread(cbuf,sizeof(char),aGeb.length,in);
        if(read != aGeb.length) return;
        if(aGeb.type == n) {
            flag = true;
            break;
        }
        gebs[index] = aGeb;
        index = (index +1 )%5;
        cnt++;
    }
    if(flag){
        index = (index +1)%5;
        if(cnt>5 ) cnt = 5;
        for(int i = 0; i < cnt; i++ ){
            printout(gebs[index]);
            index = (index +1)%5;
        }
        printout(aGeb);
    }
}

int main(int argc, char** argv) {
    if(argc<2) {
        cout<<argv[0]<<" filename"<<endl;
    }

    FILE *in = NULL;
    if(strcmp(argv[1],"STDIN"))
       in = fopen(argv[1],"rb");
    else
       in = stdin;
    if(!in) {
        cout<<"cannot open file: "<< argv[1] <<endl;
    }

    bool flag = true;
    gebData aGeb;
    BYTE cbuf[8*1024];
    int read = 0;
    int cnt = 0;
    vector<long long> gretina;
    vector<long long> ddas;
   

    while(flag) {
        if(fread(&aGeb,sizeof(struct gebData),1,in) !=1 ) break;
        read = fread(cbuf,sizeof(char),aGeb.length,in);
        if(read != aGeb.length) break;
        if(aGeb.type == 0x19) ddas.push_back(aGeb.timestamp);
//        else if(aGeb.type == 1 || aGeb.type == 8) gretina.push_back(aGeb.timestamp);
        else if(aGeb.type == 1) gretina.push_back(aGeb.timestamp);

        cnt++;
    }

    cout<<"size ddas: "<<ddas.size()<<endl;
    cout<<"size gre : "<<gretina.size()<<endl;

    // save tdiff to the tree
    Long64_t tdiff = 0;
    TFile *opf = new TFile("temp.root","RECREATE");
    TTree *opt = new TTree("t","t");
    opt->Branch("tdiff",&tdiff,"tdiff/L");

    for (size_t i = 0; i<gretina.size(); i++){
        Long64_t fts = gretina.at(i);
        vector<long long>::iterator index = std::lower_bound(ddas.begin(),ddas.end(),fts-1000); // search window +/-10us
        while(index != ddas.end() && abs((*index)-fts)<=1000){
           tdiff = (*index)-fts;
           opt->Fill();
           index++;
        }
    }
    opt->Write();
    opf->Close();
}
