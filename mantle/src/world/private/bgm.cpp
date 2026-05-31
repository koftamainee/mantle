#include "bgm.h"

#include <bit>

#include "core/assert.h"
#include "world/face_mask.h"

namespace mantle::bgm {

    namespace {

        constexpr u32 CHUNK_SIZE = Chunk::size;
        constexpr f32 VOXEL_SCALE = 0.1f;


        u32 voxel_index(u32 axis, u32 a, u32 u, u32 v) noexcept {
            switch (axis) {
            case 0:
                return a + u * 32 + v * 1024;
            case 1:
                return u + a * 32 + v * 1024;
            default:
                return u + v * 32 + a * 1024;
            }
        }

        u16 sample_voxel(const Chunk &chunk, const Chunk *neighbors[6],
                         u32 axis, i32 a, u32 u, u32 v) noexcept {
            if (a >= 0 && a < static_cast<i32>(CHUNK_SIZE)) {
                auto idx = voxel_index(axis, static_cast<u32>(a), u, v);
                return chunk.voxels[idx];
            }

            u32 nb_idx = (axis * 2) + (a < 0 ? 0u : 1u);
            const Chunk *nb = neighbors[nb_idx];
            if (!nb) {
                return 0;
            }

            u32 na = (a < 0) ? CHUNK_SIZE - 1 : 0;
            return nb->voxels[voxel_index(axis, na, u, v)];
        }

        void build_solid_masks(const Chunk &chunk, u32 axis,
                                      FaceMask solid[CHUNK_SIZE]) noexcept {
            for (u32 a = 0; a < CHUNK_SIZE; a++) {
                for (u32 v = 0; v < CHUNK_SIZE; v++) {
                    u32 bits = 0;
                    for (u32 u = 0; u < CHUNK_SIZE; u++) {
                        if (chunk.voxels[voxel_index(axis, a, u, v)] != 0) {
                            bits |= (1u << u);
                        }
                    }
                    solid[a].rows[v] = bits;
                }
            }
        }

        void build_face_masks(const Chunk *neighbors[], u32 axis, u32 positive,
                              const FaceMask solid[],
                              FaceMask face[]) noexcept {
            for (u32 a = 0; a < CHUNK_SIZE; a++) {
                i32 nb_a = static_cast<i32>(positive ? a + 1 : a - 1);

                FaceMask neighbor_mask = {};
                if (nb_a >= 0 && nb_a < static_cast<i32>(CHUNK_SIZE)) {
                    neighbor_mask = solid[nb_a];
                } else {
                    u32 nb_idx = axis * 2 + positive;
                    if (const Chunk *nb = neighbors[nb_idx]) {
                        u32 na = positive ? 0 : CHUNK_SIZE - 1;
                        for (u32 v = 0; v < CHUNK_SIZE; v++) {
                            u32 r = 0;
                            for (u32 u = 0; u < CHUNK_SIZE; u++) {
                                if (nb->voxels[voxel_index(axis, na, u, v)] !=
                                    0)
                                    r |= (1u << u);
                            }
                            neighbor_mask.rows[v] = r;
                        }
                    }
                }

                for (u32 v = 0; v < CHUNK_SIZE; v++) {
                    face[a].rows[v] = solid[a].rows[v] & ~neighbor_mask.rows[v];
                }
            }
        }

        glm::vec3 to_world_offset(u32 axis, f32 a, f32 u,
                                         f32 v) noexcept {
            switch (axis) {
            case 0:
                return {a, u, v};
            case 1:
                return {u, a, v};
            default:
                return {u, v, a};
            }
        }

