#include "mantle/assets/asset_manager.h"

#include "flecs.h"

namespace mantle {

    SceneHandle         AssetManager::preload(std::string_view scene_uuid) {}
    void                AssetManager::unload(SceneHandle scene) {}
    flecs::entity       AssetManager::instantiate(flecs::world &world, SceneHandle scene) {}
    flecs::entity       AssetManager::instantiate(flecs::world &world, SceneHandle scene,
                                                  const LocalTransform &root_transform) {}
    const MeshData     &AssetManager::get_mesh(MeshHandle handle) const {}
    const MaterialData &AssetManager::get_material(MaterialHandle handle) const {}
    void                AssetManager::init(Renderer &renderer, MemoryBlock block) {}
    void                AssetManager::destroy() {}
    void                AssetManager::flush_uploads(Renderer &renderer) {}
} // namespace mantle
