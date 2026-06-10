#pragma once
#include <filesystem>
#include <functional>
#include <span>

namespace dusk::assets {

using writeFunctionType = std::function<const std::filesystem::path(
    const std::filesystem::path& outputName, const std::span<const u8>& buffer)>;
using unpackConvertFunctionType =
    std::function<const std::filesystem::path(const std::filesystem::path& outputPath,
        const std::span<const u8>& buffer, writeFunctionType writeFunction)>;

const std::filesystem::path assets_unpack_convertFunction_None(
    const std::filesystem::path& outputPath, const std::span<u8>& buffer,
    writeFunctionType writeFunction);
int assets_unpack_main(const std::filesystem::path& input, const std::filesystem::path& output);
void assets_unpack_check_dir(const std::filesystem::path& path);

}  // namespace dusk::assets
