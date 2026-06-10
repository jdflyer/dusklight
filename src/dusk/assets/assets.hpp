#pragma once
#include <sstream>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"

namespace dusk::assets {

// Helper for JSON
template <typename T>
std::string toHex(T value) {
    std::ostringstream ss;
    ss << "0x" << std::uppercase << std::hex << static_cast<u32>(value);
    return ss.str();
}

inline u32 fromHex(const std::string& value) {
    return std::stoul(value, nullptr, 16);
}

inline std::string getKeyString(const nlohmann::json& j, const std::string& key) {
    return j.at(key).get<std::string>();
}
inline u32 getKeyHex(const nlohmann::json& j, const std::string& key) {
    return fromHex(getKeyString(j, key));
}

}  // namespace dusk::assets
