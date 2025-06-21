#include "vuk_extra.hpp"

#include <vuk/Context.hpp>

namespace vuk {

Texture allocate_texture(Allocator& allocator, Format format, Extent3D extent) {
    ImageCreateInfo ici;
    ici.format = format;
    ici.extent = extent;
    ici.samples = Samples::e1;
    ici.initialLayout = ImageLayout::eUndefined;
    ici.tiling        = ImageTiling::eOptimal;
    ici.usage         = ImageUsageFlagBits::eStorage | ImageUsageFlagBits::eTransferDst | ImageUsageFlagBits::eSampled;
    ici.mipLevels = 1;
    ici.arrayLayers = 1;
    auto tex = allocator.get_context().allocate_texture(allocator, ici);
    return std::move(tex);
}

}