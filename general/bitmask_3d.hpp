﻿#pragma once

#include "general/umap.hpp"
#include "general/function.hpp"
#include "general/math/geometry.hpp"

namespace spellbook {

struct Bitmask3D {
    umap<v3i, uint64> chunks;

    void set(v3i pos, bool on = true);
    bool get(v3i pos) const;
    v3i rough_min() const;
    v3i rough_max() const;
    void clear();
};
bool ray_intersection(const Bitmask3D& bitmask, ray3 ray, v3& pos, v3i& cube, const function<bool(ray3, v3i, v3&)>& additional_constraints);

}