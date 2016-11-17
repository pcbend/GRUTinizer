
#include <GHistPopup.h>

#include <GH2.h>

GHistPopup::GHistPopup(GH2 *parent,int w,int h) : 
  GPopup(gClient->GetDefaultRoot(),gClient->GetDefaultRoot(),w,h),listv(0),listc(0) {
  SetFrames();
  AddEntry(parent->GetProjections());
  Init();
  PrintSelected();
}

void GHistPopup::SetFrames() {
  TGVerticalFrame   *vframe = new TGVerticalFrame(this,500,200);
                     listv  = new TGListView(vframe,500,200);
                     listc  = new TGLVContainer(listv->GetViewPort(),500,200,
                                                kHorizontalFrame,fgWhitePixel);
                     listc->Associate(this);
                     listv->SetContainer(listc);
                     listv->GetViewPort()->SetBackgroundColor(fgWhitePixel);
                     listv->SetViewMode(kLVList);
                     //listv->SetIncremental(1,19);
  /////////////////////////////////
  /////////////////////////////////
  
  //TGLVEntry *myentry= new TGLVEntry(listc,"name1","name2");
  //myentry->Connect("Clicked()","GPopup",this,"Print");
  //myentry->SetItemName("Iamname");
  //listc->AddItem(myentry);
  
  /////////////////////////////////
  /////////////////////////////////

  vframe->AddFrame(listv,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

  TGHorizontalFrame *hframe = new TGHorizontalFrame(vframe,500,20);
  TGTextButton      *b1     = new TGTextButton(hframe,"&button1");
  TGTextButton      *b2     = new TGTextButton(hframe,"&button2");
  b1->Connect("Clicked()","GHistPopup",this,"PrintSelected()");
  b2->Connect("Clicked()","GPopup",this,"Print()");
  hframe->AddFrame(b1,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));
  hframe->AddFrame(b2,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

  vframe->AddFrame(hframe,new TGLayoutHints(kLHintsExpandX));
  this->AddFrame(vframe,new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));
}


void GHistPopup::AddEntry(const char *name) {
  //printf("%s\t0x%08x\n",__PRETTY_FUNCTION__,listc);
  if(!listc) return;
  TGLVEntry *myentry= new TGLVEntry(listc,"name","name");
  listc->AddItem(myentry);
}

void GHistPopup::AddEntry(TList *list) {
  //printf("%s\t0x%08x\n",__PRETTY_FUNCTION__,listc);
  if(!listc || list) return;
  TIter iter(list);
  while(TObject *obj=iter.Next())
    AddEntry(obj->GetName());
}

void GHistPopup::PrintSelected() const {
  //printf("%s\t0x%08x\n",__PRETTY_FUNCTION__,listc);
  if(!listc) return;
  TList *list = listc->GetSelectedItems();
  if(list) {
    printf("list->GetSize() = %i\n",list->GetSize());
    list->Print();
  }

}


