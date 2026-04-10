#pragma once
#include <type_traits>
#include <utility>


#include "core/memory/virtual_heap.h"
#include "core/types.h"

namespace mantle {

    class ArenaAllocator final {
      public:
        ArenaAllocator() = default;
        ~ArenaAllocator();

        ArenaAllocator(const ArenaAllocator &) = delete;
        ArenaAllocator &operator=(const ArenaAllocator &) = delete;
        ArenaAllocator(ArenaAllocator &&) = delete;
        ArenaAllocator &operator=(ArenaAllocator &&) = delete;

        void init(VirtualHeap &heap, usize size);
        void destroy();

        [[nodiscard]] void *push(usize size, usize align = 16);

        template <typename T>
        [[nodiscard]] T *push(usize count = 1) {
            static_assert(std::is_trivially_constructible_v<T>);
            return static_cast<T *>(push(sizeof(T) * count, alignof(T)));
        }

        template <typename T, typename... Args>
        [[nodiscard]] T *emplace(Args &&...args) {
            void *mem = push(sizeof(T), alignof(T));
            return new (mem) T(std::forward<Args>(args)...);
        }

        struct Marker {
            usize offset;
        };

        Marker save() const;
        void restore(Marker marker);
        void reset();

        usize size() const;
        usize offset() const;

      private:
        void *m_base = nullptr;
        usize m_size = 0;
        usize m_offset = 0;
        bool m_is_initialized = false;
    };

} // namespace mantle
