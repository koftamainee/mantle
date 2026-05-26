#include "renderer/render_graph.h"

namespace mantle {

    RGImageHandle RenderGraphBuilder::create_image(const ImageDesc &desc) {
        u32 index = (*m_next_image_index)++;
        if (index >= m_images->size()) {
            m_images->resize(index + 1);
        }
        (*m_images)[index] = RGImageEntry{.desc = desc};

        RGImageHandle handle;
        handle.index = index;
        handle.generation = 1;
        return handle;
    }

    RGBufferHandle RenderGraphBuilder::create_buffer(const BufferDesc &desc) {
        u32 index = (*m_next_buffer_index)++;
        if (index >= m_buffers->size()) {
            m_buffers->resize(index + 1);
        }
        (*m_buffers)[index] = RGBufferEntry{.desc = desc};

        RGBufferHandle handle;
        handle.index = index;
        handle.generation = 1;
        return handle;
    }

    RGImageHandle RenderGraphBuilder::read(RGImageHandle image,
                                           ReadUsage usage) {
        m_image_reads->push_back({
            .pass_index = m_pass_index,
            .handle = image,
            .usage = usage,
        });
        return image;
    }

    RGImageHandle RenderGraphBuilder::write(RGImageHandle image,
                                            WriteUsage usage) {
        m_image_writes->push_back({
            .pass_index = m_pass_index,
            .handle = image,
            .usage = usage,
        });

        auto &entry = (*m_images)[image.index];
        entry.version++;

        RGImageHandle new_handle;
        new_handle.index = image.index;
        new_handle.generation = entry.version;
        return new_handle;
    }

    RGBufferHandle RenderGraphBuilder::read(RGBufferHandle buffer,
                                            BufferReadUsage usage) {
        m_buffer_reads->push_back({
            .pass_index = m_pass_index,
            .handle = buffer,
            .usage = usage,
        });
        return buffer;
    }

    RGBufferHandle RenderGraphBuilder::write(RGBufferHandle buffer,
                                             BufferWriteUsage usage) {
        m_buffer_writes->push_back({
            .pass_index = m_pass_index,
            .handle = buffer,
            .usage = usage,
        });

        auto &entry = (*m_buffers)[buffer.index];
        entry.version++;

        RGBufferHandle new_handle;
        new_handle.index = buffer.index;
        new_handle.generation = entry.version;
        return new_handle;
    }
} // namespace mantle
