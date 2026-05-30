#pragma once
#include "renderer/types.h"

namespace mantle {
    struct BbChunkVertex {
        FGBufferHandle handle;
    };
    struct BbChunkIndex {
        FGBufferHandle handle;
    };
    struct BbChunkIndirect {
        FGBufferHandle handle;
    };
} // namespace mantle
