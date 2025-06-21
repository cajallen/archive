#pragma once

#include <plf_colony.h>
#include <imgui.h>
#include <vuk/Types.hpp>
#include <vuk/Value.hpp>

#include "general/string.hpp"

namespace spellbook {

struct ImGuiData {
	vuk::Unique<vuk::Image> font_image;
	vuk::Unique<vuk::ImageView> font_image_view;
	vuk::SamplerCreateInfo font_sci;
	vuk::ImageAttachment font_ia;
	std::vector<vuk::Value<vuk::SampledImage>> sampled_images;

    ImTextureID add_sampled_image(vuk::Value<vuk::SampledImage> sampled_image);
	ImTextureID add_image(vuk::Value<vuk::ImageAttachment> image);
};

struct ImGuiShaderInfo {
    string vert_path;
    vector<uint32> vert_contents;
    string frag_path;
    vector<uint32> frag_contents;
};
ImGuiData ImGui_ImplVuk_Init(vuk::Allocator& allocator, vuk::Compiler& compiler, const ImGuiShaderInfo& shader_info);
vuk::Value<vuk::ImageAttachment> ImGui_ImplVuk_Render(vuk::Allocator& allocator, vuk::Value<vuk::ImageAttachment> target, ImGuiData& data, vuk::Compiler& compiler);

}