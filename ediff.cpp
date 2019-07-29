#include <TGretina.h>
#include <TGretinaHit.h>
#include <TFile.h>
#include <map>
#include <TBank88.h>
#include <TH1D.h>
#include <TH2D.h>
#include <vector>

void ediff() {
    std::multimap<long, TGretinaHit> ghit;
    std::multimap<long,TBank88> uhit;
    TBank88 *bank = 0;
    TBank88 *uml = 0;
    TGretina *gretina = 0;
    gChain->SetBranchAddress("TBank88",&uml);
    gChain->SetBranchAddress("TGretina",&gretina);

    TFile *opf = new TFile("temp.root","RECREATE");
    TTree *opt = new TTree("opt","opt");
    Long64_t ts_dan ;
    Float_t en_dan;
    opt->Branch("ts_dan",&ts_dan,"ts_dan/L");
    opt->Branch("en_dan",&en_dan,"en_dan/L");
    TH2D *hh = new TH2D("hh","hh",30000,-30000,30000,4000,0,4000);
    TH2D *hh3 =  new TH2D("hh_m2","hh_m2",300,-30000,30000,4000,0,4000);
    TH2D *hh4 =  new TH2D("hh_m2_n","hh_m2_n",300,-30000,30000,4000,0,4000);
    TH2D *hh2 = new TH2D("hh2","hh2",1000,0,4000,1000,0,4000);
    long twin = 300000; //30 us
    std::vector<std::multimap<long,TGretinaHit>::iterator> store_ghit;
    long nentries = gChain->GetEntries();
//  long nentries = 1000000;
    for(Long64_t i = 0; i< nentries; i++) {
        gChain->GetEntry(i);
        if(uml->Timestamp()>0) uhit.insert(std::make_pair(uml->Timestamp()*10,*uml));
        if(gretina->Size()>0) {
            for(size_t i = 0; i<gretina->Size(); i++) {
                TGretinaHit *gretinahit = (TGretinaHit*)&(gretina->GetHit(i));
                if(gretinahit->GetPad()==0)
                    ghit.insert(std::make_pair(gretinahit->Timestamp()*10,*gretinahit));
            }
        }

       auto itr = ghit.rbegin();
//     std::cout<<"size uhit: "<<uhit.size()<<std::endl;
//     std::cout<<"size ghit: "<<ghit.size()<<std::endl;
        while(ghit.size()>0 && uhit.size()>0 && (itr->first-uhit.begin()->first>twin)) {
            while(uhit.begin()->first - ghit.begin()->first > twin ) {
                ghit.erase(ghit.begin());
            }
            long fts = uhit.begin()->first;
            for( auto it = ghit.begin(); it!=ghit.end(); it++) {
                if(it->first>fts+twin) break;
                TGretinaHit tmp_hit = it->second;
                ts_dan = it->first - fts;
                en_dan = tmp_hit.GetCoreEnergy();
                hh->Fill(it->first-fts,tmp_hit.GetCoreEnergy());
                store_ghit.push_back(it);
                opt->Fill();
            }
            bool *fill = new bool[store_ghit.size()];
            memset(fill,0,sizeof(bool)*(store_ghit.size()));
            if(store_ghit.size()>1)
                for(size_t l = 0; l<store_ghit.size()-1; l++) {
                    if(store_ghit.at(l+1)->first-store_ghit.at(l)->first<500) {
                        if(fill[l] == false) {
                            TGretinaHit tmphit = store_ghit.at(l)->second;
                            hh4->Fill(tmphit.Timestamp()*10-fts,tmphit.GetCoreEnergy());
                            fill[l] = true;
                        }
                        if(fill[l+1] == false) {
                            TGretinaHit tmphit = store_ghit.at(l+1)->second;
                            hh4->Fill(tmphit.Timestamp()*10-fts,tmphit.GetCoreEnergy());
                            fill[l+1] = true;
                        }
                    }
                }
            delete [] fill;
            store_ghit.clear();
            uhit.erase(uhit.begin());
        }

        if((i%5000)==0) {
            printf( "  on entry %lu / %lu                \r",i,nentries);
            fflush(stdout);
        }
    }
    printf( "  on entry %lu / %lu                \n",nentries,nentries);

    opt->Write();
    opf->Close();
}
