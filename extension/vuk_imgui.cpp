#include "vuk_imgui.hpp"

#include <vuk/RenderGraph.hpp>
#include <vuk/EmbeddedResource.hpp>

#include <vuk/runtime/CommandBuffer.hpp>
#include <vuk/runtime/vk/AllocatorHelpers.hpp>
#include <vuk/runtime/vk/VkRuntime.hpp>
#include <vuk/vsl/Core.hpp>

#include "extension/icons/font_awesome4.h"
#include "general/file/file_path.hpp"

namespace spellbook {

ImTextureID ImGuiData::add_sampled_image(vuk::Value<vuk::SampledImage> sampled_image) {
	auto idx = sampled_images.size() + 1;
	sampled_images.emplace_back(std::move(sampled_image));
	return (ImTextureID)idx;
}

ImTextureID ImGuiData::add_image(vuk::Value<vuk::ImageAttachment> image) {
	return add_sampled_image(combine_image_sampler("_simg", std::move(image), vuk::acquire_sampler("_default_sampler", {})));
}

ImGuiData ImGui_ImplVuk_Init(vuk::Allocator& allocator, vuk::Compiler& compiler, const ImGuiShaderInfo& shader_info) {
    vuk::Runtime& ctx      = allocator.get_context();
    auto&         io       = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_vuk";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.

    io.Fonts->AddFontDefault();

    if (FilePath(FONT_ICON_FILE_NAME_FA).exists()) {
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
        ImFontConfig         icons_config;
        icons_config.MergeMode   = true;
        icons_config.PixelSnapH  = true;
        icons_config.GlyphOffset = {0.0f, 2.0f};
        io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FA, 14.0f, &icons_config, icons_ranges);
    }

    unsigned char* pixels;
    v2i            dimensions;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &dimensions.x, &dimensions.y);

    ImGuiData data;
    auto      font_ia = vuk::ImageAttachment::from_preset(
        vuk::ImageAttachment::Preset::eMap2D, vuk::Format::eR8G8B8A8Srgb, vuk::Extent3D(dimensions), vuk::Samples::e1);
    auto [image, view, fut] = create_image_and_view_with_data(allocator, vuk::DomainFlagBits::eTransferOnTransfer, font_ia, pixels);
    data.font_image         = std::move(image);
    data.font_image_view    = std::move(view);

    fut.as_released(vuk::Access::eFragmentSampled, vuk::DomainFlagBits::eGraphicsQueue);
    data.font_ia        = *fut.get(allocator, compiler);
    data.font_ia.layout = vuk::ImageLayout::eReadOnlyOptimal;
    ctx.set_name(data.font_image_view->payload, "ImGui/font");
    vuk::SamplerCreateInfo sci;
    sci.minFilter = sci.magFilter = vuk::Filter::eLinear;
    sci.mipmapMode                = vuk::SamplerMipmapMode::eLinear;
    sci.addressModeU = sci.addressModeV = sci.addressModeW = vuk::SamplerAddressMode::eRepeat;
    data.font_sci                                          = sci;

    {
        vuk::PipelineBaseCreateInfo pci;

        pci.add_spirv(std::vector(shader_info.vert_contents.begin(), shader_info.vert_contents.end()), shader_info.vert_path);
        pci.add_spirv(std::vector(shader_info.frag_contents.begin(), shader_info.frag_contents.end()), shader_info.frag_path);
        ctx.create_named_pipeline("imgui", pci);
    }

    return data;
}

