#pragma once

#include "general/math/geometry.hpp"

namespace spellbook {

struct NavigationPath {
    vector<v3> waypoints;
    int32 reached = INT_MAX;

    NavigationPath() {}
    NavigationPath(const vector<v3>& wps) : waypoints(wps), reached(wps.size()) {}
    NavigationPath(vector<v3>&& wps) : waypoints(wps), reached(wps.size()) {}

    v3 get_start() const;
    v3 get_destination() const;
    bool valid() const;

    v3 get_real_target(const v3& current_pos);
};

}