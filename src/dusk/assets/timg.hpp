#pragma once
#include <filesystem>
#include <span>
#include "JSystem/JUtility/JUTTexture.h"

namespace dusk::assets {

const std::filesystem::path bti_unpack(
    const std::filesystem::path& outputName, const std::span<const u8>& buffer);
const std::vector<u8> bti_pack(const std::filesystem::path& source);

};  // namespace dusk::assets
