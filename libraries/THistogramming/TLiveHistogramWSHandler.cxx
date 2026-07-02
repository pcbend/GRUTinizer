#include "TLiveHistogramWSHandler.h"

#include <algorithm>
#include <cctype>
#include <sstream>

#include "TBufferFile.h"
#include "TClass.h"
#include "TDirectory.h"
#include "THttpCallArg.h"
#include "TList.h"
#include "TObject.h"
#include "TString.h"

TLiveHistogramWSHandler::TLiveHistogramWSHandler(const char* name, const char* title)
  : THttpWSHandler(name, title, kFALSE) { }

std::string TLiveHistogramWSHandler::Trim(const std::string& input) {
  std::string::size_type first = 0;
  while(first < input.size() &&
        std::isspace(static_cast<unsigned char>(input[first]))) {
    first++;
  }

  std::string::size_type last = input.size();
  while(last > first &&
        std::isspace(static_cast<unsigned char>(input[last - 1]))) {
    last--;
  }

  return input.substr(first, last - first);
}

std::string TLiveHistogramWSHandler::ToUpper(std::string input) {
  std::transform(input.begin(), input.end(), input.begin(),
                 [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
  return input;
}

Bool_t TLiveHistogramWSHandler::ProcessWS(THttpCallArg* arg) {
  if(!arg || arg->GetWSId() == 0) {
    return kTRUE;
  }

  const UInt_t wsid = arg->GetWSId();

  if(arg->IsMethod("WS_CONNECT")) {
    return kTRUE;
  }

  if(arg->IsMethod("WS_READY")) {
    {
      std::lock_guard<std::mutex> lock(client_mutex);
      clients[wsid] = ClientState();
    }
    SendText(wsid, "READY GRUTLIVE/1\n");
    return kTRUE;
  }

  if(arg->IsMethod("WS_CLOSE")) {
    std::lock_guard<std::mutex> lock(client_mutex);
    clients.erase(wsid);
    return kTRUE;
  }

  if(!arg->IsMethod("WS_DATA")) {
    return kFALSE;
  }

  std::string command(static_cast<const char*>(arg->GetPostData()),
                      static_cast<size_t>(arg->GetPostDataLength()));
  command = Trim(command);
  std::istringstream stream(command);
  std::string verb;
  stream >> verb;
  verb = ToUpper(verb);

  if(verb == "PING") {
    SendText(wsid, "PONG\n");
    return kTRUE;
  }

  if(verb == "LIST") {
    SendKnownList(wsid);
    return kTRUE;
  }

  if(verb == "SUBSCRIBE") {
    std::string path;
    stream >> path;
    path = Trim(path);
    if(path.empty()) {
      SendText(wsid, "ERROR SUBSCRIBE requires a path or *\n");
      return kTRUE;
    }

    {
      std::lock_guard<std::mutex> lock(client_mutex);
      ClientState& client = clients[wsid];
      if(path == "*") {
        client.subscribe_all = true;
      } else {
        client.subscriptions.insert(path);
      }
    }
    SendText(wsid, "OK SUBSCRIBE " + path + "\n");
    return kTRUE;
  }

  if(verb == "UNSUBSCRIBE") {
    std::string path;
    stream >> path;
    path = Trim(path);
    if(path.empty()) {
      SendText(wsid, "ERROR UNSUBSCRIBE requires a path or *\n");
      return kTRUE;
    }

    {
      std::lock_guard<std::mutex> lock(client_mutex);
      ClientState& client = clients[wsid];
      if(path == "*") {
        client.subscribe_all = false;
        client.subscriptions.clear();
      } else {
        client.subscriptions.erase(path);
      }
    }
    SendText(wsid, "OK UNSUBSCRIBE " + path + "\n");
    return kTRUE;
  }

  SendText(wsid, "ERROR unknown command\n");
  return kTRUE;
}

std::vector<TLiveHistogramWSHandler::PublishedObject>
TLiveHistogramWSHandler::CollectObjects(TList& objects) const {
  std::vector<PublishedObject> result;

  TIter next(&objects);
  TObject* current = nullptr;
  while((current = next())) {
    TDirectory* dir = dynamic_cast<TDirectory*>(current);
    if(dir) {
      TString folder = TString::Format("/histograms/%s", dir->GetName());
      TIter dir_next(dir->GetList());
      TObject* dir_object = nullptr;
      while((dir_object = dir_next())) {
        result.push_back(PublishedObject{
          TString::Format("%s/%s", folder.Data(), dir_object->GetName()).Data(),
          dir_object->ClassName(),
          dir_object
        });
      }
    } else {
      result.push_back(PublishedObject{
        TString::Format("/histograms/%s", current->GetName()).Data(),
        current->ClassName(),
        current
      });
    }
  }

  std::sort(result.begin(), result.end(),
            [](const PublishedObject& lhs, const PublishedObject& rhs) {
              return lhs.path < rhs.path;
            });
  return result;
}

void TLiveHistogramWSHandler::SendKnownList(UInt_t wsid) {
  std::vector<PublishedObject> snapshot;
  {
    std::lock_guard<std::mutex> lock(client_mutex);
    snapshot = known_objects;
  }

  std::ostringstream reply;
  reply << "LIST_BEGIN\n";
  for(const PublishedObject& object : snapshot) {
    reply << object.path << '\t' << object.class_name << '\n';
  }
  reply << "LIST_END\n";
  SendText(wsid, reply.str());
}

void TLiveHistogramWSHandler::SendText(UInt_t wsid, const std::string& text) {
  SendCharStarWS(wsid, text.c_str());
}

void TLiveHistogramWSHandler::SendSnapshot(UInt_t wsid, const PublishedObject& object) {
  if(!object.object) {
    return;
  }

  TBufferFile buffer(TBuffer::kWrite);
  buffer.WriteObject(object.object);

  std::ostringstream header;
  header << "GRUTLIVE/1 SNAPSHOT\n"
         << "PATH " << object.path << "\n"
         << "CLASS " << object.class_name << "\n"
         << "BYTES " << buffer.Length() << "\n\n";

  std::string payload = header.str();
  payload.append(static_cast<const char*>(buffer.Buffer()),
                 static_cast<size_t>(buffer.Length()));
  SendWS(wsid, payload.data(), static_cast<int>(payload.size()));
}

void TLiveHistogramWSHandler::PublishSnapshots(TList& objects) {
  std::vector<PublishedObject> objects_to_publish = CollectObjects(objects);
  std::map<UInt_t, ClientState> client_snapshot;

  {
    std::lock_guard<std::mutex> lock(client_mutex);
    known_objects = objects_to_publish;
    client_snapshot = clients;
  }

  for(const auto& client : client_snapshot) {
    const UInt_t wsid = client.first;
    const ClientState& state = client.second;
    if(!state.subscribe_all && state.subscriptions.empty()) {
      continue;
    }

    for(const PublishedObject& object : objects_to_publish) {
      if(state.subscribe_all ||
         state.subscriptions.count(object.path)) {
        SendSnapshot(wsid, object);
      }
    }
  }
}
