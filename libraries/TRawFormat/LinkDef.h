// TRawSource.h TRawEvent.h TSmartBuffer.h TMultiRawFile.h TGlobRawFile.h TOrderedRawFile.h  TSequentialRawFile.h TRawFileOut.h TTreeSource.h TRCNPSource.h

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;



#pragma link C++ class TSmartBuffer+;
#pragma link C++ class TRawEvent+;
#pragma link C++ class std::vector<TRawEvent>+;

#pragma link C++ class TRawEventSource+;
#pragma link C++ class TRawEventTimestampSource+;

#pragma link C++ class TByteSource+;
#pragma link C++ class TFileByteSource+;
#pragma link C++ class TGZipByteSource+;
#pragma link C++ class TPipeByteSource+;
#pragma link C++ class TBZipByteSource+;
#pragma link C++ class TRingByteSource+;

#pragma link C++ class TRawFile+;
#pragma link C++ class TRawFileIn+;

#pragma link C++ class TMultiRawFile+;
#pragma link C++ class TGlobRawFile+;
#pragma link C++ class TOrderedRawFile+;
#pragma link C++ class TSequentialRawFile+;
#pragma link C++ class TRawFileOut+;

#pragma link C++ class TTreeSource<RCNPEvent>+;
#pragma link C++ class TRCNPSource+;

#pragma link C++ enum TRawEvent::ArgonneType;

#endif
