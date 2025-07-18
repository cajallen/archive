﻿#include "general/bitmask_3d.hpp"

#include "extension/fmt.hpp"
#include "extension/fmt_geometry.hpp"

namespace spellbook {

void Bitmask3D::set(v3i pos, bool on) {
    v3i chunk_index = math::floor_cast(v3(pos) / v3(4.0f));
    v3i local_pos = pos - chunk_index * v3i(4);
    uint8 bit_index = local_pos.z * 4 * 4 + local_pos.y * 4 + local_pos.x;
    
    if (!chunks.contains(chunk_index))
        chunks[chunk_index] = 0;
    if (on)
        chunks[chunk_index] |= 0b1ull << bit_index;
    else
        chunks[chunk_index] &= ~(0b1ull << bit_index);
}

bool Bitmask3D::get(v3i pos) const {
    v3i chunk_index = math::floor_cast(v3(pos) / v3(4.0f));
    if (!chunks.contains(chunk_index))
        return false;
    v3i local_pos = pos - chunk_index * v3i(4);
    uint8 bit_index = local_pos.z * 4 * 4 + local_pos.y * 4 + local_pos.x;
    return chunks.at(chunk_index) & (0b1ull << bit_index);
}

void Bitmask3D::clear() {
    chunks.clear();
}


v3i Bitmask3D::rough_min() const {
    v3i min_id = v3i(INT_MAX);
    for (auto& [chunk_id, _] : chunks) {
        if (chunk_id.x < min_id.x)
            min_id.x = chunk_id.x;
        if (chunk_id.y < min_id.y)
            min_id.y = chunk_id.y;
        if (chunk_id.z < min_id.z)
            min_id.z = chunk_id.z;
    }
    if (min_id.x == INT_MAX || min_id.y == INT_MAX || min_id.z == INT_MAX)
        return v3i(INT_MAX);
    return min_id * 4;
}
v3i Bitmask3D::rough_max() const {
    v3i max_id = v3i(-INT_MAX);
    for (auto& [chunk_id, _] : chunks) {
        if (chunk_id.x > max_id.x)
            max_id.x = chunk_id.x;
        if (chunk_id.y > max_id.y)
            max_id.y = chunk_id.y;
        if (chunk_id.z > max_id.z)
            max_id.z = chunk_id.z;
    }
    if (max_id.x == -INT_MAX || max_id.y == -INT_MAX || max_id.z == -INT_MAX)
        return v3i(-INT_MAX);
    return max_id * 4 + v3i(3);
}

bool ray_intersection(const Bitmask3D& bitmask, ray3 ray, v3& pos, v3i& open_cube, const function<bool(ray3, v3i, v3&)>& additional_constraints) {
    v3i bound_min = bitmask.rough_min();
    v3i bound_max = bitmask.rough_max();
    
    v3i step = math::copy_sign(v3i(1), math::floor_cast(ray.dir));
    v3 delta_t = math::abs(v3(1.0f) / ray.dir);
    
    v3i current_voxel = math::floor_cast(ray.origin);
    open_cube = current_voxel;

    float x_dist = step.x > 0 ? math::floor(ray.origin.x + 1.0f) - ray.origin.x : math::ceil(ray.origin.x - 1.0f) - ray.origin.x;
    float y_dist = step.y > 0 ? math::floor(ray.origin.y + 1.0f) - ray.origin.y : math::ceil(ray.origin.y - 1.0f) - ray.origin.y;
    float z_dist = step.z > 0 ? math::floor(ray.origin.z + 1.0f) - ray.origin.z : math::ceil(ray.origin.z - 1.0f) - ray.origin.z;
    v3 next_t = math::abs(v3(delta_t.x * x_dist, delta_t.y * y_dist, delta_t.z * z_dist));
    float t = 0.0f;

    while (!math::is_nan(t)) {
        if (bitmask.get(current_voxel)) {
            pos = ray.origin + t * ray.dir;
            return true;
        }
        if (additional_constraints && additional_constraints(ray, current_voxel, pos)) {
            return true;
        }

        open_cube = current_voxel;
        
        int min_axis = 0;
        if (next_t[1] < next_t[0])
            min_axis = 1;
        if (next_t[2] < next_t[min_axis])
            min_axis = 2;
        
        current_voxel[min_axis] = current_voxel[min_axis] + step[min_axis];
        t += next_t[min_axis];
        next_t -= v3(next_t[min_axis]);
        next_t[min_axis] += delta_t[min_axis];
        

        for (int i = 0; i < 3; ++i) {
            if (step[i] > 0) {
                if (current_voxel[i] > bound_max[i])
                    return false;
            }
            else {
                if (current_voxel[i] < bound_min[i])
                    return false;
            }
        }
    }
    return false;
}

}