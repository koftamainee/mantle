// Copyright 2026 Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "mantle/core/types.h"
#include "mantle/renderer/types.h"

#include <cstdint>
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
        BufferHandle                  vertex_buffer;
        BufferHandle                  index_buffer;
        u32                           vertex_count;
        u32                           index_count;
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
