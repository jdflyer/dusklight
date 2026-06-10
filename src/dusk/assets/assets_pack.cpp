#include "dusk/assets/assets_pack.hpp"
#include <unordered_map>
#include "dusk/assets/iso.hpp"
#include "dusk/io.hpp"

namespace dusk::assets {

const std::unordered_map<std::string, packDef> packConvTable = {
    {".iso", {iso_pack, ".iso", true}},
    // {".arc", arc_pack},
    // {"speakerse.arc", assets_pack_convertFunction_None}
};

// const std::unordered_map<std::string, packConvertFunctionType> dirPackConvTable = {
// {"/Audiores/Waves/",jaudio_wave_dir_pack}
// };

bool isSourceNewer(const std::filesystem::path& sourcePath, const std::filesystem::path& dstPath) {
    if (!std::filesystem::exists(dstPath)) {
        return true;
    }
    bool sourceIsDirectory = std::filesystem::is_directory(sourcePath);
    bool dstIsDirectory = std::filesystem::is_directory(dstPath);
    if (!sourceIsDirectory && !dstIsDirectory) {
        return std::filesystem::last_write_time(sourcePath) >
               std::filesystem::last_write_time(dstPath);
    }
    if (sourceIsDirectory && !dstIsDirectory) {
        auto dstMTime = std::filesystem::last_write_time(dstPath);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(sourcePath)) {
            if (entry.is_regular_file() && entry.last_write_time() > dstMTime) {
                return true;
            }
        }
        return false;
    }
    if (!sourceIsDirectory && dstIsDirectory) {
        auto srcMTime = std::filesystem::last_write_time(sourcePath);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dstPath)) {
            if (entry.is_regular_file() && srcMTime > entry.last_write_time()) {
                return true;
            }
        }
        return false;
    }
    // In the case both are directories, get the max mtime from a recursive search and compare
    std::filesystem::file_time_type srcMTime;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(sourcePath)) {
        if (entry.is_regular_file()) {
            srcMTime = std::max(srcMTime, entry.last_write_time());
        }
    }
    std::filesystem::file_time_type dstMTime;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dstPath)) {
        if (entry.is_regular_file()) {
            dstMTime = std::max(srcMTime, entry.last_write_time());
        }
    }
    return srcMTime > dstMTime;
}

std::filesystem::path assets_pack_convert_entry(
    const std::filesystem::path& sourcePath, const std::filesystem::path& dstPathOriginal, std::vector<u8>* output) {
    bool isDir = std::filesystem::is_directory(sourcePath);

    std::filesystem::path outputPath = dstPathOriginal;
    bool doCompress = false;
    if (outputPath.stem().extension() == ".c") {
        doCompress = true;
        outputPath = outputPath.parent_path() /
                     std::filesystem::path(
                         outputPath.stem().stem().string() + outputPath.extension().string());
    }

    packConvertBinaryFunctionType convFunction = nullptr;
    std::string newExtension = outputPath.extension();

    // First, search all explicit names
    auto it = packConvTable.find(sourcePath.filename());
    if (it != packConvTable.end()) {
        if (!isDir || (isDir && it->second.sourceIsDir)) {
            convFunction = it->second.convFunction;
        }
        newExtension = it->second.dstExtension;
    }

    // Next, search by extension
    if (convFunction == nullptr) {
        it = packConvTable.find(sourcePath.extension().string());
        if (it != packConvTable.end()) {
            if (!isDir || (isDir && it->second.sourceIsDir)) {
                convFunction = it->second.convFunction;
            }
            newExtension = it->second.dstExtension;
        }
    }

    if (isDir && convFunction == nullptr) {
        return outputPath;
    }

    outputPath = outputPath.parent_path() / std::filesystem::path(outputPath.stem().string() + newExtension);

    printf("Converting %s -> %s\n",sourcePath.c_str(),outputPath.c_str());
    
    // bool doConvert = isSourceNewer(sourcePath, outputPath);
    // if (!doConvert) {
    //     return outputPath;
    // }

    std::vector<u8> defaultOutput;
    std::vector<u8>* outputBuffer= &defaultOutput;
    if (output != nullptr) {
        outputBuffer = output;
    }
    if (convFunction != nullptr) {
        *outputBuffer = convFunction(sourcePath);
    }

    if (doCompress) {
        if (convFunction == nullptr) {
            *outputBuffer = dusk::io::FileStream::ReadAllBytes(sourcePath);
        }
        // yaz0 compress here
    }

    if (output != nullptr) {
        if (output->size() == 0) {
            *output = dusk::io::FileStream::ReadAllBytes(sourcePath);
        }
    }else {
        if (!std::filesystem::exists(outputPath.parent_path())) {
            std::filesystem::create_directories(outputPath.parent_path());
        }
        if (convFunction == nullptr && doCompress == false) {
            std::filesystem::copy(sourcePath,outputPath);
        }else {
            auto fs = dusk::io::FileStream::Create(outputPath);
            fs.Write(*outputBuffer);
        }
    }

    return outputPath;
}

void assets_pack_copy_recurse(
    const std::filesystem::path& input, const std::filesystem::path& output) {
    auto entries = getSortedFileList(input);
    for (const auto& entry : entries) {
        const auto relative = std::filesystem::relative(entry.path(), input);

        if (!entry.is_directory()) {
            assets_pack_convert_entry(entry.path(), output / relative, nullptr);
            continue;
        }

        auto it = packConvTable.find(entry.path().extension().string());
        if (it != packConvTable.end()) {
            assets_pack_convert_entry(entry.path(), output / relative, nullptr);
            continue;
        }

        if (!std::filesystem::exists(output/relative)) {
            std::filesystem::create_directories(output/relative);
        }

        assets_pack_copy_recurse(entry.path(), output / relative);
    }
}

int assets_pack_main(const std::filesystem::path& input, const std::filesystem::path& output) {
    auto it = packConvTable.find(input.extension().string());
    if (it != packConvTable.end()) {
        // If argument is an asset, convert it right away
        assets_pack_convert_entry(input, output, nullptr);
    } else {
        assets_pack_copy_recurse(input, output);
    }
    return 0;
}

}  // namespace dusk::assets
