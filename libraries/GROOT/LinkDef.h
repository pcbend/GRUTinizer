// GRootGuiFactory.h GRootFunctions.h GRootCommands.h GRootCanvas.h GRootBrowser.h GCanvas.h GH2I.h  GPeak.h GValue.h GH1D.h


#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ namespace GRootFunctions;

//#pragma link C++ function GRootFunctions::PolyBg;
//#pragma link C++ function GRootFunctions::StepBG;
//#pragma link C++ function GRootFunctions::StepFunction;
//#pragma link C++ function GRootFunctions::PhotoPeak;
//#pragma link C++ function GRootFunctions::PhotoPeakBG;
//#pragma link C++ function GRootFunctions::Gaus;
//#pragma link C++ function GRootFunctions::SkewedGaus;


#pragma link C++ function LabelPeaks;
#pragma link C++ function ShowPeaks;
#pragma link C++ function RemovePeaks;
#pragma link C++ function PhotoPeakFit;
#pragma link C++ function GausFit;

#pragma link C++ function MergeStrings;

//#pragma link C++ function PeakFit;

#pragma link C++ function Help;
#pragma link C++ function Commands;
#pragma link C++ function GrabHist;

//#pragma link C++ global gManager;
//#pragma link C++ global gBaseManager;

#pragma link C++ class GValue+;
#pragma link C++ class GMarker+;
#pragma link C++ class GCanvas+;
#pragma link C++ class GRootCanvas+;
#pragma link C++ class GRootBrowser+;
#pragma link C++ class GBrowserPlugin+;
#pragma link C++ class GRootGuiFactory+;

#pragma link C++ class GH1D+;
#pragma link C++ class GH2I+;

#pragma link C++ class GPeak+;
//#pragma link C++ class GEfficiency+;

#endif
