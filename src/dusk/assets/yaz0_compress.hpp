#pragma once
#include <span>
#include <vector>

namespace dusk::assets {

std::vector<u8> Yaz0Compress(const std::span<const u8>& src);

}
