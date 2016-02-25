#ifndef TPRESETCANVAS_H
#define TPRESETCANVAS_H

#include <map>

#include "TList.h"
#include "TNamed.h"

class TPresetCanvas;

class TPresetPad : public TNamed {
  
  public:
    TPresetPad(const char *name="");
    TPresetPad(const TPresetPad&);
    virtual ~TPresetPad(); 
   
    void SetColNumber(int num)         { fColNumber  = num;  }
    void SetRowNumber(int num)         { fRowNumber  = num;  }
    void SetObjName(std::string name)  { fObjectName = name.c_str();   }
    void SetOption(std::string option) { fOption     = option.c_str(); }
    void SetColor(std::string color)   { fColor      = color.c_str(); }

    int GetPadNumber()         { return fPadNumber; }
    int GetColNumber()         { return fColNumber;  }
    int GetRowNumber()         { return fRowNumber;  }
    const char *GetObjName()   { return fObjectName.Data(); } 
    const char *GetOption()    { return fOption.Data();     } 
    const char *GetColor()     { return fColor.Data();     } 

    virtual void Print(Option_t *opt="") const;
    virtual void Clear(Option_t *opt="");
    virtual void Draw(Option_t *opt="");
    virtual void Copy(TObject&) const;

    void AppendOption(const char *opt) { fOption.Append(opt); }
    std::string PrintString(Option_t *opt="") const;

    void Set(int,int);

  //private:
    int fPadNumber;
    int fRowNumber;
    int fColNumber;
    TString fObjectName;
    TString fOption;
    TString fColor;

  ClassDef(TPresetPad,0)
};



class TPresetCanvas : public TNamed {
  public:
    TPresetCanvas();
    virtual ~TPresetCanvas();

    virtual void Print(Option_t *opt="") const ;
    virtual void Clear(Option_t *opt="");
    virtual void Draw(Option_t *opt="");

    int Size() const { return fPadMap.size(); }

    std::string PrintString(Option_t *opt="") const;

    int GetMaxRow() { return fRows; }
    int GetMaxCol() { return fCols; }

    void Set();
    void AddPad(TPresetPad &pad); 

  //private:
    std::map<int,TPresetPad> fPadMap;

    int fRows;
    int fCols;

  public:
    int ReadWinFile(const char *filename="",Option_t *opt="replace");
    int WriteWinFile(std::string filename="",Option_t *opt="");
    int Size()  { return fPadMap.size(); }

  //private:
    int  ParseInputData(const std::string input,Option_t *opt="");
    //static std::map<std::string,TPresetCanvas*> fCanvasMap;
    static void trim(std::string *, const std::string &trimChars=" \f\n\r\t\v");

  ClassDef(TPresetCanvas,1);
};




#endif
