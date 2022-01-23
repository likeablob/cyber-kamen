#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <Adafruit_NeoMatrix.h>
#include <Chrono.h>

#include "config_manager.h"

#define PIN_WS2812 26
#define DISP_W 8
#define DISP_H 8
#define NUMPIXELS (DISP_W * DISP_H)

class DisplayManager {
  protected:
  public:
    Adafruit_NeoMatrix *matrix;
    Chrono *blinkTask;
    Chrono *lazyDrawTask;
    uint32_t buff[NUMPIXELS];
    ConfigManager *configManager;

    explicit DisplayManager(ConfigManager *configManager);
    ~DisplayManager();

    void begin();
    void handle();

    void drawFrame(const uint8_t *frame, rgb24_t color);
    void drawFrame(const uint8_t *frame);
    void drawFrame(const uint32_t *frame);
    void writeBuffer(const uint32_t *frame, bool lazyDraw);
    void writeBuffer(const uint8_t *frame, bool lazyDraw);
    void writeBuffer(const uint8_t *frame, rgb24_t color, bool lazyDraw);
    void writeBuffer(const uint8_t *frame, uint32_t color, bool lazyDraw);
    void drawFrameFromBuffer();
    void zerofill();

    void bootAnim();

    void blink();
    void stopBlink();
    void startBlink();
};
