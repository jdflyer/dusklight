#pragma once
#include <filesystem>

namespace dusk::assets {

// using packConvertEntryFunctionType = std::function<const std::filesystem::path(const
// std::filesystem::path& sourcePath, const std::filesystem::path& dstPathOriginal)>;
using packConvertBinaryFunctionType =
    std::function<const std::vector<u8>(const std::filesystem::path& source)>;

struct packDef {
    packConvertBinaryFunctionType convFunction;
    std::string dstExtension;
    bool sourceIsDir = false;
};

extern const std::unordered_map<std::string, packDef> packConvTable;

inline std::vector<std::filesystem::directory_entry> getSortedFileList(
    const std::filesystem::path& path) {
    std::vector<std::filesystem::directory_entry> entries(
        std::filesystem::directory_iterator(path), std::filesystem::directory_iterator{});
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        // This ordering matches the original tools (lowercase compare with _ being sorted last)
        auto an = a.path().filename().string();
        auto bn = b.path().filename().string();
        return std::lexicographical_compare(
            an.begin(), an.end(), bn.begin(), bn.end(), [](char a, char b) {
                auto rank = [](char c) -> int {
                    if (c == '_')
                        return 'z' + 1;
                    return std::tolower(c);
                };
                return rank(a) < rank(b);
            });
    });
    return entries;
}

std::filesystem::path assets_pack_convert_entry(const std::filesystem::path& sourcePath,
    const std::filesystem::path& dstPathOriginal, std::vector<u8>* output);
int assets_pack_main(const std::filesystem::path& input, const std::filesystem::path& output);

}  // namespace dusk::assets
