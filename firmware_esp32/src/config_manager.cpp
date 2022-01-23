#include "config_manager.h"

ConfigManager::ConfigManager() { pref = new Preferences(); }
ConfigManager::~ConfigManager() { delete pref; }

void ConfigManager::begin() { load(); }

void ConfigManager::load(boolean reset) {
    pref->begin(PREFERENCE_NAMESPACE, false);

    // Reset if required
    if(reset) {
        pref->putBytes(PREFERENCE_KEY_DEFAULT_COLOR, &defaultColor,
                       sizeof(rgb24_t));
    }

    // Load values from preference
    size_t len = pref->getBytes(PREFERENCE_KEY_DEFAULT_COLOR, &defaultColor,
                                sizeof(rgb24_t));
    if(!len) {
        Serial.println("Failed to load PREFERENCE_KEY_DEFAULT_COLOR.");
    }

    pref->end();
}

void ConfigManager::reset() {
    pref->begin(PREFERENCE_NAMESPACE, false);
    pref->clear();
    pref->end();

    load(true);
}

void ConfigManager::setDefaultColor(rgb24_t &rgb) {
    memcpy(&defaultColor, &rgb, sizeof(rgb24_t));
    pref->begin(PREFERENCE_NAMESPACE, false);
    pref->putBytes(PREFERENCE_KEY_DEFAULT_COLOR, &defaultColor,
                   sizeof(rgb24_t));
    pref->end();
}
