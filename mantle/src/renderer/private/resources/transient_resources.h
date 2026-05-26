#pragma once

#include <vector>

#include "core/macros.h"
#include "core/memory/pmr/persistent_resource.h"
#include "renderer/types.h"

namespace mantle {
    class GPUResourceManager;

    class TransientResources final {
    public:
        TransientResources() = default;
        ~TransientResources() = default;

        MANTLE_NO_COPY_NO_MOVE(TransientResources);

        void init(PersistentResource *persistent_resource,
                  GPUResourceManager *resource_manager);

        void set_images(std::pmr::vector<RGImageEntry> *images);
        void set_buffers(std::pmr::vector<RGBufferEntry> *buffers);

        ImageHandle get_image(RGImageHandle handle);
        BufferHandle get_buffer(RGBufferHandle handle);

    private:
        struct CreatedImageEntry {
            ImageHandle handle;
            ImageDesc desc;
        };

        struct CreatedBufferEntry {
            BufferHandle handle;
            BufferDesc desc;
        };

        std::pmr::vector<RGImageEntry> *m_images = nullptr;
        std::pmr::vector<RGBufferEntry> *m_buffers = nullptr;

        GPUResourceManager *m_resource_manager = nullptr;
        std::pmr::vector<CreatedImageEntry> m_image_cache;
        std::pmr::vector<CreatedBufferEntry> m_buffer_cache;
    };
} // namespace mantle
