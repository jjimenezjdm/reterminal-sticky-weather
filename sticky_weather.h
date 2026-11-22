// Memory that survives deep sleep (RTC slow memory) for the Sticky weather panel:
//  - reference temperatures from ~1 h ago, for the trend arrows
//  - a "signature" of what was last drawn, to skip redrawing when nothing changed
#pragma once
#include "esp_attr.h"
#include <cmath>
#include <ctime>
#include <string>

struct RtcState {
  uint32_t magic;
  float ref_indoor;      // indoor temperature at the last reference point
  float ref_outdoor;     // outdoor temperature at the last reference point
  time_t ref_time;       // when the reference was taken
  int trend_indoor;      // -1 falling, 0 steady, 1 rising (over the last hour)
  int trend_outdoor;
  uint32_t signature;    // hash of what is on screen
  time_t drawn_time;     // when the screen was last redrawn
};

static RTC_DATA_ATTR RtcState rtc_state_data;
static const uint32_t RTC_MAGIC = 0x57C4A5E1;

inline RtcState &rtc_state() {
  if (rtc_state_data.magic != RTC_MAGIC) {   // first boot (or after a full power loss)
    rtc_state_data = {RTC_MAGIC, NAN, NAN, 0, 0, 0, 0, 0};
  }
  return rtc_state_data;
}

inline uint32_t fnv1a(const std::string &s) {
  uint32_t h = 2166136261u;
  for (unsigned char c : s) { h ^= c; h *= 16777619u; }
  return h;
}
