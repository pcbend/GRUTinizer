#ifndef _TUNPACKEDEVENT_H_
#define _TUNPACKEDEVENT_H_

#ifndef __CINT__
#include <type_traits>
#endif

#include "TClass.h"

#include "TDetector.h"
#include "TGRUTTypes.h"
#include "TRawEvent.h"

class TUnpackedEvent {
public:
  TUnpackedEvent();
  ~TUnpackedEvent();

  template<typename T>
  T* GetDetector();

  std::vector<TDetector*>& GetDetectors() { return detectors; }
  void AddDetector(TDetector* det) { detectors.push_back(det); }
  void AddRawData(const TRawEvent& event, kDetectorSystems detector);

  void Build();

  int Size() { return detectors.size(); }

private:
  std::vector<TDetector*> detectors;
};

#ifndef __CINT__
template<typename T>
T* TUnpackedEvent::GetDetector() {
  static_assert(std::is_base_of<TDetector, T>::value,
                "T must be a subclass of TDetector");
  for(auto det : detectors) {
    T* output = dynamic_cast<T*>(det);
    if(output){
      return output;
    }
  }

  return NULL;
}
#endif

#endif /* _TUNPACKEDEVENT_H_ */
