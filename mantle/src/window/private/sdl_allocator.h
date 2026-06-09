#pragma once

#include "core/macros.h"
#include "core/memory/memory_block.h"
#include "core/memory/tlsf_allocator.h"
#include <SDL3/SDL_stdinc.h>

namespace mantle {

    class SDLAllocator final {
    public:
        MANTLE_NO_COPY_NO_MOVE(SDLAllocator);

        static SDLAllocator &create(MemoryBlock block);
        static SDLAllocator &instance();
        static void          destroy();

        SDL_malloc_func  malloc()  const;
        SDL_calloc_func  calloc()  const;
        SDL_realloc_func realloc() const;
        SDL_free_func    free()    const;

    private:
        explicit SDLAllocator(MemoryBlock block);
        ~SDLAllocator();

        static void *SDLCALL sdl_malloc_impl(size_t size);
        static void *SDLCALL sdl_calloc_impl(size_t nmemb, size_t size);
        static void *SDLCALL sdl_realloc_impl(void *mem, size_t size);
        static void  SDLCALL sdl_free_impl(void *mem);

        TlsfAllocator    m_tlsf;
        SDL_malloc_func  m_malloc;
        SDL_calloc_func  m_calloc;
        SDL_realloc_func m_realloc;
        SDL_free_func    m_free;
    };

} // namespace mantle