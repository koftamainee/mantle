#pragma once
#include "renderer/types.h"


namespace mantle {
    class TransientResources final {
    public:
        TransientResources() = default;
        ~TransientResources();

        MANTLE_NO_COPY_NO_MOVE(TransientResources);

        void init();
        void destroy();

        ImageHandle get_image(RGImageHandle handle);
        BufferHandle get_buffer(RGBufferHandle handle);

    private:
        bool m_is_initialized = false;
    };
}