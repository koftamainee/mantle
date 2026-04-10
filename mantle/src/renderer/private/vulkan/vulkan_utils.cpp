#include "../vulkan/vulkan_utils.h"

#include <fstream>
#include <ios>

#include "core/assert.h"

namespace mantle {
    void load_spv(std::string_view path, std::pmr::vector<u32> &out) {
        std::ifstream file(path.data(), std::ios::binary | std::ios::ate);
        check(file.is_open());

        long size = file.tellg();
        check(size % 4 == 0);

        file.seekg(0);
        out.resize(size / 4);
        file.read(reinterpret_cast<char *>(out.data()), size);
    }
} // namespace mantle
