//TChannel.h TGRUTUtilities.h TPresetCanvas.h TCalibrator.h TVariable.h
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class TVariable+;

#pragma link C++ class TChannel-;
#pragma link C++ class TCalibrator+;
#pragma link C++ struct TCalibrator::Peak+;

#pragma link C++ class TPresetPad+;
#pragma link C++ class TPresetCanvas-;

#pragma link C++ function get_run_number;
#pragma link C++ function FindColor;
#pragma link C++ function CalculateParameters;
#pragma link C++ function MakeVectorFromFile;



#pragma link C++ function operator+(const TVariable&,const TVariable&);
#pragma link C++ function operator+(const TVariable&,double);
#pragma link C++ function operator-(const TVariable&,const TVariable&);
#pragma link C++ function operator-(const TVariable&,double);
#pragma link C++ function operator*(const TVariable&,const TVariable&);
#pragma link C++ function operator*(const TVariable&,double);
#pragma link C++ function operator/(const TVariable&,const TVariable&);
#pragma link C++ function operator/(const TVariable&,double);
#pragma link C++ function operator%(const TVariable&,const TVariable&);
#pragma link C++ function operator%(const TVariable&,int);




#endif
