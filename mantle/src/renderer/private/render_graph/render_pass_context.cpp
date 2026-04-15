#include "renderer/render_graph.h"

// TODO
namespace mantle {
    RenderPassContext::~RenderPassContext() {}

    void RenderPassContext::bind_pipeline(GraphicsPipelineHandle pipeline) {}

    void RenderPassContext::bind_pipeline(ComputePipelineHandle pipeline) {}

    void RenderPassContext::begin_rendering(const RGRenderingInfo &info) {}

    void RenderPassContext::end_rendering() {}

    void RenderPassContext::set_viewport(f32 x, f32 y, f32 width, f32 height) {}

    void RenderPassContext::set_scissor(i32 x, i32 y, u32 width, u32 height) {}

    void RenderPassContext::bind_vertex_buffer(RGBufferHandle buffer,
                                               u32 binding, usize offset) {}

    void RenderPassContext::bind_index_buffer(RGBufferHandle buffer,
                                              usize offset) {}

    void
    RenderPassContext::copy_image_to_buffer(const RGImageBufferCopyInfo &info) {
    }

    void RenderPassContext::copy_image(const RGImageCopyInfo &info) {}

    void RenderPassContext::blit_image(const RGImageBlitInfo &info) {}

    void RenderPassContext::clear_color_image(RGImageHandle image, f32 r, f32 g,
                                              f32 b, f32 a) {}

    void RenderPassContext::clear_depth_image(RGImageHandle image, f32 depth) {}

    void RenderPassContext::push_constants(const void *data,
                                           ShaderStage stage) {}

    void RenderPassContext::draw(const RGDrawInfo &info) {}

    void RenderPassContext::draw_indexed(const RGDrawIndexedInfo &info) {}

    void RenderPassContext::dispatch(const RGDispatchInfo &info) {}

    void RenderPassContext::copy_buffer(const RGBufferCopyInfo &info) {}

    void
    RenderPassContext::copy_buffer_to_image(const RGBufferImageCopyInfo &info) {
    }

} // namespace mantle
