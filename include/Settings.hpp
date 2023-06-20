#pragma once
#ifndef KNC_SETTINGS_HPP
#define KNC_SETTINGS_HPP
#include <bitset>
#include <fstream>
namespace Settings {
    enum struct Status {
        OK,
        FAIL
    };
    namespace Toggle {
        enum Index {
            FULLSCREEN = 0,
            SOUNDEFFECTS = 1
        };
        bool isFullScren();
        bool isSoundEffectsOn();
        void setFullScreen(bool newVal);
        void setSoundEffectsOn(bool newVal);
        bool load(std::ifstream& optFile);
        bool save(std::ofstream& optFile);
    };
    Status loadSettings();
    Status saveSettings();
}
#endif