#pragma once

#ifdef ENABLE_OTA
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

namespace OTAService {
void begin();
void handle();
} // namespace OTAService

#endif
