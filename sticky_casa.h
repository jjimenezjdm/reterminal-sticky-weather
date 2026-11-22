// Memoria que sobrevive al sueño profundo (RTC) para el Sticky Casa:
//  - temperaturas de referencia de hace ~1 h, para las flechas de tendencia
//  - "firma" de lo último que se pintó, para no repintar si nada ha cambiado
#pragma once
#include "esp_attr.h"
#include <cmath>
#include <ctime>
#include <string>

struct EstadoRTC {
  uint32_t magia;
  float ref_casa;        // temperatura de casa en la última referencia
  float ref_calle;       // temperatura de la calle en la última referencia
  time_t ref_t;          // cuándo se tomó la referencia
  int tend_casa;         // -1 bajando, 0 estable, 1 subiendo (última hora)
  int tend_calle;
  uint32_t firma;        // hash de lo pintado
  time_t pintado_t;      // cuándo se pintó por última vez
};

static RTC_DATA_ATTR EstadoRTC rtc_estado;
static const uint32_t RTC_MAGIA = 0x57C4A5E1;

inline EstadoRTC &estado_rtc() {
  if (rtc_estado.magia != RTC_MAGIA) {   // primer arranque (o tras quitar la batería)
    rtc_estado = {RTC_MAGIA, NAN, NAN, 0, 0, 0, 0, 0};
  }
  return rtc_estado;
}

inline uint32_t fnv1a(const std::string &s) {
  uint32_t h = 2166136261u;
  for (unsigned char c : s) { h ^= c; h *= 16777619u; }
  return h;
}
