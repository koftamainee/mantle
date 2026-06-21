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

#include "mantle/core/logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace mantle {
    void register_loggers() {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mantle.log", true);

        auto meta_pattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v";
        stdout_sink->set_pattern(meta_pattern);
        file_sink->set_pattern(meta_pattern);

        std::vector<spdlog::sink_ptr> sinks = {stdout_sink, file_sink};

        auto make_logger = [&](const char *name) {
            auto logger = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
            spdlog::register_logger(logger);
            return logger;
        };

        make_logger("engine");
        make_logger("renderer");
        make_logger("vulkan");
        make_logger("world");
        make_logger("window");
        make_logger("core");
        make_logger("ecs");
        make_logger("physics");
        make_logger("assets");

        spdlog::set_default_logger(spdlog::get("engine"));

        auto raw_stdout = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        raw_stdout->set_pattern("%v");
        auto raw_file = std::make_shared<spdlog::sinks::basic_file_sink_mt>("mantle.log", true);
        raw_file->set_pattern("%v");
        std::vector<spdlog::sink_ptr> raw_sinks = {raw_stdout, raw_file};
        auto raw = std::make_shared<spdlog::logger>("raw", raw_sinks.begin(), raw_sinks.end());
        spdlog::register_logger(raw);

#ifndef NDEBUG
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::trace);
#else
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);
#endif
    }

    spdlog::logger *raw_logger() { return spdlog::get("raw").get(); }
} // namespace mantle
