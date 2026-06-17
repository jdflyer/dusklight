#include <filesystem>

#include "dusk/assets/assets_unpack.hpp"
#include "dusk/assets/iso.hpp"
#include "dusk/io.hpp"

#include "JSystem/JKernel/JKRDecomp.h"

namespace dusk::assets {

const std::unordered_map<std::string, unpackConvertFunctionType> unpackConvTable = {
    {".iso", iso_unpack},
    // {".arc", arc_unpack},
    // {"speakerse.arc", assets_unpack_convertFunction_None}
};

// const std::unordered_map<std::string, unpackConvertFunctionType> convTable = {
// {"/Audiores/Waves/",jaudio_wave_dir_unpack}
// };

void assets_unpack_check_dir(const std::filesystem::path& path) {}

std::filesystem::path assets_unpack_write(
    const std::filesystem::path& name, const std::span<const u8>& buffer) {
    const std::span<const u8>* outputBuffer = &buffer;
    bool compressed = false;
    std::vector<u8> decompBuffer;
    std::span<const u8> decompBufferSpan;
    if (JKRDecomp::checkCompressed((u8*)buffer.data()) !=
        COMPRESSION_NONE) {  // Gross cast here but that's just how JSystem works lol
        u32 outputSize = JKRDecompExpandSize((u8*)buffer.data());
        decompBuffer.resize(outputSize);
        JKRDecomp::decode((u8*)buffer.data(), decompBuffer.data(), outputSize,
            0);  // I'm not using the function parameters right but it works??
        decompBufferSpan = decompBuffer;
        outputBuffer = &decompBufferSpan;
        compressed = true;
    }

    unpackConvertFunctionType convertFunction = nullptr;

    // Search the table first by full filename, then by extension

    const std::string fullName = name.filename();
    auto it = unpackConvTable.find(fullName);
    if (it != unpackConvTable.end()) {
        convertFunction = it->second;
    }

    const std::string ext = name.extension().string();
    if (convertFunction == nullptr) {
        it = unpackConvTable.find(ext);
        if (it != unpackConvTable.end()) {
            convertFunction = it->second;
        }
    }

    std::string compressedFlag = compressed ? ".c" : "";
    const std::filesystem::path outputPath =
        name.parent_path() / (name.stem().string() + compressedFlag + ext);

    // Convert the file if any candidates
    if (convertFunction != nullptr) {
        return convertFunction(outputPath, *outputBuffer, assets_unpack_write);
    }

    // Otherwise, write the file
    auto fs = dusk::io::FileStream::Create(outputPath);
    fs.Write(outputBuffer->data(), outputBuffer->size());
    return outputPath;
}

const std::filesystem::path assets_unpack_convertFunction_None(
    const std::filesystem::path& outputPath, const std::vector<u8>& buffer,
    writeFunctionType writeFunction) {
    auto fs = dusk::io::FileStream::Create(outputPath);
    fs.Write(buffer.data(), buffer.size());
    return outputPath;
}

int assets_unpack_main(const std::filesystem::path& input, const std::filesystem::path& output) {
    const auto buffer = dusk::io::FileStream::ReadAllBytes(input);
    assets_unpack_write(output, std::span<const u8>(buffer));
    return 0;
}

}  // namespace dusk::assets
