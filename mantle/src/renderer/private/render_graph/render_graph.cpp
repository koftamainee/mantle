#include "renderer/render_graph.h"

#include "core/assert.h"
#include "render_graph/render_graph_internal.h"

namespace mantle {
    RenderGraph::RenderGraph(ArenaAllocator *arena) :
        m_arena(arena), m_scope(arena), m_resource(arena), m_passes(&m_resource),
        m_images(&m_resource), m_buffers(&m_resource),
        m_image_reads(&m_resource), m_image_writes(&m_resource),
        m_buffer_reads(&m_resource), m_buffer_writes(&m_resource) {
        check(arena != nullptr);
    }

    RGImageHandle RenderGraph::create_image(const ImageDesc &desc) {
        RGImageHandle handle;
        handle.index = m_next_image_index++;
        handle.generation = 1;

        if (handle.index >= m_images.size()) {
            m_images.resize(handle.index + 1);
        }
        m_images[handle.index] = RGImageEntry{.desc = desc};

        return handle;
    }

    RGImageHandle RenderGraph::import_image(ImageHandle image) {
        RGImageHandle handle;
        handle.index = m_next_image_index++;
        handle.generation = 1;

        if (handle.index >= m_images.size()) {
            m_images.resize(handle.index + 1);
        }
        m_images[handle.index] = RGImageEntry{.imported_handle = image};

        return handle;
    }

    RGBufferHandle RenderGraph::create_buffer(const BufferDesc &desc) {
        RGBufferHandle handle;
        handle.index = m_next_buffer_index++;
        handle.generation = 1;

        if (handle.index >= m_buffers.size()) {
            m_buffers.resize(handle.index + 1);
        }
        m_buffers[handle.index] = RGBufferEntry{.desc = desc};

        return handle;
    }

    RGBufferHandle RenderGraph::import_buffer(BufferHandle buffer) {
        RGBufferHandle handle;
        handle.index = m_next_buffer_index++;
        handle.generation = 1;

        if (handle.index >= m_buffers.size()) {
            m_buffers.resize(handle.index + 1);
        }
        m_buffers[handle.index] = RGBufferEntry{.imported_handle = buffer};

        return handle;
    }
} // namespace mantle
