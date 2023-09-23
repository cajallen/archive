#include "navigation_path.hpp"

#include "general/math/math.hpp"

namespace spellbook {

v3 Path::get_start() const {
    return waypoints.back();
}

v3 Path::get_destination() const {
    return waypoints.front();
}

bool Path::valid() const {
    return !waypoints.empty();
}


v3 Path::get_real_target(const v3& current_pos) {
    float min_dist = FLT_MAX;
    v3 min_pos;
    int32 min_waypoint_index = -1;
    for (int32 i = math::min(reached, int32(waypoints.size()) - 1); i >= 1; i--) {
        v3 projected = math::project_to_segment(current_pos, line3{waypoints[i], waypoints[i-1]});
        float projected_dist = math::distance(current_pos, projected);
        if (projected_dist < min_dist) {
            min_dist = projected_dist;
            min_pos = projected;
            min_waypoint_index = i;
        }
    }

    if (min_waypoint_index < 0) {
        reached = 0;
        return current_pos;
    }
    reached = math::min(int32(reached), min_waypoint_index);
    
    min_waypoint_index--;
    float dist_to_advance = math::max(0.1f, min_dist);
    float dist_to_target = math::distance(min_pos, waypoints[min_waypoint_index]);
    while (min_waypoint_index >= 0 && dist_to_target < dist_to_advance) {
        dist_to_advance -= dist_to_target;
        min_pos = waypoints[min_waypoint_index];
        min_waypoint_index--;
        if (min_waypoint_index == -1)
            return min_pos;
        dist_to_target = math::distance(min_pos, waypoints[min_waypoint_index]);
    }

    v3 target_pos = min_pos + dist_to_advance * math::normalize(waypoints[min_waypoint_index] - min_pos);
//    if (math::distance(current_pos, target_pos) < 0.1f)
//        reached = math::min(int32(reached), min_waypoint_index - 1);
    
    return target_pos;
}


}