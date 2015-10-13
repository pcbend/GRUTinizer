#ifndef GEFFICIENCY_H
#define GEFFICIENCY_H

#include <TF1.h>

#include <string>
#include <algorithm>

class GEfficiency : public TF1 {
 public:
  GEfficiency(Double_t zeroth,Double_t first,Double_t second, Double_t inverse); // done
  GEfficiency(const GEfficiency&); // done
  GEfficiency(); //done
  virtual ~GEfficiency(); // done

  void Copy(TObject&) const;          // done
  void Print(Option_t *opt="") const; // done
  void Clear(Option_t *opt="");       // done

  void InitNames();                   // done
  bool InitParams();              // done
  bool Fit(TH1*, Option_t *opt="");
  void DrawResids(TH1*) const;

 private:
  double fchi2;
  double fNdf;

  bool IsInitialized() const { return init_flag; }
  void SetInitialized(bool flag = true) {init_flag = flag;}
  bool init_flag;

  ClassDef(GEfficiency,1);
};

#endif
