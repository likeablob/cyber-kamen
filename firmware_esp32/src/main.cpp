#include <Arduino.h>
#include <Chrono.h>

#include "ble.h"
#include "config_manager.h"
#include "display.h"
#ifdef ENABLE_OTA
#include "ota.h"
#endif

ConfigManager configManager;
DisplayManager displayManager(&configManager);

void setup() {
    Serial.begin(115200);
    Serial.println("started");

#ifdef ENABLE_OTA
    OTAService::begin();
#endif
    configManager.begin();
    displayManager.begin();
    BleManager::begin(&configManager, &displayManager);
}

void loop() {
#ifdef ENABLE_OTA
    OTAService::handle();
#endif

    BleManager::handle();
    displayManager.handle();
}
