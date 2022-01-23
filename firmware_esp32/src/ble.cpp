#include "ble.h"
#include <MsgPack.h>

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        Serial.println("connected");
        BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer *pServer) {
        Serial.println("disconnected");
        BleManager::postDisconnectionTask.restart();
    }
};

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
  public:
    ConfigManager *configManager;
    DisplayManager *displayManager;

    CharacteristicCallbacks(ConfigManager *configManager,
                            DisplayManager *displayManager)
        : configManager(configManager), displayManager(displayManager){};

    void parseDraw(uint8_t *data, uint8_t len) {}

    void parseDrawMono(uint8_t *data, uint8_t len) {
        MsgPack::Unpacker unpacker;
        unpacker.feed(data, len);

        MsgPack::bin_t<uint8_t> color;
        MsgPack::bin_t<uint8_t> monoFrame;

        // 1. Ensure the top object is an array of 2 elements
        if(!(unpacker.isArray() && unpacker.unpackArraySize() == 2)) {
            Serial.println("Something went wrong. Can't find an array at top.");
            return;
        }

        // 2. The first element can be null or Bin([r, g, b])
        bool useDefaultColor = false;
        if(unpacker.isNil()) {
            useDefaultColor = true;
            unpacker.unpackNil();
        } else if(unpacker.isBin()) {
            color = unpacker.unpackBinary();
            if(color.size() != 3) {
                Serial.println(
                    "Something went wrong. color[].length must be 3.");
                return;
            }
        } else {
            Serial.println(
                "Something went wrong. Can't find a proper element at [0].");
            return;
        }

        // 3. The second element must be Bin.
        if(!unpacker.isBin()) {
            Serial.println(
                "Something went wrong. Can't find a proper element at [1].");
            return;
        }
        monoFrame = unpacker.unpackBinary();

        if(monoFrame.size() != NUMPIXELS / 8) {
            Serial.printf("Something went wrong. monoFrame.length must be "
                          "NUMPIXELS/8. length: %d, NUMPIXELS/8: %d\r\n",
                          monoFrame.size(), NUMPIXELS / 8);
            return;
        }

        if(useDefaultColor) {
            displayManager->stopBlink();
            displayManager->writeBuffer(monoFrame.data(), true);
        } else {
            displayManager->stopBlink();
            rgb24_t col = {color[0], color[1], color[2]};
            displayManager->writeBuffer(monoFrame.data(), col, true);
        }
    }

    void parseConfig(uint8_t *data, uint8_t len) {
        MsgPack::Unpacker unpacker;
        unpacker.feed(data, len);
        bool blinkEnabled = false;
        MsgPack::bin_t<uint8_t> defaultColor; // RGB

        // 1. Ensure the top object is an array of 2 elements
        if(!(unpacker.isArray() && unpacker.unpackArraySize() == 2)) {
            Serial.println("Something went wrong. Can't find an array at top.");
            return;
        }

        // 2. The first element can be null or bool (blinkEnabled)
        if(unpacker.isNil()) {
            unpacker.unpackNil();
        } else if(unpacker.isBool()) {
            blinkEnabled = unpacker.unpackBool();
        } else {
            Serial.println(
                "Something went wrong. Can't find a proper element at [0].");
            return;
        }

        // 3. The second element can be null or Bin([r, g, b])
        if(unpacker.isNil()) {
            unpacker.unpackNil();
        } else if(unpacker.isBin()) {
            defaultColor = unpacker.unpackBinary();
            if(defaultColor.size() != 3) {
                Serial.println(
                    "Something went wrong. color[].length must be 3.");
                return;
            }
        } else {
            Serial.println(
                "Something went wrong. Can't find a proper element at [1].");
            return;
        }

        Serial.println("Setting defaultColor.");
        Serial.println(defaultColor.size());
        if(!defaultColor.empty()) {
            // Set defaultColor before starting blink
            rgb24_t col = {defaultColor[0], defaultColor[1], defaultColor[2]};
            configManager->setDefaultColor(col);
        }

        // Trigger blinking
        if(blinkEnabled) {
            displayManager->startBlink();
        }
    }

    void onWrite(BLECharacteristic *pCharacteristic) {
        Serial.println("received");
        Serial.println(pCharacteristic->getUUID().toString().c_str());

        auto &&data = pCharacteristic->getData();
        auto &&dataLen = pCharacteristic->getValue().length();
        Serial.println("dataLen");
        Serial.println(dataLen);

        if(pCharacteristic == BleManager::bleCharDraw) {
            Serial.println("draw");
            parseDraw(data, dataLen);
        } else if(pCharacteristic == BleManager::bleCharDrawMono) {
            Serial.println("mono");
            parseDrawMono(data, dataLen);
        } else if(pCharacteristic == BleManager::bleCharConfig) {
            Serial.println("blink");
            parseConfig(data, dataLen);
        } else {
            Serial.println("unknown characteristic!");
        }
    }
};

void BleManager::begin(ConfigManager *configManager,
                       DisplayManager *displayManager) {
    BleManager::postDisconnectionTask.stop();

    BLEDevice::init(BLE_DEVICE_NAME);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService *pService = pServer->createService(CK_BLE_SERVICE_UUID);

    CharacteristicCallbacks *charCallbacks =
        new CharacteristicCallbacks(configManager, displayManager);

    BleManager::bleCharDraw = pService->createCharacteristic(
        CK_BLE_CHAR_UUID_DRAW,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    BleManager::bleCharDraw->setCallbacks(charCallbacks);

    BleManager::bleCharDrawMono = pService->createCharacteristic(
        CK_BLE_CHAR_UUID_DRAW_MONO, BLECharacteristic::PROPERTY_WRITE);
    BleManager::bleCharDrawMono->setCallbacks(charCallbacks);

    BleManager::bleCharConfig = pService->createCharacteristic(
        CK_BLE_CHAR_UUID_CONFIG,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    BleManager::bleCharConfig->setCallbacks(charCallbacks);

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(CK_BLE_SERVICE_UUID);
    // pAdvertising->setScanResponse(false);
    // pAdvertising->setMinPreferred(
    //     0x0); // set value to 0x00 to not advertise this parameter
    // pAdvertising->start();
    BLEDevice::startAdvertising();
};

void BleManager::handle() {
    if(BleManager::postDisconnectionTask.isRunning() &&
       BleManager::postDisconnectionTask.hasPassed(
           POST_DISCONNECTION_TASK_DELAY_MS)) {
        Serial.println("postDisconnectionTask");
        BleManager::postDisconnectionTask.stop();
        BLEDevice::startAdvertising();
    }
}
