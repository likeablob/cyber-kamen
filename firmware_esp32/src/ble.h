#pragma once

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Chrono.h>

#include "config_manager.h"
#include "display.h"

#define BLE_DEVICE_NAME "CyberKamen"

#define CK_BLE_SERVICE_UUID "1546c1bd-186a-498b-89f6-712abee5affd"
#define CK_BLE_CHAR_UUID_DRAW "4b8473f1-f01f-4f35-94ca-2c27d5659601"
#define CK_BLE_CHAR_UUID_DRAW_MONO "4b8473f1-f01f-4f35-94ca-2c27d5659602"
#define CK_BLE_CHAR_UUID_CONFIG "4b8473f1-f01f-4f35-94ca-2c27d5659610"

#define POST_DISCONNECTION_TASK_DELAY_MS 500

namespace BleManager {
static Chrono postDisconnectionTask;
static BLECharacteristic *bleCharDraw;
static BLECharacteristic *bleCharDrawMono;
static BLECharacteristic *bleCharConfig;

void begin(ConfigManager *configManager, DisplayManager *displayManager);
void handle();

} // namespace BleManager
