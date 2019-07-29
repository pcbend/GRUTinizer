#include <TUML.h>
#include <TUMLHit.h>
#include <TGretina.h>
#include <TGretinaHit.h>
#include <TBank88.h>
#include <TH1D.h>
#include <TH2D.h>
#include <vector>

void tdiff(){
  std::vector<long > gts;
  std::vector<long> dts;
  std::vector<long> xts;
  std::vector<TGretina
  TBank88 *bank = 0;
  TUML *uml = 0;
  gChain->SetBranchAddress("TBank88",&bank);
  gChain->SetBranchAddress("TUML",&uml);

//  for(Long64_t i = 0; i< gChain->GetEntries(); i++){
//
  long nentries = gChain->GetEntries(); 
//  long nentries = 10000; 
  for(Long64_t i = 0; i< nentries; i++){
     gChain->GetEntry(i);
     if(bank->Timestamp()>0) gts.push_back(bank->Timestamp()*10);
     if(uml->Timestamp()>0) dts.push_back(uml->Timestamp()*10);
     if(uml->Size()>0) {
       TUMLHit uhit =  uml->GetUMLHit(0);
       if(uhit.Timestamp()>0) xts.push_back(uhit.Timestamp()*8);
     }
     if((i%5000)==0) {
        printf( "  on entry %lu / %lu                \r",i,nentries);
        fflush(stdout);
     }
  }
        printf( "  on entry %lu / %lu                \n",nentries,nentries);

  long tdiff_bank_ddas;
  long tdiff_bank_ddashit;
  long tdiff_ddas_ddashit;

  cout<<"gts size: "<<gts.size()<<endl;
  cout<<"dts size: "<<dts.size()<<endl;
  cout<<"xts size: "<<xts.size()<<endl;

  long twin = 1000; //1 us
  TH2D *h1 = new TH2D("h1","h1",3000,0,600,1000,-1e3,1e3);
  TH2D *h2 = new TH2D("h2","h2",3000,0,600,1000,0,1e6);
  TH1D *h3 = new TH1D("h3","h3",1000,0,0);
    for (size_t k = 0; k<gts.size(); k++){
        Long64_t fts = gts.at(k);
        std::vector<long>::iterator index = std::lower_bound(dts.begin(),dts.end(),fts-twin); // search window +/-10us
        while(index != dts.end() && abs((*index)-fts)<=twin){
           tdiff_bank_ddas = (*index)-fts;
           if(!isnan(tdiff_bank_ddas)) 
           h1->Fill(fts/1e9,tdiff_bank_ddas);
//           h2->Fill(fts/1e9,);
           index++;
        }
//        index = std::lower_bound(dts.begin(),dts.end(),fts-twin); // search window +/-10us
//        size_t index2 = index-dts.begin();
//        while(index2 < xts.size() && abs(xts.at(index2)-fts)<=twin){
//           tdiff_bank_ddashit = (*index)-fts;
//           if(!isnan(tdiff_bank_ddashit)) 
//           h2->Fill(fts/1e9,tdiff_bank_ddashit);
//           index++;
//        }
    }
    for (size_t k = 0; k<dts.size(); k++){
        Long64_t fts = dts.at(k);
        std::vector<long>::iterator index = std::lower_bound(xts.begin(),xts.end(),fts-twin); // search window +/-10us
        while(index != xts.end() && abs((*index)-fts)<=twin){
           tdiff_ddas_ddashit = (*index)-fts;
           if(!isnan(tdiff_ddas_ddashit)) 
           h3->Fill(tdiff_ddas_ddashit);
           index++;
        }
    }

    h1->Draw();
    h2->Draw();
    h3->Draw();
    TCanvas *c1 = new TCanvas("c1","tdiff_vs_time");
    c1->Divide(2,1);
    c1->cd(1);
    h1->Draw();
    c1->cd(2);
    h2->Draw();

  TH2D *h4 = new TH2D("h4","h4",3000,0,300,5000,980000,1000000);
  TH2D *h5 = new TH2D("h5","h5",3000,0,300,5000,980000,1000000);
  TH2D *h6 = new TH2D("h6","h6",3000,0,300,5000,980000,1000000);

  for(size_t k = 0; k< gts.size()-1; k++)
    h4->Fill(gts.at(k)/1e9,gts.at(k+1)-gts.at(k));
  for(size_t k = 0; k< dts.size()-1; k++)
    h5->Fill(dts.at(k)/1e9,dts.at(k+1)-dts.at(k));
  for(size_t k = 0; k< xts.size()-1; k++)
    h6->Fill(xts.at(k)/1e9,xts.at(k+1)-xts.at(k));
//  TCanvas *c = new TCanvas();
//  c->Divide(1,3);
//  c->cd(1);
//  h4->Draw("colz");
//  c->cd(2);
//  h5->Draw("colz");
//  c->cd(3);
//  h6->Draw("colz");

}
