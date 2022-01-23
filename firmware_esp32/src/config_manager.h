#pragma once

#include <Arduino.h>
#include <Preferences.h>

#define PREFERENCE_NAMESPACE "cyberkamen"
#define PREFERENCE_KEY_DEFAULT_COLOR "def_color"

typedef struct rgb24 {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb24_t;

class ConfigManager {
  private:
    Preferences *pref;
    void load(boolean reset = false);

  public:
    rgb24_t defaultColor = {10, 50, 20};

    explicit ConfigManager();
    ~ConfigManager();

    void begin();

    void reset();

    void setDefaultColor(rgb24_t &rgb);
};
