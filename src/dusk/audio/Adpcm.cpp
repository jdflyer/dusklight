#include "Adpcm.hpp"

#include <cassert>
#include <fstream>

#include "JSystem/JAudio2/JASAramStream.h"


void dusk::audio::Adpcm4ToPcm16(const u8* adpcm, size_t adpcmLength, s16* pcm, size_t pcmLength, s16& hist2, s16& hist1) {
    assert (adpcmLength % AdpcmFrameSize == 0 && "ADPCM must be divisible by frame size");

    auto endPtr = pcm + pcmLength;

    for (int i = 0; i < adpcmLength; i += AdpcmFrameSize) {
        u8 header = adpcm[i];

        s32 scale = 1 << (header >> 4);
        u8 coefIndex = header & 0xF;

        s16 coef0 = (s16)Coefficient0[coefIndex];
        s16 coef1 = (s16)Coefficient1[coefIndex];

        for (int sampleIdx = 0; sampleIdx < 16; sampleIdx++) {
            u8 adpcmValue = adpcm[i + 1 + sampleIdx / 2];
            u8 unsignedNibble = sampleIdx % 2 == 0 ? adpcmValue >> 4 : adpcmValue & 0xF;
            s8 signedNibble = ((s8)(unsignedNibble << 4)) >> 4;
            s16 sample = Clamp16((((signedNibble * scale) << 11) + (coef0 * hist1 + coef1 * hist2)) >> 11);

            hist2 = hist1;
            hist1 = sample;

            *pcm++ = sample;
            if (endPtr == pcm)
                return;
        }
    }
}
