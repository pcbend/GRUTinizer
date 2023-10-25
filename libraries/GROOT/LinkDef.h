// GRootGuiFactory.h GRootFunctions.h GRootCommands.h GRootCanvas.h GRootBrowser.h GCanvas.h  GH2I.h GH2D.h  GPeak.h GGaus.h GValue.h GH1D.h GrutNotifier.h  GSnapshot.h GCutG.h GH1.h GH2.h GPopup.h GHistPopup.h TF1Sum.h  GGraph.h GGraph2D.h GDoubleGaus.h


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
#pragma link C++ function DoubleGausFit;

#pragma link C++ function MergeStrings;

//#pragma link C++ function PeakFit;

#pragma link C++ function Help;
#pragma link C++ function Commands;
#pragma link C++ function Version;
#pragma link C++ function GrabHist;
#pragma link C++ function GrabFit;
#pragma link C++ function StartGUI;
#pragma link C++ function GUIIsRunning;
#pragma link C++ class DummyGuiCaller+;
#pragma link C++ function GetProjectionX;
#pragma link C++ function GetProjectionY;

#pragma link C++ enum EAxis;
#pragma link C++ enum kBackgroundSubtraction;
//#pragma link C++ function AddOffset;


#pragma link C++ class GPopup+;
#pragma link C++ class GHistPopup+;

#pragma link C++ class GCutG+;
#pragma link C++ class GPeak+;
#pragma link C++ class GGaus+;
#pragma link C++ class GDoubleGaus+;

#pragma link C++ class GValue-;
#pragma link C++ class std::map<std::string,GValue*>+;
#pragma link C++ class std::pair<std::string,GValue*>+;
#pragma link C++ class GMarker+;
#pragma link C++ class GCanvas-;
#pragma link C++ class GRootCanvas+;
#pragma link C++ class GRootBrowser+;
#pragma link C++ class GBrowserPlugin+;
#pragma link C++ class GRootGuiFactory+;


#pragma link C++ class GH1+;
#pragma link C++ class GH2+;

#pragma link C++ class GH1D+;
#pragma link C++ class GH2I+;
#pragma link C++ class GH2D+;

//#pragma link C++ class GEfficiency+;
#pragma link C++ class GGraph+;
#pragma link C++ class GGraph2D+;

#pragma link C++ class TF1Sum+;

#pragma link C++ class GrutNotifier+;

#pragma link C++ class GSnapshot+;


//#pragma link C++ class GHist-;
//#pragma link C++ class xGH1D-;

#endif
