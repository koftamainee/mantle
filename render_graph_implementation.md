# Render Graph Implementation Plan

## References

- Frostbite FrameGraph (GDC 2017, Yuriy O'Donnell): read/write flags encode *how* a pass accesses a resource (sampled, color attachment, storage, etc.). This is essential because the render graph needs this information to automatically compute correct pipeline barriers — it maps each usage to the right image layout, pipeline stage, and access mask. Without flags, you'd need manual barrier code, defeating the purpose of the abstraction.

## TODOs from `render_graph.h` — status

| TODO | Status |
|------|--------|
| Setup phase | **MVP** — run setup lambda in `add_pass` |
| TransientResources | **MVP** — basic get/set for imported resources; full aliasing **deferred** |
| Deferred created resources | **Deferred** — create GPU resource on first use, not at declaration |
| Derived resource parameters | **Deferred** — e.g. half-res output from input size |
| MoveSubresource (aliasing) | **Deferred** — reuse memory for non-overlapping lifetimes |
| **Compilation**: cull unused, calc lifetimes, greedy alloc | **Deferred** — topo-sort + barrier computation in MVP, full allocator later |
| **Execution**: execute callbacks, RenderPassContext, real GPU handles | **MVP** — `Renderer::execute()` walks compiled passes |
| read/write flags | ✅ **New design added below** — existing API must change |
| use_render_target | **Deferred** — may be superseded by write flags |
| Async compute | **Ignored per user** |
| Blackboard | **Deferred** — inter-pass data communication |

## API changes

### Builder read/write get flags

Without flags, the graph cannot infer image layouts, pipeline stages, or access masks — you'd have to hand-roll barriers anyway.

```cpp
enum class ReadUsage {
    None = 0,
    Sampled           = 1 << 0,   // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    StorageRead       = 1 << 1,   // VK_IMAGE_LAYOUT_GENERAL (read access)
    InputAttachment   = 1 << 2,   // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (input)
    TransferSrc       = 1 << 3,   // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
    IndirectArg       = 1 << 4,   // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (indirect)
    MaxEnum           = (1 << 5) - 1,
};

enum class WriteUsage {
    None = 0,
    ColorAttachment   = 1 << 0,   // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    DepthAttachment   = 1 << 1,   // VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
    StorageWrite      = 1 << 2,   // VK_IMAGE_LAYOUT_GENERAL (write access)
    TransferDst       = 1 << 3,   // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
    Clear             = 1 << 4,   // arbitrary layout (clear before use)
    Present           = 1 << 5,   // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    MaxEnum           = (1 << 6) - 1,
};
```

Methods change from:
```cpp
RGImageHandle read(RGImageHandle image);
RGImageHandle write(RGImageHandle image);
```
to:
```cpp
RGImageHandle read(RGImageHandle image, ReadUsage usage);
RGImageHandle write(RGImageHandle image, WriteUsage usage);
```

Usage in `engine.cpp` becomes:
```cpp
pass.out_backbuffer = builder.write(backbuffer, WriteUsage::ColorAttachment);
```

Buffer variants get buffer-specific enums (Vertex, Index, Storage, etc.) **deferred**.

### Write-invalidation (deferred)

In Frostbite, `write()` returns a *new* handle and invalidates the old one to prevent read-after-write hazards. **Deferred** — for MVP the same handle is returned.

## Implementation steps

### Step 0: Read/Write usage enums
**Files:** `mantle/src/renderer/public/renderer/types.h`
- Add `ReadUsage` and `WriteUsage` enum classes (see above)

### Step 1: Fix `RenderPassContext` constructibility
**Files:** `mantle/src/renderer/public/renderer/render_graph.h`
- `RenderPassContext() = delete;` → `RenderPassContext() = default;`

### Step 2: Complete `TransientResources::get_image/get_buffer`
**Files:** `mantle/src/renderer/private/resources/transient_resources.cpp`
- `get_image(handle)`: lookup in `m_images`, return the `ImageHandle`
- `get_buffer(handle)`: lookup in `m_buffers`, return the `BufferHandle`
- Add `check()` that handle exists

### Step 3: Implement `RenderGraph::import_image/import_buffer`
**Files:** `mantle/src/renderer/private/render_graph/render_graph.cpp`
- Add internal struct to track `(RGImageHandle → ImageHandle)` and `(RGBufferHandle → BufferHandle)` mappings
- Store on the arena PMR resource
- `import_image()`: assign new handle, store mapping, return RG handle
- `import_buffer()`: same

### Step 4: Implement `RenderGraph::add_pass`
**Files:** `mantle/src/renderer/private/render_graph/render_graph.h` (template body, internal struct)
- Move template body from header to add internal helpers
- Allocate `TData` on `m_arena` via `m_arena->push<TData>()`
- Build a `RenderGraphBuilder::Impl` on the arena pointing to the graph's compilation state
- Call `setup(builder, *data)`
- Store `{name, data, execute_fn}` in `m_passes`
- Return `*data`

### Step 5: Implement builder read/write with flags
**Files:** `mantle/src/renderer/private/render_graph/render_graph_builder.h/.cpp`
- Extend `RenderGraphBuilder::Impl` to track the current pass index and the graph's access lists
- `read(image, usage)`: record `{pass_index, RGImageHandle, ReadUsage}` in an access list; return the handle
- `write(image, usage)`: record `{pass_index, RGImageHandle, WriteUsage}` in an access list; return the handle
- These records feed barrier computation in Step 6

### Step 6: Barrier computation from flags
**Files:** `mantle/src/renderer/private/render_graph/render_graph.cpp` (new helper)
- Map each `WriteUsage` → `(ImageLayout, PipelineStage, AccessType)`:
  - `ColorAttachment` → `(ColorAttachment, ColorOutput, ColorAttachmentWrite)`
  - `DepthAttachment` → `(DepthAttachment, EarlyDepth, DepthAttachmentWrite)`
  - etc.
- Map each `ReadUsage` → `(ImageLayout, PipelineStage, AccessType)`:
  - `Sampled` → `(ShaderReadOnly, FragmentShader, ShaderRead)`
  - `TransferSrc` → `(TransferSrc, Transfer, TransferRead)`
  - etc.
- Compile phase: for each pass, compute delta between current layout (from previous pass's write) and required layout (from this pass's read/write), emit `ImageBarrier` array

### Step 7: Implement `Renderer::execute(graph)`
**Files:** `mantle/src/renderer/private/core/renderer.cpp`
- Get `current_frame.cmd` (already set by `begin_frame`)
- For each pass in `m_passes` in order:
  - Compute barriers from Step 6 for all resources the pass touches
  - Record barriers on `cmd` via `cmd->image_barriers()`
  - Update `ImageResource::current_layout` for each barrier (already done by `CommandRecorder::image_barrier`)
  - Create `RenderPassContext::Impl` on frame arena with: `cmd`, `resource_manager`, `transient_resources`, `scratch_arena`
  - Default-construct `RenderPassContext`, call `init(&impl)`, invoke `execute_fn(data, ctx)`

### Step 8: Wire `TransientResources` into renderer
**Files:** `mantle/src/renderer/private/resources/transient_resources.h/.cpp`, `mantle/src/renderer/private/core/renderer.cpp`
- Add `TransientResources m_transient_resources` to `Renderer::Impl`
- Initialize in `Renderer::init()`, destroy in `Renderer::destroy()`
- Pass it through to the render graph (via Steps 3-5)
- `RenderPassContext` already references `transient_resources` in its `Impl`

## Files modified (final)

| File | Change |
|------|--------|
| `mantle/src/renderer/public/renderer/types.h` | Add `ReadUsage`, `WriteUsage` enums with `MaxEnum` |
| `mantle/src/renderer/public/renderer/render_graph.h` | Fix `RenderPassContext` ctor; add `usage` params to `read/write`; graph internal storage |
| `mantle/src/renderer/private/render_graph/render_graph.cpp` | Implement `import_image/buffer`, `add_pass` storage, barrier computation helpers |
| `mantle/src/renderer/private/render_graph/render_graph_builder.h` | Extend `Impl` with pass index + graph access list refs |
| `mantle/src/renderer/private/render_graph/render_graph_builder.cpp` | Implement `read/write` with flag recording |
| `mantle/src/renderer/private/resources/transient_resources.cpp` | Complete `get_image/get_buffer` |
| `mantle/src/renderer/private/core/renderer.cpp` | Implement `execute()`; add + init `TransientResources` |
| `mantle/src/engine/private/engine.cpp` | Minor: update `builder.write(backbuffer)` → `builder.write(backbuffer, WriteUsage::ColorAttachment)` |

## Verification

```bash
cmake --build <build-dir> --target mantle
# Triangle should render through the render graph path
ctest  # existing aabb/frustum tests pass
```
