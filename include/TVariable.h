#ifndef _TVARIABLE_H_
#define _TVARIABLE_H_

#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include <Rtypes.h>

class TVariable {
  public:
    TVariable():fValue(sqrt(-1)),fName("") { }
    TVariable(double v,const char *s) : fValue(v),fName(s) { }
    TVariable(const TVariable &v) { v.Copy(*this); }
    virtual ~TVariable() { }

    virtual void Copy(TVariable&) const;
    virtual void Print(Option_t *opt="") const;
    void Clear(Option_t *opt);

    const char *GetName() const { return fName.c_str(); }
    void Set(double tmp) { fValue = tmp; }

    bool IsZero() const {if((fValue!=fValue)||GetAsInt()==0) return true;return false;}

    double   GetAsDouble() const { return fValue; }
    int      GetAsInt()    const { return static_cast<int>(fValue); }
    UInt_t   GetAsUInt()   const { return static_cast<UInt_t>(fValue); }
    short    GetAsShort()  const { return static_cast<short>(fValue); }
    UShort_t GetAsUShort() const { return static_cast<UShort_t>(fValue); }
    long     GetAsLong()   const { return static_cast<long>(fValue); }
    ULong_t  GetAsULong()  const { return static_cast<ULong_t>(fValue); }

    TVariable& operator=(const TVariable &v)  { v.Copy(*this); return *this;}
    TVariable& operator+=(const TVariable &rhs) {fValue+=rhs.fValue;return *this;} 
    TVariable& operator-=(const TVariable &rhs) {fValue-=rhs.fValue;return *this;} 
    TVariable& operator*=(const TVariable &rhs) {fValue*=rhs.fValue;return *this;} 
    TVariable& operator/=(const TVariable &rhs) {fValue/=rhs.fValue;return *this;} 
    bool       operator!()  const   { return IsZero(); }
    double     operator()() const   { return GetAsDouble(); }
    void       operator++()         {fValue=fValue+1.0;} 
    void       operator++(int d)    {fValue+=d;} 
    void       operator--()         {fValue=fValue-1.0;} 
    void       operator--(int d)    {fValue-=d;} 

    //TODO:  move these to the cxx, use std::tie
    bool operator==(const TVariable &v) { return GetAsInt()==v.GetAsInt(); }
    bool operator!=(const TVariable &v) { return GetAsInt()!=v.GetAsInt(); }
    bool operator<=(const TVariable &v) { return GetAsInt()<=v.GetAsInt(); }
    bool operator>=(const TVariable &v) { return GetAsInt()>=v.GetAsInt(); }
    bool operator<(const TVariable &v)  { return GetAsInt()<v.GetAsInt(); }
    bool operator>(const TVariable &v)  { return GetAsInt()>v.GetAsInt(); }


    friend TVariable operator+(const TVariable& lhs,const TVariable& rhs);
    friend double    operator+(const TVariable& lhs,double);
    friend TVariable operator-(const TVariable& lhs,const TVariable& rhs);
    friend double    operator-(const TVariable& lhs,double);
    friend TVariable operator*(const TVariable& lhs,const TVariable& rhs);
    friend double    operator*(const TVariable& lhs,double);
    friend TVariable operator/(const TVariable& lhs,const TVariable& rhs);
    friend double    operator/(const TVariable& lhs,double);
    friend TVariable operator%(const TVariable& lhs,const TVariable& rhs);
    friend int       operator%(const TVariable& lhs,int);

    friend std::ostream& operator<<(std::ostream&,const TVariable&);


  private:
    double      fValue;
    std::string fName;

    static std::map<std::string,TVariable*> fVarMap;

  private: 
    double Value(std::string);


  ClassDef(TVariable,1)

};

TVariable operator+(const TVariable&,const TVariable&);
double    operator+(const TVariable&,double);
TVariable operator-(const TVariable&,const TVariable&);
double    operator-(const TVariable&,double);
TVariable operator*(const TVariable&,const TVariable&);
double    operator*(const TVariable&,double);
TVariable operator/(const TVariable&,const TVariable&);
double    operator/(const TVariable&,double);
TVariable operator%(const TVariable&,const TVariable&);
int       operator%(const TVariable&,int);

std::ostream& operator<<(std::ostream&,const TVariable&);

#endif

