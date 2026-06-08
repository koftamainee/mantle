// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include "spdlog/logger.h"

namespace mantle {
    void init_logger();

    spdlog::logger *raw_logger();
} // namespace mantle
