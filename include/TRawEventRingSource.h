#ifndef _TRAWEVENTRINGSOURCE_H_
#define _TRAWEVENTRINGSOURCE_H_

#include <string>

#include "TRawEventSource.h"

class TRawEventRingSource : public TRawEventPipeSource {
public:
  TRawEventRingSource(const std::string& ringname, kFileType file_type);
  ~TRawEventRingSource() { }

  virtual std::string SourceDescription() const;

private:
  std::string fRingName;

  ClassDef(TRawEventRingSource,0);
};

#endif /* _TRAWEVENTRINGSOURCE_H_ */
