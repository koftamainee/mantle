// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "chunk.h"
#include "core/macros.h"
#include "noise/sampler.h"

namespace spdlog {
    class logger;
}

namespace mantle {
    enum class VoxelType : u16 {
        Air = 0,
        Stone = 1,
        Limestone = 2,
        MossGreen = 3,
        MossLight = 4,
        MossDark = 5,
    };

    class ChunkGenerationSystem final {
      public:
        MANTLE_DEFAULT_INIT(ChunkGenerationSystem);

        void init(u32 seed);
        void destroy();

        void generate(Chunk &chunk, glm::ivec3 pos) const;

      private:
        bool m_is_initialized = false;

        Sampler<f32 (*)(glm::vec3)> m_cave_noise {};
        Sampler<f32 (*)(glm::vec3)> m_warp_noise {};
        Sampler<f32 (*)(glm::vec3)> m_detail_noise {};
        Sampler<f32 (*)(glm::vec3)> m_moss_noise {};
        Sampler<f32 (*)(glm::vec2)> m_floor_noise {};

        spdlog::logger *m_logger = nullptr;
    };
} // namespace mantle
