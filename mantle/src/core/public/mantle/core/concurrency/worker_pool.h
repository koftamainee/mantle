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

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "mantle/core/macros.h"
#include "mantle/core/memory/arena_allocator.h"
#include "mantle/core/memory/memory_block.h"
#include "mantle/core/types.h"

namespace mantle {

    class WorkerPool final {
      public:
        WorkerPool() = default;
        ~WorkerPool();

        MANTLE_NO_COPY_NO_MOVE(WorkerPool);

        void init(u32 num_workers, usize scratch_size, MemoryBlock block);
        void destroy();

        using TaskFn = void (*)(ArenaAllocator &scratch, void *user_data);
        void submit(TaskFn fn, void *user_data);
        void wait();

        u32 num_workers() const { return static_cast<u32>(m_workers.size()); }

      private:
        struct Worker {
            std::jthread   thread;
            ArenaAllocator scratch;
        };

        void worker_loop(Worker &worker, std::stop_token stop);

        std::vector<Worker>                   m_workers;
        std::mutex                            m_mutex;
        std::condition_variable               m_cv_has_work;
        std::condition_variable               m_cv_done;
        std::queue<std::pair<TaskFn, void *>> m_queue;
        u32                                   m_in_flight = 0;
        bool                                  m_stop = false;
    };

} // namespace mantle
