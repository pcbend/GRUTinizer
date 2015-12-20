#ifndef TGRUTVARIABLE_H
#define TGRUTVARIABLE_H

#include "TNamed.h"

class GValue : public TNamed {
public:
  GValue();
  GValue(const char *name);
  GValue(const char *name,double value);
  GValue(const GValue &val);

  double GetValue() { return fValue; }
  void SetValue(double value) { fValue = value; }

  virtual void Copy(TObject &obj) const;

private:
  double fValue;

  ClassDef(GValue,1);
};

#endif
