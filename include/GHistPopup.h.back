#ifndef _GHISTPOPUP_H_
#define _GHISTPOPUP_H_

#include <TGButton.h>
#include <TGListView.h>

#include <GPopup.h>

class GH2;

class GHistPopup : public GPopup {
  public: 
   GHistPopup(GH2 *parent,int w=500,int h=200);
   
   virtual ~GHistPopup() { }
   
   void PrintSelected() const;
   
   void SetFrames();

  private:
    TGListView    *listv; 
    TGLVContainer *listc; 

    void AddEntry(TList*);
    void AddEntry(const char*);


  ClassDef(GHistPopup,0)
};


#endif
