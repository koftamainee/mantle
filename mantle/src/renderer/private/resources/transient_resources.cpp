#include "transient_resources.h"

#include "core/assert.h"
#include "renderer/frame_graph.h"
#include "renderer/gpu_resource_manager.h"

namespace mantle {
    void TransientResources::init(PersistentResource *persistent_resource,
                                  GPUResourceManager *resource_manager) {
        m_resource_manager = resource_manager;
        m_image_cache = std::pmr::vector<CreatedImageEntry>(persistent_resource);
        m_buffer_cache = std::pmr::vector<CreatedBufferEntry>(persistent_resource);
    }

    void TransientResources::set_images(
        std::pmr::vector<FGImageEntry> *images) {
        m_images = images;
    }

    void TransientResources::set_buffers(
        std::pmr::vector<FGBufferEntry> *buffers) {
        m_buffers = buffers;
    }

    ImageHandle TransientResources::get_image(FGImageHandle handle) {
        check(m_images != nullptr);
        check(handle.is_valid());
        check(handle.index < m_images->size());

        auto &entry = (*m_images)[handle.index];

        if (entry.is_imported()) {
            return entry.imported_handle;
        }

        if (handle.index >= m_image_cache.size()) {
            m_image_cache.resize(handle.index + 1);
        }

        auto &cached = m_image_cache[handle.index];
        if (cached.handle.is_valid()) {
            if (cached.desc.width == entry.desc.width &&
                cached.desc.height == entry.desc.height &&
                cached.desc.format == entry.desc.format) {
                return cached.handle;
            }
            m_resource_manager->destroy_image(cached.handle);
            cached.handle = {};
        }

        cached.handle = m_resource_manager->create_image(entry.desc);
        cached.desc = entry.desc;
        return cached.handle;
    }

    BufferHandle TransientResources::get_buffer(FGBufferHandle handle) {
        check(m_buffers != nullptr);
        check(handle.is_valid());
        check(handle.index < m_buffers->size());

        auto &entry = (*m_buffers)[handle.index];

        if (entry.is_imported()) {
            return entry.imported_handle;
        }

        if (handle.index >= m_buffer_cache.size()) {
            m_buffer_cache.resize(handle.index + 1);
        }

        auto &cached = m_buffer_cache[handle.index];
        if (cached.handle.is_valid()) {
            if (cached.desc.size == entry.desc.size &&
                cached.desc.usage == entry.desc.usage &&
                cached.desc.memory == entry.desc.memory) {
                return cached.handle;
            }
            m_resource_manager->destroy_buffer(cached.handle);
            cached.handle = {};
        }

        cached.handle = m_resource_manager->create_buffer(entry.desc);
        cached.desc = entry.desc;
        return cached.handle;
    }
} // namespace mantle
