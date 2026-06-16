#pragma once

#include <cstdint>

#include "mantle/core/types.h"
#include "mantle/renderer/types.h"

#include <vector>

namespace mantle {

MANTLE_HANDLE(MeshHandle);
MANTLE_HANDLE(MaterialHandle);
MANTLE_HANDLE(SceneHandle);

struct SubMeshInfo {
    u32   vertex_count;
    u32   vertex_offset;
    u16   vertex_stride;
    u32   index_count;
    u32   index_offset;
    u32   attr_flags;
    float bounding_min[3];
    float bounding_max[3];
};

struct MeshData {
    BufferHandle vertex_buffer;
    BufferHandle index_buffer;
    u32          vertex_count;
    u32          index_count;
    std::pmr::vector<SubMeshInfo> submeshes;
};

struct MaterialData {
    float    base_color[4];
    float    metallic;
    float    roughness;
    float    emissive[3];
    float    alpha_cutoff;
    float    normal_scale;
    float    emissive_strength;
    uint32_t flags;
};

} // namespace mantle
