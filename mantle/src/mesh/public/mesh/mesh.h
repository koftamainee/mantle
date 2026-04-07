#pragma once
#include <vector>
#include <cstdint>
#include "core/types.h"

#include "vertex.h"

namespace mantle {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
    };
}