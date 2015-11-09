#ifndef GRUTCANVAS_H
#define GRUTCANVAS_H

#include "TROOT.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
//#include "TPeak.h"

#include "TH1.h"
#include "TLine.h"

class GMarker : public TObject{
  public:
    GMarker():x(-1),y(-1),linex(0),liney(0),localx(0.0),localy(0.0) { }
    GMarker(const GMarker &m) { ((GMarker&)m).Copy(*this); }
    virtual ~GMarker() { if(linex) linex->Delete(); if(liney) liney->Delete(); }
    int x;
    int y;
    double localx;
    double localy;
    TLine *linex;
    TLine *liney;
    void Copy(TObject &object) const;
    bool operator<(const GMarker &rhs) const { return x < rhs.x; }
    ClassDef(GMarker,0)
};



class GCanvas : public TCanvas {
   public:
      GCanvas(Bool_t build = kTRUE);
      GCanvas(const char* name, const char* title = "", Int_t form = 1);
      GCanvas(const char* name, const char* title, Int_t ww, Int_t wh);
      GCanvas(const char* name, Int_t ww, Int_t wh, Int_t winid);
      GCanvas(const char* name, const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
      virtual ~GCanvas();

      //void ProcessEvent(Int_t event,Int_t x,Int_t y,TObject *obj);
      //void CatchEvent(Int_t event,Int_t x,Int_t y,TObject *obj);
      void HandleInput(Int_t event,Int_t x,Int_t y);
      void Draw(Option_t *opt="");

      static GCanvas *MakeDefCanvas(); 

      Int_t  GetNMarkers()               { return fMarkers.size();    }
      //Int_t  GetNBG_Markers()            { return fBG_Markers.size(); }
      void SetMarkerMode(bool flag=true) { fMarkerMode = flag;        }

      //static void SetBackGroundSubtractionType();

      TF1 *GetLastFit();
   private:
      void GCanvasInit();

      void UpdateStatsInfo(int,int);

      static int lastx;
      static int lasty;

      //bool fStatsDisplayed;
      bool fMarkerMode;
      std::vector<GMarker*> fMarkers;
      void AddMarker(int,int,int dim=1);
      void RemoveMarker(Option_t *opt="");
      void OrderMarkers();

      //std::vector<GMarker*> fBG_Markers;
      //void AddBGMarker(GMarker *mark);
      //void RemoveBGMarker();
      //void ClearBGMarkers();
      //void OrderBGMarkers();

      std::vector<TH1*> FindHists(int dim=1);
      std::vector<TH1*> FindAllHists();

   public:
     //void HandleKeyPress(int event,int x,int y,TObject *obj);
     bool HandleArrowKeyPress(Event_t *event,UInt_t *keysym);
     bool HandleKeyboardPress(Event_t *event,UInt_t *keysym);
     bool HandleMousePress(Int_t event,Int_t x,Int_t y);

   //private:

     bool Process1DArrowKeyPress(Event_t *event,UInt_t *keysym);
     bool Process1DKeyboardPress(Event_t *event,UInt_t *keysym);
     bool Process1DMousePress(Int_t event,Int_t x,Int_t y);

     bool Process2DArrowKeyPress(Event_t *event,UInt_t *keysym);
     bool Process2DKeyboardPress(Event_t *event,UInt_t *keysym);
     bool Process2DMousePress(Int_t event,Int_t x,Int_t y);
      
      //bool SetBackGround(GMarker *m1=0,GMarker *m2=0,GMarker *m3=0,GMarker *m4=0);
      //bool SetLinearBG(GMarker *m1=0,GMarker *m2=0);
      //bool SetConstantBG(); //GMarker *m1=0,GMarker *m2=0);
      //bool SetBGGate(GMarker *m1,GMarker *m2,GMarker *m3=0,GMarker *m4=0);
      
      //TH1 *GetBackGroundHist(GMarker *addlow,GMarker *addhigh);
      
      //bool GausFit(GMarker *m1=0,GMarker *m2=0);
      //bool GausBGFit(GMarker *m1=0,GMarker *m2=0);
      //bool PeakFit(GMarker *m1=0,GMarker *m2=0);
      //bool PeakFitQ(GMarker *m1=0,GMarker *m2=0);

      //static int fBGSubtraction_type;
   private:
      Window_t fCanvasWindowID;
      TRootCanvas *fRootCanvas;

      bool control_key;

      bool toggle_control() { control_key = !control_key;}

   ClassDef(GCanvas,2);
};

#endif