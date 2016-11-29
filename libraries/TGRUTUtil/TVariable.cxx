
#include <TVariable.h>

std::map<std::string,TVariable*> TVariable::fVarMap;

void TVariable::Copy(TVariable &v) const {
  v.fValue = fValue;
  v.fName.assign(fName); 

}

void TVariable::Print(Option_t *opt) const {
  printf("%s:   %.03f\n",fName.c_str(),fValue);
}

void TVariable::Clear(Option_t *opt) {
  fValue = sqrt(-1);
  fName.clear();
}

double TVariable::Value(std::string(fName)) {
  //if(!var)
  //  return sqrt(-1);
  if(fVarMap.size()) {
    if(fVarMap.count(fName)) {
      return fVarMap.at(fName)->fValue;
    }
  }
  return sqrt(-1);
}


///////////////////////////////////
///////////////////////////////////
///////////////////////////////////
///////////////////////////////////

TVariable operator+(const TVariable& lhs,const TVariable& rhs) {
  return TVariable(lhs.GetAsDouble()+rhs.GetAsDouble(),lhs.GetName());
}

double    operator+(const TVariable& lhs,double d) {
  return lhs.GetAsDouble()+d; 
}

TVariable operator-(const TVariable& lhs,const TVariable& rhs) {
  return TVariable(lhs.GetAsDouble()-rhs.GetAsDouble(),lhs.GetName());
}

double    operator-(const TVariable& lhs,double d) {
  return lhs.GetAsDouble()-d; 
}

TVariable operator*(const TVariable& lhs,const TVariable& rhs) {
  return TVariable(lhs.GetAsDouble()*rhs.GetAsDouble(),lhs.GetName());
}

double    operator*(const TVariable& lhs,double d) {
  return lhs.GetAsDouble()*d; 
}

TVariable operator/(const TVariable& lhs,const TVariable& rhs) {
  return TVariable(lhs.GetAsDouble()/rhs.GetAsDouble(),lhs.GetName());
}

double    operator/(const TVariable& lhs,double d) {
  return lhs.GetAsDouble()/d; 
}

TVariable operator%(const TVariable& lhs,const TVariable& rhs) {
  return TVariable(lhs.GetAsInt()%rhs.GetAsInt(),lhs.GetName());
}

int       operator%(const TVariable& lhs,int d) {
  return lhs.GetAsInt()%d; 
}

std::ostream& operator<<(std::ostream &out,const TVariable& v) {
  return out << v.GetName() << "\t" << v();
}



