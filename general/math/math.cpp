#include "math.hpp"

#include <cmath>
#include <random>

#include "general/logger.hpp"

namespace spellbook::math {

float r2d(float rad) {
    return rad * (180.0f / PI);
}

float d2r(float deg) {
    return deg * (PI / 180.0f);
}

euler r2d(euler rad) {
    return euler{math::r2d(rad.yaw), math::r2d(rad.pitch), math::r2d(rad.roll)};
}

euler d2r(euler deg) {
    return euler{math::d2r(deg.yaw), math::d2r(deg.pitch), math::d2r(deg.roll)};
}

bool contains(range r, float f) {
    return r.start < f && f < r.end;
}

bool contains(range2 r, v2 v) {
    return r.start[0] < v[0] && v[0] < r.end[0] && r.start[1] < v[1] && v[1] < r.end[1];
}

bool aabb_overlaps(range2 r1, range2 r2) {
    v2 e1 = 0.5f * math::abs(r1.end - r1.start);
    v2 e2 = 0.5f * math::abs(r2.end - r2.start);
    v2 c1 = 0.5f * math::abs(r1.start + r1.end);
    v2 c2 = 0.5f * math::abs(r2.start + r2.end);

    bool x = math::abs(c1.x - c2.x) <= (e1.x + e2.x);
    bool y = math::abs(c1.y - c2.y) <= (e1.y + e2.y);

    return x && y;
}

bool line_intersects_aabb(range2 line, range2 box) {
    v2 min = v2(math::min(box.start.x, box.end.x), math::min(box.start.y, box.end.y));
    v2 max = v2(math::max(box.start.x, box.end.x), math::max(box.start.y, box.end.y));
    float dist;
    if (ray_aabb_intersection(line.start, math::normalize(line.end - line.start), min, max, &dist)) {
        if (dist < math::distance(line.start, line.end)) {
            return true;
        }
    }
    return false;
}


bool contains(range2i r, v2i v) {
    return r.start[0] <= v[0] && v[0] < r.end[0] && r.start[1] <= v[1] && v[1] < r.end[1];
}

bool contains(range3 r, v3 v) {
    return r.start[0] < v[0] && v[0] < r.end[0] && r.start[1] < v[1] && v[1] < r.end[1] && r.start[2] < v[2] && v[2] < r.end[2];
}

void expand(range2i& r, v2i v) {
    r.start = math::min(r.start, v);
    r.end = math::max(r.end, v);
}

int32 round_cast(float value) {
    return (int32) math::round(value);
}
v2i round_cast(v2 value) {
    return v2i((int32) math::round(value.x), (int32) math::round(value.y));
}
v3i round_cast(v3 value) {
    return v3i((int32) math::round(value.x), (int32) math::round(value.y), (int32) math::round(value.z));
}

int32 floor_cast(float value) {
    return (int32) math::floor(value);
}
v2i floor_cast(v2 value) {
    return v2i((int32) math::floor(value.x), (int32) math::floor(value.y));
}
v3i floor_cast(v3 value) {
    return v3i((int32) math::floor(value.x), (int32) math::floor(value.y), (int32) math::floor(value.z));
}

int32 ceil_cast(float value) {
    return (int32) math::ceil(value);
}
v2i ceil_cast(v2 value) {
    return v2i((int32) math::ceil(value.x), (int32) math::ceil(value.y));
}
v3i ceil_cast(v3 value) {
    return v3i((int32) math::ceil(value.x), (int32) math::ceil(value.y), (int32) math::ceil(value.z));
}

v3 convert_h(v4 v) {
    return v3 {v.x / v.w, v.y / v.w, v.z / v.w};
}

v4 convert_h(v3 v) {
    return v4 {v.x, v.y, v.z, 1.0f};
}

float length_squared(v2 v) {
    return v.x * v.x + v.y * v.y;
}

float length_squared(v3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float length_squared_h(v4 v) {
    return length(convert_h(v));
}

float length(float v) {
    return abs(v);
}

float length(v2 v) {
    return sqrt(length_squared(v));
}

float length(v3 v) {
    return sqrt(length_squared(v));
}

float length_h(v4 v) {
    return sqrt(length_squared_h(v));
}

float angle(v2 a) {
    return atan2(a.y, a.x);
}

float angle_difference(v2 a, v2 b) {
    return angle(b) - angle(a);
}

float angle_to(v2 a, v2 b) {
    return angle(b - a);
}

float dot(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

float dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot(v4 a, v4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float cross(v2 a, v2 b) {
    return a.x * b.y - a.y * b.x;
}

v3 cross(v3 a, v3 b) {
    v3 out;
    out.x = a.y * b.z - a.z * b.y;
    out.y = a.z * b.x - a.x * b.z;
    out.z = a.x * b.y - a.y * b.x;
    return out;
}

v3 ncross(v3 a, v3 b) {
    return normalize(cross(a, b));
}

float to_range(float value, range r) {
    return r.start + value * (r.end - r.start);
}

float from_range(float value, range r) {
    if (r.end - r.start == 0.0f)
        return 0.0f;
    return (value - r.start) / (r.end - r.start);
}

bool iterate(v3i& current, const v3i& min, const v3i& max) {
    current.x += 1;
    if (current.x > max.x) {
        current.y += 1;
        current.x = min.x;
    }
    if (current.y > max.y) {
        current.z += 1;
        current.y = min.y;
    }
    if (current.z > max.z)
        return false;
    return true;
}

float length_squared(line2 v) {
    return math::length_squared(v.end - v.start);
}

float length_squared(line3 v) {
    return math::length_squared(v.end - v.start);
}

float length(line2 v) {
    return math::length(v.end - v.start);
}

float length(line3 v) {
    return math::length(v.end - v.start);
}

float distance(v2 a, v2 b) {
    return math::length(b - a);
}

float distance(v3 a, v3 b) {
    return math::length(b - a);
}

void random_seed(uint32 seed) {
    srand(seed);
}

int32 random_int32() {
    return rand();
}
int32 random_int32(int32 high) {
    return math::random_int32() % high;
}
int32 random_int32(int32 low, int32 high) {
    return low + math::random_int32(high - low + 1);
}

float random_float() {
    return rand() / float(RAND_MAX);
}
float random_float(float high) {
    return math::random_float() * high;
}
float random_float(const range& r) {
    return r.start + math::random_float(r.end - r.start);
}

uint64 random_uint64() {
    static std::random_device rd;
    static std::mt19937_64  random_engine(rd());
    static std::uniform_int_distribution<uint64> uniform_dist;
    return uniform_dist(random_engine);
}

v2 project_point_onto_line(v2 point, line2 line) {
    return line.end * (math::dot(line.vector(), point - line.start) / math::length_squared(line));
}

v3 project_point_onto_line(v3 point, line3 line) {
    return line.end * (math::dot(line.vector(), point - line.start) / math::length_squared(line));
}

v3 project_to_segment(v3 point, line3 line) {
    v3 line_vec = line.vector();
    float len_sq = math::length_squared(line_vec);
    if (len_sq == 0.0f)
        return line.start;
    float d = math::dot(line_vec, point - line.start);
    float t = math::clamp(d / len_sq, {0.0f, 1.0f});
    return line.start + t * line_vec;
}

v3 project_point_onto_plane(v3 point, v3 plane_point, v3 plane_normal) {
    return point - math::dot(point - plane_point, plane_normal) * plane_normal;
}

float cos(float angle) {
    return std::cos(angle);
}
float sin(float angle) {
    return std::sin(angle);
}
float tan(float angle) {
    return std::tan(angle);
}
float acos(float angle) {
    return std::acos(angle);
}
float asin(float angle) {
    return std::asin(angle);
}
float atan(float angle) {
    return std::atan(angle);
}
float atan2(float y, float x) {
    return std::atan2(y, x);
}
float atan3(float y, float x) {
    return y < 0 ? std::atan2(y, x) + math::TAU : std::atan2(y, x);
}

int32 ffsb(int32 input) {
    for (int32 i = 0; i < sizeof(int32) * 8; i++) {
        if (input & 0b1 << i)
            return i;
    }
    return -1;
}

int32 csb(uint8 input) {
    return std::popcount(input);
}

int32 csb(uint32 input) {
    return std::popcount(input);
}

int32 csb(int32 input) {
    int32 n = 0;
    for (int32 i = 0; i < sizeof(int32) * 8; i++) {
        if (input & 0b1 << i)
            n++;
    }
    return n;
}

v3 intersect_axis_plane(ray3 ray, uint32 axis, float axis_value) {
    double t     = (axis_value - ray.origin[axis]) / ray.dir[axis];
    uint32 axis1 = (axis + 1) % 3;
    uint32 axis2 = (axis + 2) % 3;
    v3 ret = {};
    ret[axis] = axis_value;
    ret[axis1] = ray.origin[axis1] + ray.dir[axis1] * t;
    ret[axis2] = ray.origin[axis2] + ray.dir[axis2] * t;
    return ret;
}

v3 euler2vector(euler e) {
    return math::normalize(v3 {math::cos(e.pitch) * math::cos(e.yaw), math::cos(e.pitch) * math::sin(e.yaw), math::sin(e.pitch)});
}

euler vector2euler(v3 v) {
    v         = math::normalize(v);
    float pitch = math::asin(v.z);
    float yaw   = math::atan2(v.y, v.x);
    return euler{yaw, pitch};
}

bool ray_aabb_intersection(v3 rstart, v3 rdir, v3 bstart, v3 bend, float* out_dist) {
    float tx1 = (bstart.x - rstart.x) / rdir.x;
    float tx2 = (bend.x - rstart.x) / rdir.x;

    float tmin = math::min(tx1, tx2);
    float tmax = math::max(tx1, tx2);

    float ty1 = (bstart.y - rstart.y) / rdir.y;
    float ty2 = (bend.y - rstart.y) / rdir.y;

    tmin = math::max(tmin, math::min(ty1, ty2));
    tmax = math::min(tmax, math::max(ty1, ty2));

    float tz1 = (bstart.z - rstart.z) / rdir.z;
    float tz2 = (bend.z - rstart.z) / rdir.z;

    tmin = math::max(tmin, math::min(tz1, tz2));
    tmax = math::min(tmax, math::max(tz1, tz2));

    if (out_dist != nullptr)
        *out_dist = math::max(0.0f, tmin);
    return tmax >= math::max(0.0f, tmin) && FLT_MAX > tmin;
}

// https://www.desmos.com/calculator/rryes1oucs
// Not fully sure why the graph isn't completely accurate, but the algorithm seems to work
// so there is probably a small error in the graph. - Cole
bool ray_aabb_intersection(v2 rstart, v2 rdir, v2 bstart, v2 bend, float* out_dist) {
    float tx1 = (bstart.x - rstart.x) / rdir.x;
    float tx2 = (bend.x - rstart.x) / rdir.x;

    float tmin = math::min(tx1, tx2); // segments to first intercept
    float tmax = math::max(tx1, tx2); // segments to second intercept

    float ty1 = (bstart.y - rstart.y) / rdir.y; //
    float ty2 = (bend.y - rstart.y) / rdir.y;

    tmin = math::max(tmin, math::min(ty1, ty2));
    tmax = math::min(tmax, math::max(ty1, ty2));

    if (out_dist != nullptr)
        *out_dist = math::max(0.0f, tmin);
    return tmax >= math::max(0.0f, tmin) && tmin < FLT_MAX;
}

template <> bool is_nan(v2 value) {
    return std::isnan(value.x) || std::isnan(value.y);
}
template <> bool is_nan(v3 value) {
    return std::isnan(value.x) || std::isnan(value.y) || std::isnan(value.z);
}

float segment_distance(v2 pos32, line2 line) {
    v2 s2p = pos32 - line.start;
    v2 s2e = line.end - line.start;
    s2e        = s2e / math::length(s2e);

    float d = dot(s2p, s2e);
    if (d < 0 || 1 < d)
        return -1;
    return length(cross(s2p, s2e));
}

bool segment_intersection(const v2 p0, const v2 p1, const v2 q0, const v2 q1, v2* result) {
    v2 v = p1 - p0;
    v2 u = q1 - q0;
    v2 d = p0 - q0;
    float c = math::cross(v, u);
    if (math::is_equal(c, 0.0f, 0.00001f))
        return false;
    float s = math::cross(v, d) / c;
    float t = math::cross(u, d) / c;
    if (s < 0 || s > 1 || t < 0 || t > 1)
        return false;
    *result = p0 + t * v;
    return true;
}

#define d_(a,b,c,d) (a.x-b.x)*(c.x-d.x)+(a.y-b.y)*(c.y-d.y)+(a.z-b.z)*(c.z-d.z);
float line_intersection(const ray3& a, const ray3& b) {
    auto o1 = a.origin;
    auto e1 = a.origin + a.dir;
    auto o2 = b.origin;
    auto e2 = b.origin + b.dir;
    float d1321 = d_(o1, o2, e1, o1);
    float d2121 = d_(e1, o1, e1, o1);
    float d4321 = d_(e2, o2, e1, o1);
    float d1343 = d_(o1, o2, e2, o2);
    float d4343 = d_(e2, o2, e2, o2);
    return (d1343*d4321-d1321*d4343)/(d2121*d4343-d4321*d4321);
}

float lerp_angle(float t, range r) {
    float diff = r.end - r.start;

    diff = math::mod(diff + math::PI, math::TAU);
    if (diff < 0.0f)
        diff += math::TAU;
    diff -= math::PI;

    return r.start + (diff * t);
}

float angle_difference(float a, float b) {
    return atan2(sin(b-a), cos(b-a));
}

float wrap_angle (float angle) {
    return angle - math::TAU * floor( angle / math::TAU);
};

bool contains_angle(range r, float angle) {
    r.start = wrap_angle(r.start);
    r.end = wrap_angle(r.end);
    angle = wrap_angle(angle);

    if (r.start < r.end)
        return (angle > r.start && angle < r.end);
    if (r.start > r.end)
        return (angle > r.start || angle < r.end);
    return (angle == r.start);
}

float clamp_angle(float angle, range r) {
    r.start = wrap_angle(r.start);
    r.end = wrap_angle(r.end - 0.001f);
    angle = wrap_angle(angle);

    // Clamp the angle to the range of the two given angles
    if (r.start < r.end)
        return math::max(r.start, math::min(r.end, angle));
    if (r.start > r.end) {
        if (angle > r.start || angle < r.end)
            return angle;
        if (math::abs(r.end - angle) < math::abs(r.start - angle))
            return r.end;
        return r.start;
    }
    return r.start;
}

v3 rotate(v3 v, uint8 cardinal_rotation) {
    if (cardinal_rotation % 4 == 0)
        return v;
    if (cardinal_rotation % 4 == 1)
        return v3{-v.y, v.x, v.z};
    if (cardinal_rotation % 4 == 2)
        return v3{-v.x, -v.y, v.z};
    if (cardinal_rotation % 4 == 3)
        return v3{v.y, -v.x, v.z};
    return v;
}

} // namespace math