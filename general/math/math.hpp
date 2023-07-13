#pragma once

#include <cmath>

#include "general/math/geometry.hpp"

namespace spellbook::math {

constexpr float PI  = 3.14159265359f;
constexpr float TAU = 6.28318530718f;
constexpr float D2R = PI / 180.0f;
constexpr float R2D = 180.0f / PI;

float r2d(float rad);
float d2r(float deg);
euler r2d(euler rad);
euler d2r(euler deg);

bool contains(range r, float f);
bool contains(range2 r, v2 v);
bool contains(range2i r, v2i v);
bool contains(range3 r, v3 v);

int32 round_cast(float value);
v2i round_cast(v2 value);
v3i round_cast(v3 value);

int32 floor_cast(float value);
v2i floor_cast(v2 value);
v3i floor_cast(v3 value);

int32 ceil_cast(float value);
v2i ceil_cast(v2 value);
v3i ceil_cast(v3 value);

v3 convert_h(v4 v);
v4 convert_h(v3 v);

float length_squared(v2 v);
float length_squared(v3 v);
float length_squared_h(v4 v);
float length_squared(line2 v);
float length_squared(line3 v);

float length(v2 v);
float length(v3 v);
float length_h(v4 v);
float length(line2 v);
float length(line3 v);

float distance(v2 a, v2 b);
float distance(v3 a, v3 b);

float angle(v2 a);
float angle_difference(v2 a, v2 b);
float angle_to(v2 a, v2 b);

float dot(v2 a, v2 b);
float dot(v3 a, v3 b);

float cross(v2 a, v2 b);
v3  cross(v3 a, v3 b);
v3  ncross(v3 a, v3 b);

float to_range(float value, range r);
float from_range(float value, range r);

bool iterate(v3i& current, const v3i& min, const v3i& max);

float cos(float angle);
float sin(float angle);
float tan(float angle);
float acos(float angle);
float asin(float angle);
float atan(float angle);
float atan2(float y, float x);
float atan3(float y, float x);

void random_seed(uint32 seed);

int32 random_int32();
int32 random_int32(int32 high);
int32 random_int32(int32 low, int32 high);

float random_float();
float random_float(float high);
float random_float(const range& r);

uint64 random_uint64();

v3    euler2vector(euler e);
euler vector2euler(v3 v);

int32 ffsb(int32 input);
int32 csb(uint8 input);
int32 csb(uint32 input);
int32 csb(int32 input);

v2 project_point_onto_line(v2 point, line2 line);
v3 project_point_onto_line(v3 point, line3 line);
v3 project_to_segment(v3 point, line3 line);
v3 project_point_onto_plane(v3 point, v3 plane_point, v3 plane_normal);
v3 intersect_axis_plane(ray3 ray, uint32 axis, float axis_value);

float segment_distance(v2 pos32, line2 line);
bool segment_intersection(const v2 p0, const v2 p1, const v2 q0, const v2 q1, v2* result);
float line_intersection(const ray3& a, const ray3& b);

bool ray_aabb_intersection(v3 rstart, v3 rdir, v3 bstart, v3 bend, float* out_dist = nullptr);
bool ray_aabb_intersection(v2 rstart, v2 rdir, v2 bstart, v2 bend, float* out_dist = nullptr);

template <typename T> bool is_nan(T value) {
    return std::isnan(value);
}
template <> bool is_nan(v2 value);
template <> bool is_nan(v3 value);

template <typename T> bool is_inf(T value) {
    return std::isinf(value);
}
template <typename T> T abs(T value) {
    return std::abs(value);
}
template <typename T> constexpr T round(T value) {
    return std::round(value);
}
template <typename T> T floor(T value) {
    return std::floor(value);
}
template <typename T> T ceil(T value) {
    return std::ceil(value);
}
template <typename T> T ln(T value) {
    return std::log(value);
}
template <typename T> T log10(T value) {
    return std::log10(value);
}
template <typename T> T sqrt(T value) {
    return std::sqrt(value);
}
template <typename T> T cbrt(T value) {
    return std::cbrt(value);
}
template <typename T> T exp(T value) {
    return std::exp(value);
}
template <typename T> T hypot(T x, T y) {
    return std::hypot(x, y);
}
template <typename T> T pow(T base, T exp) {
    return std::pow(base, exp);
}
template <typename T> T copy_sign(T value, T sign) {
    return std::copysign(value, sign);
}

template <typename T> v2_<T> abs(v2_<T> v) {
    return v2_<T>(math::abs(v.x), math::abs(v.y));
}
template <typename T> constexpr v3_<T> abs(v3_<T> v) {
    return v3_<T>(math::abs(v.x), math::abs(v.y), math::abs(v.z));
}
template <typename T> v4_<T> abs(v4_<T> v) {
    return v4_<T>(math::abs(v.x), math::abs(v.y), math::abs(v.z), math::abs(v.w));
}

template <typename T> v2_<T> round(v2_<T> v) {
    return v2_<T>(math::round(v.x), math::round(v.y));
}
template <typename T> v3_<T> round(v3_<T> v) {
    return v3_<T>(math::round(v.x), math::round(v.y), math::round(v.z));
}
template <typename T> v4_<T> round(v4_<T> v) {
    return v4_<T>(math::round(v.x), math::round(v.y), math::round(v.z), math::round(v.w));
}

template <typename T> v2_<T> floor(v2_<T> v) {
    return v2_<T>(math::floor(v.x), math::floor(v.y));
}
template <typename T> v3_<T> floor(v3_<T> v) {
    return v3_<T>(math::floor(v.x), math::floor(v.y), math::floor(v.z));
}
template <typename T> v4_<T> floor(v4_<T> v) {
    return v4_<T>(math::floor(v.x), math::floor(v.y), math::floor(v.z), math::floor(v.w));
}

template <typename T> v2_<T> pow(v2_<T> base, T exp) {
    return v2_<T>(math::pow(base.x, exp), math::pow(base.y, exp));
}
template <typename T> v3_<T> pow(v3_<T> base, T exp) {
    return v3_<T>(math::pow(base.x, exp), math::pow(base.y, exp), math::pow(base.z, exp));
}
template <typename T> v4_<T> pow(v4_<T> base, T exp) {
    return v4_<T>(math::pow(base.x, exp), math::pow(base.y, exp), math::pow(base.z, exp), math::pow(base.w, exp));
}

template <typename T> v2_<T> copy_sign(v2_<T> value, v2_<T> sign) {
    return v2_<T>(math::copy_sign(value.x, sign.x), math::copy_sign(value.y, sign.y));
}
template <typename T> v3_<T> copy_sign(v3_<T> value, v3_<T> sign) {
    return v3_<T>(math::copy_sign(value.x, sign.x), math::copy_sign(value.y, sign.y), math::copy_sign(value.z, sign.z));
}
template <typename T> v4_<T> copy_sign(v4_<T> value, v4_<T> sign) {
    return v4_<T>(
        math::copy_sign(value.x, sign.x), math::copy_sign(value.y, sign.y), math::copy_sign(value.z, sign.z), math::copy_sign(value.w, sign.w));
}

template <typename T> constexpr const T& clamp(const T& value, const T& minValue, const T& maxValue) {
    if (value < minValue)
        return minValue;
    else if (value > maxValue)
        return maxValue;
    else
        return value;
}

template <typename T> constexpr v2_<T> clamp(v2_<T> value, v2_<T> low, v2_<T> high) {
    return v2_<T>(math::clamp(value.x, low.x, high.x), math::clamp(value.y, low.y, high.y));
}
template <typename T> constexpr v3_<T> clamp(v3_<T> value, v3_<T> low, v3_<T> high) {
    return v3_<T>(math::clamp(value.x, low.x, high.x), math::clamp(value.y, low.y, high.y), math::clamp(value.z, low.z, high.z));
}

template <typename T> T max(T a, T b) {
    return (a > b) ? a : b;
}
template <typename T, typename... Ts> T max(T a, T b, Ts... c) {
    return math::max(math::max(a, b), c...);
}
template <typename T> v2_<T> max(v2_<T> a, v2_<T> b) {
    return v2_<T>(math::max(a.x, b.x), math::max(a.y, b.y));
}
template <typename T> v3_<T> max(v3_<T> a, v3_<T> b) {
    return v3_<T>(math::max(a.x, b.x), math::max(a.y, b.y), math::max(a.z, b.z));
}

template <typename T> T min(T a, T b) {
    return (a < b) ? a : b;
}
template <typename T, typename... Ts> T min(T a, T b, Ts... c) {
    return math::min(math::min(a, b), c...);
}
template <typename T> v2_<T> min(v2_<T> a, v2_<T> b) {
    return v2_<T>(math::min(a.x, b.x), math::min(a.y, b.y));
}
template <typename T> v3_<T> min(v3_<T> a, v3_<T> b) {
    return v3_<T>(math::min(a.x, b.x), math::min(a.y, b.y), math::min(a.z, b.z));
}

template <typename T> bool is_equal(T a, T b, T e) {
    return math::abs(a - b) < e;
}
template <typename T> bool is_equal(v2_<T> a, v2_<T> b, T e) {
    return math::is_equal(a.x, b.x, e) && math::is_equal(a.y, b.y, e);
}
template <typename T> bool is_equal(v3_<T> a, v3_<T> b, T e) {
    return math::is_equal(a.x, b.x, e) && math::is_equal(a.y, b.y, e) && math::is_equal(a.z, b.z, e);
}
template <typename T> bool is_equal(v4_<T> a, v4_<T> b, T e) {
    return math::is_equal(a.x, b.x, e) && math::is_equal(a.y, b.y, e) && math::is_equal(a.z, b.z, e) && math::is_equal(a.w, b.w, e);
}

template <typename T> T normalize(T t) {
    return t / math::length(t);
}

// Converts 0to1 values to -1to1
template <typename T> T to_signed_range(T value, bool invert = false) {
    return ((invert ? (1.0f - value) : value) * 2.0f) - 1.0f;
}

// Converts -1to1 values to 0to1
template <typename T> T to_unsigned_range(T value, bool invert = false) {
    return ((invert ? -1.0f : 1.0f) * value + 1.0f) / 2.0f;
}

template <typename T> constexpr T mix(T a, T b, float x) {
    return a*(1.f-x)+b*x;
}

constexpr float smoothstep(float edge0, float edge1, float x) {
    x = math::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3 - 2 * x);
}

