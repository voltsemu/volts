#pragma once

#include "Components/Audio/Audio.h"

#include <vector>

namespace Volts
{
    struct Emulator
    {
        // get global singleton
        static Emulator* Get();

        std::vector<Audio::Audio*> AudioBackends;
        uint32_t AudioIndex = 0;
        const char** AudioNames = nullptr;
        Audio::Audio* CurrentAudio() const { return AudioBackends[AudioIndex]; }
    };
}