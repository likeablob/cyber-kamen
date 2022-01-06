#include <Chrono.h>
Chrono blinkTask;

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 1
#define NUMPIXELS 64

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint16_t hue = 200;
int16_t saturation = 200;
int16_t brightness = 10;

const uint8_t face1[] = {0b0,        0b11000011, 0b11000011, 0b11000011,
                         0b11000011, 0b11000011, 0b11000011, 0b0};
const uint8_t blink_frame_0[] = {0b0,        0b0,        0b11000011, 0b11000011,
                                 0b11000011, 0b11000011, 0b0,        0b0};
const uint8_t blink_frame_1[] = {0b0,        0b0, 0b0, 0b11000011,
                                 0b11000011, 0b0, 0b0, 0b0};
const uint8_t blink_frame_2[] = {0b0, 0b0, 0b0, 0b11000011, 0b0, 0b0, 0b0, 0b0};

const uint8_t face2[] = {0b10000001, 0b01000010, 0b00100100, 0b00011000,
                         0b00011000, 0b00100100, 0b01000010, 0b10000001};

void drawFrame(const uint8_t frame[]) {
    for(int i = 0; i < NUMPIXELS; i++) {
        uint8_t row = frame[i / 8];
        uint8_t rowInd = i % 8;
        if(row % 2) { // reversing for the Zig-zag alignment
            rowInd = 7 - rowInd;
        }
        uint8_t bit = (row >> (7 - rowInd)) & 0b1;
        pixels.setPixelColor(i, pixels.ColorHSV(hue, saturation, bit ? 20 : 0));
    }
}

void randomColors() {
    for(int i = 0; i < NUMPIXELS; i++) {
        pixels.setPixelColor(
            i, pixels.ColorHSV(rand() % UINT16_MAX, 200 + rand() % 50, 100));
    }
    pixels.show();
}

void blink() {
    const uint8_t *frames[] = {blink_frame_0, blink_frame_1, blink_frame_2,
                               NULL,          blink_frame_1, blink_frame_0};

    for(auto &&i : frames) {
        if(i == NULL) {
            pixels.clear();
            continue;
        }
        drawFrame(i);
        pixels.show();
        delay(30);
    }
}

void setup() {
    clock_prescale_set(clock_div_1);
    pixels.begin();

    for(int i = 0; i < NUMPIXELS; i++) {
        uint16_t hue_ = rand() % UINT16_MAX;
        for(size_t j = 0; j < brightness; j += 3) {
            pixels.setPixelColor(i, pixels.ColorHSV(hue_, saturation, j));
            if(i) {
                pixels.setPixelColor(i - 1,
                                     pixels.ColorHSV(hue_, saturation, 0));
            }
            pixels.show();
            delay(5);
        }
    }
}

void loop() {
    hue = hue + 1 % 655535;

    drawFrame(face1);
    pixels.show();

    if(blinkTask.hasPassed(2000)) {
        blinkTask.restart();
        blink();
    }
}
