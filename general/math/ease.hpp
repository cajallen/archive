#pragma once

#include "general/math/math.hpp"

namespace spellbook::math {

enum EaseMode {
    EaseMode_Linear,
    EaseMode_Quad,
    EaseMode_QuadOut,
    EaseMode_QuadIn,
    EaseMode_Cubic,
    EaseMode_CubicOut,
    EaseMode_CubicIn,
    EaseMode_ElasticOut,
    EaseMode_ElasticIn,
    EaseMode_Elastic
};
constexpr float ease(float x, EaseMode mode) {
    switch (mode) {
        case (EaseMode_Linear): {
            return x;
        } break;
        case (EaseMode_Quad): {
            return x < 0.5f ? 2.0f * math::pow(x, 2.0f) : 1.0f - math::pow(-2.0f * x + 2.0f, 2.0f) / 2.0f;
        } break;
        case (EaseMode_QuadOut): {
            return math::pow(x, 2.0f);
        } break;
        case (EaseMode_QuadIn): {
            return 1.0f - math::pow(1.0f - x, 2.0f);
        } break;
        case (EaseMode_Cubic): {
            return x < 0.5f ? 4.0f * math::pow(x, 3.0f) : 1.0f - math::pow(-2.0f * x + 2.0f, 3.0f) / 2.0f;
        } break;
        case (EaseMode_CubicOut): {
            return math::pow(x, 3.0f);
        } break;
        case (EaseMode_CubicIn): {
            return 1.0f - math::pow(1.0f - x, 3.0f);
        } break;
        case (EaseMode_Elastic): {
            const float c5 = (2.0f * math::PI) / 4.5f;
            return x == 0.0f ? 0.0f
                : x == 1.0f ? 1.0f
                    : x < 0.5f ? -(math::pow(2.0f, 20 * x - 10.0f) * math::sin((20.0f * x - 11.125f) * c5)) / 2.0f
                        : (math::pow(2.0f, -20.0f * x + 10.0f) * math::sin((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
        } break;
        default: {
            __debugbreak();
            return x;
        }
    }
}

}