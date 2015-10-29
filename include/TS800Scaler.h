
#include <TDetector.h>
#include <vector>

class TGEBEvent;

class TS800Scaler : public TDetector {
  public:
    TS800Scaler();
    TS800Scaler(const TS800Scaler&);
    //TS800Scaler(const TGEBEvent&);
    virtual ~TS800Scaler();

    virtual void Copy(TObject&)       const;
    virtual void Print(Option_t *opt) const;
    virtual void Clear(Option_t *opt);

    int Size()           { return scalers.size(); }
    int GetScaler(int i) { return scalers.at(i);  }


  private:
    std::vector<int> scalers;
    virtual int BuildHits();

    int interval_start;
    int interval_end;  
    int interval_div;  
    int unix_time;     
    int num_scalers;   

  ClassDef(TS800Scaler,1)
};















