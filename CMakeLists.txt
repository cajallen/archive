
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
    general/astar.cpp
    general/bitmask_3d.cpp
    general/color.cpp
    general/file.cpp
    general/json.cpp
    general/logger.cpp
    general/path.cpp
)

target_include_directories(archive PUBLIC .)
target_link_libraries(archive PUBLIC libs)
target_precompile_headers(archive PUBLIC general/global.hpp)