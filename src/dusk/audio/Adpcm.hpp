#ifndef DUSK_ADPCM_HPP
#define DUSK_ADPCM_HPP

#include <dolphin/types.h>

namespace dusk::audio {
    constexpr u32 Adpcm4FrameSize = 9;
    constexpr u32 AdpcmSampleCount = 16;
    constexpr u32 Adpcm2FrameSize = 5;

    // https://github.com/magcius/vgmtrans/blob/8e34ddc2fb43948dc1e1a8759c739a0c1c7b62d7/src/main/formats/JaiSeqScanner.cpp#L489-L531
    // https://github.com/XAYRGA/JaiSeqX/blob/f29c024ec3663503f506aa02bcd503ada6e7d8aa/JaiSeqXLJA/DSP/JAIDSPADPCM4.cs#L86-L87

    constexpr u16 Coefficient0[] = {
        0,0x0800,0,0x0400,
        0x1000,0x0e00,0x0c00,0x1200,
        0x1068,0x12c0,0x1400,0x0800,
        0x0400,0xfc00,0xfc00,0xf800
    };

    constexpr u16 Coefficient1[] = {
        0,0,0x0800,0x0400,0xf800,
        0xfa00,0xfc00,0xf600,0xf738,
        0xf704,0xf400,0xf800,0xfc00,
        0x0400,0,0,
    };

    constexpr int AdpcmFrameSize = 9;

    inline s16 Clamp16(s32 value) {
        if (value > 0x7FFF) return 0x7FFF;
        if (value < -0x8000) return -0x8000;
        return value;
    }

    void Adpcm4ToPcm16(const u8* adpcm, size_t adpcmLength, s16* pcm, size_t pcmLength, s16& hist1, s16& hist0);
}

#endif  // DUSK_ADPCM_HPP
