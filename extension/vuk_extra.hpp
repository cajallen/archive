#pragma once

#include <vuk/Allocator.hpp>

namespace vuk {

Texture allocate_texture(Allocator& allocator, Format format, Extent3D extent);

}
