#ifndef _TLIVEHISTOGRAMWSHANDLER_H_
#define _TLIVEHISTOGRAMWSHANDLER_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <mutex>

#include "THttpWSHandler.h"

class TList;
class TObject;

class TLiveHistogramWSHandler : public THttpWSHandler {
public:
  TLiveHistogramWSHandler(const char* name = "live",
                          const char* title = "GRUTinizer live histogram stream");

  Bool_t ProcessWS(THttpCallArg* arg) override;
  void PublishSnapshots(TList& objects);

private:
  struct ClientState {
    bool subscribe_all = false;
    std::set<std::string> subscriptions;
  };

  struct PublishedObject {
    std::string path;
    std::string class_name;
    TObject* object = nullptr;
  };

  static std::string Trim(const std::string& input);
  static std::string ToUpper(std::string input);

  std::vector<PublishedObject> CollectObjects(TList& objects) const;
  void SendKnownList(UInt_t wsid);
  void SendText(UInt_t wsid, const std::string& text);
  void SendSnapshot(UInt_t wsid, const PublishedObject& object);

  std::mutex client_mutex;
  std::map<UInt_t, ClientState> clients;
  std::vector<PublishedObject> known_objects;
};

#endif /* _TLIVEHISTOGRAMWSHANDLER_H_ */
