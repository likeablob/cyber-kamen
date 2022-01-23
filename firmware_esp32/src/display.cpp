#include "display.h"
#include "raw_frames.h"

DisplayManager::DisplayManager(ConfigManager *configManager)
    : configManager(configManager) {
    matrix = new Adafruit_NeoMatrix(8, 8, PIN_WS2812,
                                    NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
                                        NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
                                    NEO_GRB + NEO_KHZ800);
    blinkTask = new Chrono();
    lazyDrawTask = new Chrono();
    lazyDrawTask->stop();
}

DisplayManager::~DisplayManager() {
    delete matrix;
    delete blinkTask;
    delete lazyDrawTask;
}

void DisplayManager::begin() {
    matrix->begin();
    matrix->setRotation(2);
    matrix->setBrightness(100);

    blinkTask->start();
};

void DisplayManager::handle() {
    if(blinkTask->hasPassed(2000 + rand() % 500 - 250)) {
        Serial.printf("blink at %ld\n", millis());
        blinkTask->restart();
        blink();
    }

    if(lazyDrawTask->isRunning()) {
        lazyDrawTask->stop();
        drawFrameFromBuffer();
    }
}

void DisplayManager::writeBuffer(const uint8_t *frame, bool lazyDraw) {
    writeBuffer(frame, configManager->defaultColor, lazyDraw);
}

void DisplayManager::writeBuffer(const uint8_t *frame, rgb24_t color,
                                 bool lazyDraw) {
    writeBuffer(frame, Adafruit_NeoPixel::Color(color.r, color.g, color.b),
                lazyDraw);
}

void DisplayManager::writeBuffer(const uint8_t *frame, uint32_t color,
                                 bool lazyDraw) {
    for(size_t i = 0; i < NUMPIXELS; i++) {
        uint8_t row = frame[i / DISP_W];
        uint8_t bitInd = i % 8;
        uint8_t bit = (row >> (7 - bitInd)) & 0b1;
        Serial.println(buff[i], HEX);
        buff[i] = bit ? color : 0;
    }
    if(lazyDraw) {
        lazyDrawTask->start();
    }
}

void DisplayManager::writeBuffer(const uint32_t *frame, bool lazyDraw) {
    memcpy(buff, frame, NUMPIXELS);
    if(lazyDraw) {
        lazyDrawTask->start();
    }
}

void DisplayManager::drawFrameFromBuffer() { drawFrame(buff); }

void DisplayManager::drawFrame(const uint8_t *frame) {
    drawFrame(frame, configManager->defaultColor);
}

void DisplayManager::drawFrame(const uint8_t *frame, rgb24_t color) {
    for(size_t i = 0; i < NUMPIXELS; i++) {
        uint16_t x = i % DISP_H;
        uint16_t y = i / DISP_H;
        uint8_t row = frame[y];
        uint8_t bitInd = 7 - i % 8;
        uint8_t bit = (row >> bitInd) & 0b1;
        matrix->setPassThruColor(
            bit ? Adafruit_NeoPixel::Color(color.r, color.g, color.b) : 0);
        matrix->drawPixel(x, y, 0);
        matrix->setPassThruColor();
    }
    matrix->show();
};

void DisplayManager::drawFrame(const uint32_t *frame) {
    for(size_t i = 0; i < NUMPIXELS; i++) {
        uint16_t x = i % DISP_W;
        uint16_t y = i / DISP_H;
        matrix->setPassThruColor(frame[i]);
        matrix->drawPixel(x, y, 0);
        matrix->setPassThruColor();
    }
    matrix->show();
};

void DisplayManager::blink() {
    const uint8_t *frames[] = {blink_frame_0, blink_frame_1, blink_frame_2,
                               NULL,          blink_frame_1, blink_frame_0};

    for(auto &&i : frames) {
        if(i == NULL) {
            matrix->clear();
            continue;
        }
        drawFrame(i);
        delay(30);
    }
    drawFrame(default_face);
};

void DisplayManager::bootAnim() {
    for(int i = 0; i < NUMPIXELS; i++) {
        matrix->setPixelColor(
            i, matrix->ColorHSV(rand() % UINT16_MAX, 200 + rand() % 50, 2));
        matrix->show();
        delay(1);
    }
    matrix->clear();
};

void DisplayManager::stopBlink() {
    blinkTask->stop();
    zerofill();
    matrix->show();
}

void DisplayManager::startBlink() { blinkTask->restart(UINT16_MAX); }

void DisplayManager::zerofill() { matrix->fill(0); }
