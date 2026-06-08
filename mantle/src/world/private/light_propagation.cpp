// Copyright (c) 2026 Mantle. All rights reserved.

#include "world/light_propagation.h"

#include <cstring>

namespace mantle {

    void init_chunk_light(Chunk &chunk) {
        std::memset(chunk.light, 0, Chunk::volume);

        for (u32 i = 0; i < Chunk::volume; i++) {
            u16 v = chunk.voxels[i];
            if (v >= 3 && v <= 5) {
                chunk.light[i] = 8;
            }
        }
    }

    void propagate_chunk_light_level(Chunk &chunk, Chunk *neighbors[6], u8 level) {
        constexpr u32 S = Chunk::size;
        constexpr u32 S2 = S * S;
        constexpr u32 S_M1 = S - 1;
        constexpr u32 SxS_M1 = S * S_M1;

        u8 next = level - 1;

        for (u32 i = 0; i < Chunk::volume; i++) {
            if (chunk.light[i] != level) {
                continue;
            }

            u32 x = i & S_M1;
            u32 y = (i >> 5) & S_M1;
            u32 z = i >> 10;

            if (x + 1 < S) {
                u32 ni = i + 1;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[1]) {
                u32 ni = i - S_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }

            if (x > 0) {
                u32 ni = i - 1;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[0]) {
                u32 ni = i + S_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }

            if (y + 1 < S) {
                u32 ni = i + S;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[3]) {
                u32 ni = i - SxS_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }

            if (y > 0) {
                u32 ni = i - S;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[2]) {
                u32 ni = i + SxS_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }

            if (z + 1 < S) {
                u32 ni = i + S2;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[5]) {
                u32 ni = i - S2 * S_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }

            if (z > 0) {
                u32 ni = i - S2;
                if (chunk.voxels[ni] == 0 && next > chunk.light[ni]) {
                    chunk.light[ni] = next;
                }
            } else if (Chunk *nb = neighbors[4]) {
                u32 ni = i + S2 * S_M1;
                if (nb->voxels[ni] == 0 && next > nb->light[ni]) {
                    nb->light[ni] = next;
                }
            }
        }
    }

} // namespace mantle
