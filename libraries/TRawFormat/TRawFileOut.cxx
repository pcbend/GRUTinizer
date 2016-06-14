#include "TRawFileOut.h"

#include <algorithm>
#include <cstdint>

#include "TGEBEvent.h"
#include "TNSCLEvent.h"
#include "TRawEvent.h"
#include "TUnpackedEvent.h"

namespace {
  bool IsGEBData(TRawEvent& event) {
    auto file_type = event.GetFileType();
    return file_type == kFileType::GRETINA_MODE2 ||
      file_type == kFileType::GRETINA_MODE3;
  }

  bool IsGEBData(TUnpackedEvent& event) {
    for(auto& det_raw_data : event.GetRawData()) {
      for(auto& raw_event : det_raw_data.second) {
        return IsGEBData(raw_event);
      }
    }
    return false;
  }

  bool IsNSCLBuiltData(TUnpackedEvent& event) {
    for(auto& det_raw_data : event.GetRawData()) {
      for(auto& raw_event : det_raw_data.second) {
        return ((TNSCLEvent&)raw_event).IsBuiltData();
      }
    }
    return false;
  }
}

TRawFileOut::TRawFileOut()
  : raw_file_out(nullptr), gzip_file_out(nullptr) { }

TRawFileOut::TRawFileOut(const std::string& filename)
  : TRawFileOut() {

  size_t dot_pos = filename.find_last_of('.');
  std::string ext = filename.substr(dot_pos + 1);
  bool is_zipped = (ext == "gz");

  if(is_zipped) {
    gzip_file_out = new gzFile;
    *gzip_file_out = gzopen(filename.c_str(), "wb");
  } else {
    raw_file_out = fopen(filename.c_str(), "wb");
  }
}

TRawFileOut::~TRawFileOut() {
  if(raw_file_out) {
    fclose(raw_file_out);
  }
  if(gzip_file_out) {
    gzclose(*gzip_file_out);
    delete gzip_file_out;
  }
}

void TRawFileOut::Open(const std::string& filename) {
  TRawFileOut new_file(filename);
  swap(new_file);
}

void TRawFileOut::swap(TRawFileOut& other) {
  std::swap(raw_file_out, other.raw_file_out);
  std::swap(gzip_file_out, other.gzip_file_out);
}

void TRawFileOut::Write(TRawEvent& event) {
  WriteBytes((char*)event.GetRawHeader(), sizeof(TRawEvent::RawHeader));

  TSmartBuffer buf;
  if(IsGEBData(event)) {
    // GEB data may have the timestamp tacked on,
    //   if it is a Mode3 data packet that has been split up.
    long timestamp = event.GetTimestamp();
    WriteBytes((char*)&timestamp, sizeof(timestamp));
    buf = ((TGEBEvent&)event).GetPayloadBuffer();
  } else {
    buf = event.GetBuffer();
  }
  WriteBytes(buf.GetData(), buf.GetSize());
}

void TRawFileOut::Write(TUnpackedEvent& event) {
  if(IsGEBData(event) || !IsNSCLBuiltData(event)) {
    WriteUnbuiltEvent(event);
  } else {
    WriteBuiltNSCLEvent(event);
  }
}

void TRawFileOut::WriteUnbuiltEvent(TUnpackedEvent& event) {
  for(auto& det_raw_data : event.GetRawData()) {
    for(auto& raw_event : det_raw_data.second) {
      Write(raw_event);
    }
  }
}

void TRawFileOut::WriteBuiltNSCLEvent(TUnpackedEvent& event) {
  size_t total_event_size = 0;
  size_t num_events = 0;
  std::uint64_t event_timestamp = -1;
  for(auto& det_raw_data : event.GetRawData()) {
    for(auto& raw_event : det_raw_data.second) {
      event_timestamp = std::min<std::uint64_t>(event_timestamp, raw_event.GetTimestamp());
      total_event_size += raw_event.GetTotalSize();
      num_events++;
    }
  }

  if(num_events == 0) {
    return;
  }

  std::uint32_t built_item_body_size =
    total_event_size + num_events*sizeof(TRawEvent::TNSCLFragmentHeader) + sizeof(std::uint32_t);

  std::uint32_t ring_item_size =
    built_item_body_size + sizeof(TRawEvent::NSCLBodyHeader) + sizeof(TRawEvent::RawHeader);

  // The header of the event
  {
    TRawEvent::RawHeader raw_header;
    raw_header.datum1 = ring_item_size;
    raw_header.datum2 = kNSCLEventType::PHYSICS_EVENT;
    WriteBytes((char*)&raw_header, sizeof(raw_header));
  }

  // The body header of the event
  {
    TRawEvent::NSCLBodyHeader body_header;
    body_header.body_header_size = sizeof(body_header);
    body_header.timestamp = event_timestamp;
    body_header.sourceid = 0;
    body_header.barrier = 0;
    WriteBytes((char*)&body_header, sizeof(body_header));
  }

  // Size of the body
  WriteBytes((char*)&built_item_body_size, sizeof(built_item_body_size));

  // Fragment header, then event, for each fragment.
  for(auto& det_raw_data : event.GetRawData()) {
    for(auto& raw_event : det_raw_data.second) {
      TRawEvent::TNSCLFragmentHeader fragment_header;
      fragment_header.timestamp = raw_event.GetTimestamp();
      fragment_header.sourceid = ((TNSCLEvent&)raw_event).GetSourceID();
      fragment_header.payload_size = raw_event.GetTotalSize();
      fragment_header.barrier = 0;
      WriteBytes((char*)&fragment_header, sizeof(fragment_header));

      Write(raw_event);
    }
  }
}

void TRawFileOut::WriteBytes(const char* data, size_t size) {
  if(raw_file_out) {
    fwrite(data, sizeof(char), size, raw_file_out);
  } else if(gzip_file_out) {
    gzwrite(*gzip_file_out, data, size);
  }
}
