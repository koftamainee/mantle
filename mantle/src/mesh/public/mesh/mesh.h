#pragma once
#include <cstdint>
#include <vector>
#include "core/types.h"

#include "vertex.h"

namespace mantle {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
    };
} // namespace mantle
