#pragma once
#include "core/macros.h"
#include "core/types.h"

#include <memory_resource>
#include <unordered_map>

namespace mantle {

    class Blackboard final {
      public:
        struct TypeIdBase {
          protected:
            static inline u32 s_next = 0;
        };

        template <typename T>
        struct TypeId : TypeIdBase {
            static u32 id() {
                static u32 s_id = s_next++;
                return s_id;
            }
        };

        explicit Blackboard(std::pmr::memory_resource *memory = nullptr)
            : m_entries(memory) {}

        MANTLE_NO_COPY_NO_MOVE(Blackboard);

        template <typename T>
        void add(T value) {
            u32 tid = TypeId<T>::id();
            auto *ptr = m_entries.get_allocator()
                            .resource()
                            ->allocate(sizeof(T), alignof(T));
            new (ptr) T(std::move(value));
            m_entries[tid] = ptr;
        }

        template <typename T>
        T *get() {
            u32 tid = TypeId<T>::id();
            auto it = m_entries.find(tid);
            if (it != m_entries.end()) {
                return static_cast<T *>(it->second);
            }
            return nullptr;
        }

        template <typename T>
        const T *get() const {
            u32 tid = TypeId<T>::id();
            auto it = m_entries.find(tid);
            if (it != m_entries.end()) {
                return static_cast<const T *>(it->second);
            }
            return nullptr;
        }

        template <typename T>
        bool has() const {
            return m_entries.contains(TypeId<T>::id());
        }

      private:
        std::pmr::unordered_map<u32, void *> m_entries;
    };

} // namespace mantle