inline float mod(float input, float divisor) {
    return std::fmod(input, divisor);
}

constexpr int32 mod(int32 input, int32 divisor) {
    return input % divisor;
}

template <typename T>
v2_<T> mod(v2_<T> input, v2_<T> divisor) {
    return v2_<T>{math::mod(input.x, divisor.x), math::mod(input.y, divisor.y)};
}

template <typename T>
v3_<T> mod(v3_<T> input, v3_<T> divisor) {
    return v3_<T>{math::mod(input.x, divisor.x), math::mod(input.y, divisor.y), math::mod(input.z, divisor.z)};
}

inline float fract(float input) {
    return math::mod(input, 1.0f);
}

float lerp_angle(float t, range r);
float angle_difference(float a, float b);
float wrap_angle (float angle);
bool contains_angle(range r, float angle);
float clamp_angle(float angle, range r);

inline float clamp(float input, range range) {
    return math::clamp(input, range.start, range.end);
}

inline float map_range(float input, range input_range, range output_range) {
    float value = clamp(from_range(input, input_range), {0.0f, 1.0f});
    value = to_range(value, output_range);
    return value;
}

constexpr float lerp(float x, range r) {
    return r.start*(1.0f-x)+r.end*x;
}
constexpr v3 lerp(float x, range3 r) {
    return r.start*(1.0f-x)+r.end*x;
}

v3 rotate(v3 v, uint8 cardinal_rotation);

}
