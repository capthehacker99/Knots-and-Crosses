#include <fstream>
#include <bitset>
#include <cstdint>
#include "Settings.hpp"
using u8 = std::uint8_t;
namespace Settings {
    bool dirty;
    namespace Toggle {
        std::bitset<2> internal;
        bool isFullScren() {
            return internal[Index::FULLSCREEN];
        }
        bool isSoundEffectsOn() {
            return internal[Index::SOUNDEFFECTS];
        }
        void setFullScreen(bool newVal) {
            dirty = true;
            internal[Index::FULLSCREEN] = newVal;
        }
        void setSoundEffectsOn(bool newVal) {
            dirty = true;
            internal[Index::SOUNDEFFECTS] = newVal;
        }
        bool load(std::ifstream& optFile) {
            u8 val;
            optFile.read(reinterpret_cast<char*>(&val), 1);
            if(optFile.fail() || optFile.eof())
                return true;
            internal = decltype(internal)(val);
            return false;
        }
        bool save(std::ofstream& optFile) {
            u8 val = static_cast<u8>(internal.to_ulong());
            optFile.write(reinterpret_cast<char*>(&val), sizeof(val));
            if(optFile.bad())
                return true;
            return false;
        }
    };
    Status loadSettings() {
        std::ifstream optFile("options.db", std::ios::binary | std::ios::in);
        if (!optFile.is_open()) {
            return Status::FAIL;
        }
        if(Toggle::load(optFile)) {
            optFile.close();
            return Status::FAIL;
        }
        optFile.close();
        dirty = false;
        return Status::OK;
    }
    Status saveSettings() {
        if(!dirty)
            return Status::OK;
        std::ofstream optFile("options.db", std::ios::binary | std::ios::out);
        if (!optFile.is_open()) {
            return Status::FAIL;
        }
        if(Toggle::save(optFile)) {
            optFile.close();
            return Status::FAIL;
        }
        optFile.close();
        dirty = false;
        return Status::OK;
    }
};