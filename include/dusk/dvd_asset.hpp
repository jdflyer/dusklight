#pragma once

#include "dolphin/types.h"
#include "version.hpp"

namespace dusk {

struct DolHeader {
    BE(u32) textOffset[7];
    BE(u32) dataOffset[11];
    BE(u32) textAddr[7];
    BE(u32) dataAddr[11];
    BE(u32) textSize[7];
    BE(u32) dataSize[11];
};

struct OffsetVersion {
    version::GameVersion mGameVersion;
    u32 mOffset;

    constexpr OffsetVersion(const version::GameVersion gameVersion, const u32 offset)
        : mGameVersion(gameVersion), mOffset(offset) {}
};

/**
 * Load bytes from the main DOL by GameCube virtual address
 */
bool LoadDolAsset(void* dst, std::initializer_list<OffsetVersion> virtualAddress, s32 size);

/**
 * Load bytes from a REL file in the ISO filesystem, dst must be 32-byte aligned
 */
bool LoadRelAsset(void* dst, const char* dvdPath, std::initializer_list<OffsetVersion> offset, s32 size);

/**
 * Load bytes from a REL inside RELS.arc
 */
bool LoadArchivedRelAsset(void* dst, u32 memType, const char* relFileName, std::initializer_list<OffsetVersion> offset, s32 size);

}  // namespace dusk
