// Copyright (c) 2026 Mantle. All rights reserved.

#pragma once

#include <bit>

#include "core/types.h"

namespace mantle {

    struct FaceMask {
        u32 rows[32];

        void set(u32 x, u32 z) noexcept { rows[z] |= (1u << x); }
        void clear(u32 x, u32 z) noexcept { rows[z] &= ~(1u << x); }
        bool get(u32 x, u32 z) const noexcept { return (rows[z] >> x) & 1u; }

        bool empty() const noexcept {
            for (auto r : rows) {
                if (r) {
                    return false;
                }
            }
            return true;
        }

        void reset() noexcept {
            for (auto &r : rows) {
                r = 0;
            }
        }
    };

} // namespace mantle
