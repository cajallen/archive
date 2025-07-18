#pragma once

#include "general/umap.hpp"
#include "general/memory.hpp"
#include "general/function.hpp"
#include "general/bitmask_3d.hpp"
#include "general/navigation_path.hpp"
#include "general/math/math.hpp"

using std::shared_ptr;

namespace spellbook::astar {
using HeuristicFunction = function<uint32(v3i, v3i)>;

struct Node {
    uint32   G, H;
    v3i   position;
    shared_ptr<Node> parent;

    Node(v3i coord_, const shared_ptr<Node>& parent_ = nullptr);
    uint32 get_score() const;
};

struct Navigation {
    enum TileType {
        TileType_Empty,
        TileType_Path,
        TileType_OffRoad
    };
    TileType  _position_viable(v3i position);
    int _type_cost(TileType);
    shared_ptr<Node> _find_node_on_list(vector<shared_ptr<Node>>& nodes, v3i position);
    vector<std::pair<v3i, uint32>> _get_neighbors(v3i position);
    
    // includes start/end, reverse order (target first)
    NavigationPath find_path(v3i source, v3i target, float tolerance = 0.1f);

    Bitmask3D* path_solids;
    Bitmask3D* off_road_solids;
    Bitmask3D* unstandable_solids;
    umap<v3i, Direction>* ramps;
    HeuristicFunction heuristic = [](v3i start, v3i end) {
        v3i delta = end - start;
        return 10 * math::abs(delta.x) + 10 * math::abs(delta.y);
    };
    bool diagonal = false;

    static const vector<v3i> directions;
};

}

