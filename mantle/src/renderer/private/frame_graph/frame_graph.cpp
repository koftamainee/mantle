// Copyright 2026 Mantle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mantle/renderer/frame_graph.h"

#include "frame_graph/render_graph_internal.h"
#include "mantle/core/assert.h"

namespace mantle {
    FrameGraph::FrameGraph(ArenaAllocator *arena) :
        m_arena(arena),
        m_scope(arena),
        m_resource(arena),
        m_blackboard(&m_resource),
        m_passes(&m_resource),
        m_images(&m_resource),
        m_buffers(&m_resource),
        m_next_image_index(0),
        m_next_buffer_index(0),
        m_image_reads(&m_resource),
        m_image_writes(&m_resource),
        m_buffer_reads(&m_resource),
        m_buffer_writes(&m_resource) {
        MANTLE_CHECK(arena != nullptr);
    }

    FGImageHandle FrameGraph::import_image(ImageHandle image) {
        FGImageHandle handle;
        handle.index = m_next_image_index++;
        handle.generation = 1;

        if (handle.index >= m_images.size()) {
            m_images.resize((handle.index + 1) * 2);
        }
        m_images[handle.index] = FGImageEntry {.imported_handle = image};

        return handle;
    }

    FGBufferHandle FrameGraph::import_buffer(BufferHandle buffer) {
        FGBufferHandle handle;
        handle.index = m_next_buffer_index++;
        handle.generation = 1;

        if (handle.index >= m_buffers.size()) {
            m_buffers.resize((handle.index + 1) * 2);
        }
        m_buffers[handle.index] = FGBufferEntry {.imported_handle = buffer};

        return handle;
    }
} // namespace mantle
