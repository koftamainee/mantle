#pragma once
#include <cstdint>
#include <mdspan>
#include "voxel.h"

namespace mantle {
    class Chunk final {
      public:
        static constexpr uint32_t s_chunk_size = 16;
        using VoxelArray =
            std::array<Voxel, s_chunk_size * s_chunk_size * s_chunk_size>;
        using Extents =
            std::extents<uint32_t, s_chunk_size, s_chunk_size, s_chunk_size>;
        using VoxelSpan = std::mdspan<Voxel, Extents>;
        using ConstVoxelSpan = std::mdspan<const Voxel, Extents>;

        VoxelSpan voxels();
        ConstVoxelSpan voxels() const;
        VoxelArray &voxel_array();

      private:
        VoxelArray m_voxels{};
    };
} // namespace mantle
