#ifndef _TDETECTORFACTORY_H_
#define _TDETECTORFACTORY_H_

class TDetector;

class TDetectorFactoryBase {
public:
  virtual ~TDetectorFactoryBase() { }

  virtual TDetector* construct() = 0;
  virtual bool is_instance(TDetector* det) = 0;
};

template<typename T>
class TDetectorFactory : public TDetectorFactoryBase {
public:
  virtual TDetector* construct() {
    return new T();
  }

  virtual bool is_instance(TDetector* det) {
    return dynamic_cast<T*>(det);
  }
};

#endif /* _TDETECTORFACTORY_H_ */
