#pragma once

#include "general/vector.hpp"
#include "general/logger.hpp"
#include "general/math/math.hpp"
#include "general/math/geometry.hpp"

namespace spellbook {

inline v3 bspline4(float t, span<const v3> points) {
    assert_else(points.size() == 4)
        return v3(0);
    return ((-points[0] + points[2]) * 0.5f +
        ((points[0] - 2.f * points[1] + points[2]) * 0.5f + (-points[0] + 3.f * points[1] - 3.f * points[2] + points[3]) * (1.f / 6.f) * t)
        * t) * t +
        (points[0] + 4.f * points[1] + points[2]) * (1.f / 6.f);
}

inline v3 bspline(float t, const vector<v3>& points)
{
    if (t <= 0.0001f)
        return points.front();
    if (t >= 1.0f - 0.0001f)
        return points.back();
    t *= points.size() - 3;

    int start = math::floor_cast(t);
    float fract = fmod(t, 1.0f);
    assert_else(start + 3 < int(points.size()))
        return points.front();
    return bspline4(fract, span{points.data() + start, 4});
}

}