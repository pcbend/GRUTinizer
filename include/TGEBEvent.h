#ifndef _TGEBEVENT_H_
#define _TGEBEVENT_H_

class TGEBEvent : public TRawEvent {
  public:
    Long_t GetTimeStamp() const {
      return *((Long_t*)(GetBody()+0));
    }
    char* GetPayload(){
      return (GetBody() + sizeof(Long_t));
    }
   
    bool operator<(const TGEBEvent &rhs) const { return (GetTimeStamp() < rhs.GetTimeStamp()); }
    bool operator>(const TGEBEvent &rhs) const { return (GetTimeStamp() > rhs.GetTimeStamp()); }

  ClassDef(TGEBEvent,0);
};

#endif /* _TGEBEVENT_H_ */
