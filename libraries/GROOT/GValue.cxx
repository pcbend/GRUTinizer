#include "GValue.h"

GValue::GValue()
  : fValue(0.00) { }

GValue::GValue(const char *name,double value)
  : TNamed(name,name), fValue(value) { }

GValue::GValue(const char *name)
  : TNamed(name,name), fValue(0.00) { }

GValue::GValue(const GValue &val) {
  val.Copy(*this);
}

void GValue::Copy(TObject &obj) const {
  TNamed::Copy(obj);
  ((GValue&)obj).fValue = fValue;
}
