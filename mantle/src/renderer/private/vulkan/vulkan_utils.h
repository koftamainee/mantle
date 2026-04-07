#pragma once
#include <string_view>
#include <vector>

#include "core/types.h"

namespace mantle {
    std::vector<u32> load_spv(std::string_view path);
}

