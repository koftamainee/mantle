#pragma once

#include <string_view>

#include "mantle/assets/types.h"
#include "mantle/core/memory/memory_block.h"

namespace flecs {
    class world;
    class entity;
} // namespace flecs

namespace mantle {

    struct LocalTransform;
    class Renderer;

    class AssetManager {
        friend class Engine;

      public:
        AssetManager() = default;

        MANTLE_NO_COPY_NO_MOVE(AssetManager);

        SceneHandle preload(std::string_view scene_uuid);
        void        unload(SceneHandle scene);

        flecs::entity instantiate(flecs::world &world, SceneHandle scene);
        flecs::entity instantiate(flecs::world &world, SceneHandle scene,
                                  const LocalTransform &root_transform);

        const MeshData     &get_mesh(MeshHandle handle) const;
        const MaterialData &get_material(MaterialHandle handle) const;

      private:
        void init(Renderer &renderer, MemoryBlock block);
        void destroy();
        void flush_uploads(Renderer &renderer);

        bool m_is_initialized = false;
        struct Impl;
        Impl *m_impl = nullptr;
    };


} // namespace mantle
