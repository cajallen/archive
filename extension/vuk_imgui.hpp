#pragma once

#include <vuk/Types.hpp>
#include <vuk/Image.hpp>
#include <vuk/SampledImage.hpp>
#include <plf_colony.h>
#include <imgui.h>

#include "general/string.hpp"

namespace spellbook {

struct ImGuiData {
    vuk::Texture                       font_texture;
    vuk::SamplerCreateInfo             font_sci;
    std::unique_ptr<vuk::SampledImage> font_si;
};

struct ImGuiShaderInfo {
    string vert_path;
    vector<uint32> vert_contents;
    string frag_path;
    vector<uint32> frag_contents;
};
ImGuiData   ImGui_ImplVuk_Init(vuk::Allocator& allocator, vuk::Compiler& compiler, const ImGuiShaderInfo& shader_info);
vuk::Future ImGui_ImplVuk_Render(vuk::Allocator& allocator, vuk::Future target, ImGuiData& data, ImDrawData* draw_data, const plf::colony<vuk::SampledImage>& sampled_images, vuk::Compiler& compiler);

}