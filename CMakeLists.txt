
add_library(archive
    extension/glfw.cpp
    extension/imgui_extra.cpp
    extension/vuk_extra.cpp
    extension/vuk_imgui.cpp
    general/math/geometry.cpp
    general/math/quaternion.cpp
    general/math/matrix.cpp
    general/math/math.cpp
    general/math/matrix_math.cpp
    general/math/noise.cpp
    general/astar.cpp
    general/bitmask_3d.cpp
    general/color.cpp
    general/logger.cpp
    general/navigation_path.cpp
    general/input.cpp
    general/file/asset_loader.cpp
    general/file/file_cache.cpp
    general/file/file_path.cpp
    general/file/json.cpp
    general/file/resource.cpp
)

target_compile_definitions(archive PUBLIC
    DISTRIBUTED_DIR=\"${DISTRIBUTED_DIR}\"
)

target_include_directories(archive PUBLIC .)
target_link_libraries(archive PUBLIC libs)
target_precompile_headers(archive PUBLIC general/global.hpp)