        void emit_quad(ChunkMeshData &out, glm::vec3 chunk_world_origin,
                              u32 axis, u32 positive, u32 a, u32 u0, u32 v0,
                              u32 width, u32 height, u8 material) noexcept {
            u32 face_index = axis * 2 + positive;
            u32 packed = face_index | (static_cast<u32>(material) << 8u);

            f32 depth = static_cast<f32>(a) + (positive ? 1.f : 0.f);
            f32 W = static_cast<f32>(width);
            f32 H = static_cast<f32>(height);

            bool ccw = positive;
            if (axis == 1) {
                ccw = !ccw;
            }

            glm::vec3 corners[4];
            if (ccw) {
                corners[0] = to_world_offset(axis, depth, static_cast<f32>(u0), static_cast<f32>(v0));
                corners[1] = to_world_offset(axis, depth, static_cast<f32>(u0) + W, static_cast<f32>(v0));
                corners[2] =
                    to_world_offset(axis, depth, static_cast<f32>(u0) + W, static_cast<f32>(v0) + H);
                corners[3] = to_world_offset(axis, depth, static_cast<f32>(u0), static_cast<f32>(v0) + H);
            } else {
                corners[0] = to_world_offset(axis, depth, static_cast<f32>(u0), static_cast<f32>(v0));
                corners[1] = to_world_offset(axis, depth, static_cast<f32>(u0), static_cast<f32>(v0) + H);
                corners[2] =
                    to_world_offset(axis, depth, static_cast<f32>(u0) + W, static_cast<f32>(v0) + H);
                corners[3] = to_world_offset(axis, depth, static_cast<f32>(u0) + W, static_cast<f32>(v0));
            }

            u32 base_vertex = out.quad_count * 4;
            for (u32 i = 0; i < 4; i++) {
                glm::vec3 world = chunk_world_origin + corners[i] * VOXEL_SCALE;
                out.vertices[base_vertex + i] = {world.x, world.y, world.z,
                                                 packed};
            }

            u32 *idx = out.indices + out.quad_count * 6;
            idx[0] = base_vertex + 0;
            idx[1] = base_vertex + 1;
            idx[2] = base_vertex + 2;
            idx[3] = base_vertex + 0;
            idx[4] = base_vertex + 2;
            idx[5] = base_vertex + 3;

            out.quad_count++;
        }

        void greedy_merge_slice(FaceMask &mask, ChunkMeshData &out,
                                       glm::vec3 chunk_world_origin,
                                       const Chunk &chunk,
                                       const Chunk *neighbors[6], u32 axis,
                                       u32 positive, u32 depth) noexcept {
            for (u32 v = 0; v < CHUNK_SIZE; v++) {
                u32 bits = mask.rows[v];
                while (bits != 0) {
                    u32 u0 = static_cast<u32>(std::countr_zero(bits));
                    u32 shift = bits >> u0;
                    u32 width = static_cast<u32>(std::countr_zero(~shift));
                    u32 strip = static_cast<u32>(((1ull << width) - 1ull) << u0);

                    u32 height = 1;
                    while (v + height < CHUNK_SIZE &&
                           (mask.rows[v + height] & strip) == strip) {
                        mask.rows[v + height] &= ~strip;
                        height++;
                    }

                    bits &= ~strip;
                    mask.rows[v] = bits;

                    i32 na = static_cast<i32>(depth) - (positive ? 1 : 0);
                    u16 vox_val =
                        sample_voxel(chunk, neighbors, axis, na, u0, v);
                    u8 material = vox_val != 0 ? static_cast<u8>(vox_val) : static_cast<u8>(1);
                    emit_quad(out, chunk_world_origin, axis, positive, depth,
                              u0, v, width, height, material);
                }
            }
        }

    } // anonymous namespace

    ChunkMeshData mesh_chunk(const Chunk &chunk, const Chunk *neighbors[6],
                             glm::ivec3 chunk_pos, ArenaAllocator &scratch) {
        constexpr u32 MAX_QUADS = 8192;

        ChunkMeshData out;
        out.vertices = scratch.push<MeshVertex>(MAX_QUADS * 4);
        out.indices = scratch.push<u32>(MAX_QUADS * 6);
        out.quad_count = 0;

        glm::vec3 chunk_world_origin =
            glm::vec3(chunk_pos) * static_cast<f32>(CHUNK_SIZE) * VOXEL_SCALE;

        auto *solid = scratch.push_zeroed<FaceMask>(CHUNK_SIZE);
        auto *face = scratch.push_zeroed<FaceMask>(CHUNK_SIZE);

        for (u32 axis = 0; axis < 3; axis++) {
            build_solid_masks(chunk, axis, solid);

            for (u32 positive = 0; positive <= 1; positive++) {
                build_face_masks(neighbors, axis, positive, solid, face);

                for (u32 a = 0; a < CHUNK_SIZE; a++) {
                    if (face[a].empty())
                        continue;
                    greedy_merge_slice(face[a], out, chunk_world_origin, chunk,
                                       neighbors, axis, positive, a);
                }
            }
        }

        return out;
    }

} // namespace mantle::bgm