vuk::Value<vuk::ImageAttachment> ImGui_ImplVuk_Render(
    vuk::Allocator& allocator, vuk::Value<vuk::ImageAttachment> target, ImGuiData& data, vuk::Compiler& compiler) {
    auto draw_data = ImGui::GetDrawData();
    auto reset_render_state =
        [](const ImGuiData& data, vuk::CommandBuffer& command_buffer, ImDrawData* draw_data, vuk::Buffer vertex, vuk::Buffer index) {
            command_buffer.bind_image(0, 0, *data.font_image_view).bind_sampler(0, 0, data.font_sci);
            if (index.size > 0) {
                command_buffer.bind_index_buffer(index, sizeof(ImDrawIdx) == 2 ? vuk::IndexType::eUint16 : vuk::IndexType::eUint32);
            }
            command_buffer.bind_vertex_buffer(
                0, vertex, 0, vuk::Packed {vuk::Format::eR32G32Sfloat, vuk::Format::eR32G32Sfloat, vuk::Format::eR8G8B8A8Unorm});
            command_buffer.bind_graphics_pipeline("imgui");
            command_buffer.set_viewport(0, vuk::Rect2D::framebuffer());
            struct PC {
                float scale[2];
                float translate[2];
            } pc;
            pc.scale[0]     = 2.0f / draw_data->DisplaySize.x;
            pc.scale[1]     = 2.0f / draw_data->DisplaySize.y;
            pc.translate[0] = -1.0f - draw_data->DisplayPos.x * pc.scale[0];
            pc.translate[1] = -1.0f - draw_data->DisplayPos.y * pc.scale[1];
            command_buffer.push_constants(vuk::ShaderStageFlagBits::eVertex, 0, pc);
        };

    size_t vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
    size_t index_size  = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    auto   imvert      = *allocate_buffer(allocator, {vuk::MemoryUsage::eCPUtoGPU, vertex_size, 1});
    auto   imind       = *allocate_buffer(allocator, {vuk::MemoryUsage::eCPUtoGPU, index_size, 1});

    size_t vtx_dst = 0, idx_dst = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        auto              imverto  = imvert->add_offset(vtx_dst * sizeof(ImDrawVert));
        auto              imindo   = imind->add_offset(idx_dst * sizeof(ImDrawIdx));

        memcpy(imverto.mapped_ptr, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(imindo.mapped_ptr, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }

    // add rendergraph dependencies to be transitioned
    ImGui::GetIO().Fonts->TexID = (ImTextureID) (data.sampled_images.size() + 1);
    data.sampled_images.push_back(combine_image_sampler("imgui font",
        acquire_ia("imgui font", data.font_ia, vuk::Access::eFragmentSampled),
        acquire_sampler("font sampler", data.font_sci)));
    // make all rendergraph sampled images available
    auto sampled_images_array = declare_array("imgui_sampled", std::span(data.sampled_images));
    auto pass                 = vuk::make_pass("imgui",
        [&data, verts = imvert.get(), inds = imind.get(), draw_data, reset_render_state](vuk::CommandBuffer& command_buffer,
            VUK_IA(vuk::Access::eColorWrite) dst,
            VUK_ARG(vuk::SampledImage[], vuk::Access::eFragmentSampled) sis) {
            command_buffer.set_dynamic_state(vuk::DynamicStateFlagBits::eViewport | vuk::DynamicStateFlagBits::eScissor);
            command_buffer.set_rasterization(vuk::PipelineRasterizationStateCreateInfo {});
            command_buffer.set_color_blend(dst, vuk::BlendPreset::eAlphaBlend);
            reset_render_state(data, command_buffer, draw_data, verts, inds);
            // Will project scissor/clipping rectangles into framebuffer space
            ImVec2 clip_off   = draw_data->DisplayPos;       // (0,0) unless using multi-viewports
            ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

            // Render command lists
            // (Because we merged all buffers into a single one, we maintain our own offset into them)
            int global_vtx_offset = 0;
            int global_idx_offset = 0;
            for (int n = 0; n < draw_data->CmdListsCount; n++) {
                const ImDrawList* cmd_list = draw_data->CmdLists[n];
                for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                    const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                    if (pcmd->UserCallback != nullptr) {
                        // User callback, registered via ImDrawList::AddCallback()
                        // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset
                        // render state.)
                        if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                            reset_render_state(data, command_buffer, draw_data, verts, inds);
                        else
                            pcmd->UserCallback(cmd_list, pcmd);
                    } else {
                        // Project scissor/clipping rectangles into framebuffer space
                        ImVec4 clip_rect;
                        clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                        clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                        clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                        clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                        auto fb_width  = command_buffer.get_ongoing_render_pass().extent.width;
                        auto fb_height = command_buffer.get_ongoing_render_pass().extent.height;
                        if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
                            // Negative offsets are illegal for vkCmdSetScissor
                            if (clip_rect.x < 0.0f)
                                clip_rect.x = 0.0f;
                            if (clip_rect.y < 0.0f)
                                clip_rect.y = 0.0f;

                            // Apply scissor/clipping rectangle
                            vuk::Rect2D scissor;
                            scissor.offset.x      = (int32_t) (clip_rect.x);
                            scissor.offset.y      = (int32_t) (clip_rect.y);
                            scissor.extent.width  = (uint32_t) (clip_rect.z - clip_rect.x);
                            scissor.extent.height = (uint32_t) (clip_rect.w - clip_rect.y);
                            command_buffer.set_scissor(0, scissor);

                            // Bind texture
                            if (pcmd->TextureId) {
                                auto ia_index = uint64(pcmd->TextureId) - 1;

                                command_buffer.bind_image(0, 0, sis[ia_index].ia).bind_sampler(0, 0, sis[ia_index].sci);
                            }
                            // Draw
                            command_buffer.draw_indexed(
                                pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
                        }
                    }
                }
                global_idx_offset += cmd_list->IdxBuffer.Size;
                global_vtx_offset += cmd_list->VtxBuffer.Size;
            }

            data.sampled_images.clear();
            return dst;
        });

    return pass(target, std::move(sampled_images_array));
}

} // namespace spellbook
