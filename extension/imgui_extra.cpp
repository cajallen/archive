#include "imgui_extra.hpp"

#include <filesystem>

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

#include "extension/fmt.hpp"
#include "extension/icons/font_awesome4.h"

namespace fs = std::filesystem;

namespace ImGui {

bool DragMat3(const string& name, spellbook::m33* matrix, float speed, const string& format) {
    bool changed = false;

    BeginGroup();
    PushID(name.c_str());
    Text("%s", name.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat3("##row0", &matrix->cr(0, 0), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat3("##row1", &matrix->cr(0, 1), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat3("##row2", &matrix->cr(0, 2), speed, 0.0f, 0.0f, format.c_str());
    PopID();
    EndGroup();

    return changed;
}

bool DragMat4(const string& name, spellbook::m44* matrix, float speed, const string& format) {
    bool changed = false;

    BeginGroup();
    PushID(name.c_str());
    Text("%s", name.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row0", &matrix->cr(0, 0), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row1", &matrix->cr(0, 1), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row2", &matrix->cr(0, 2), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row3", &matrix->cr(0, 3), speed, 0.0f, 0.0f, format.c_str());
    PopID();
    EndGroup();

    return changed;
}

bool DragMat4(const string& name, spellbook::m44GPU* matrix_gpu, float speed, const string& format) {
    bool changed = false;

    spellbook::m44 matrix = (spellbook::m44) *matrix_gpu;
    
    BeginGroup();
    PushID(name.c_str());
    Text("%s", name.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row0", &matrix.cr(0, 0), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row1", &matrix.cr(0, 1), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row2", &matrix.cr(0, 2), speed, 0.0f, 0.0f, format.c_str());
    SetNextItemWidth(-FLT_MIN);
    changed |= DragFloat4("##row3", &matrix.cr(0, 3), speed, 0.0f, 0.0f, format.c_str());
    PopID();
    EndGroup();

    if (changed) {
        *matrix_gpu = (spellbook::m44GPU) matrix;
    }
    
    return changed;
}


bool InspectFile(const FilePath& path, FilePath* p_selected, const spellbook::function<void(const FilePath&)>& context_callback) {
    bool changed = false;
    bool selected = p_selected ? *p_selected == path : false;
    if (Selectable(path.filename().c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_DontClosePopups, ImVec2(GetContentRegionAvail().x, GetFrameHeight()))) {
        *p_selected = path;
        changed = true;
    }

    if (context_callback && BeginPopupContextItem()) {
        context_callback(path);
        EndPopup();
    }
    
    PathSource(path);
    return changed;
} 

bool InspectDirectory(const FilePath& path, FilePath* p_selected, const spellbook::function<bool(const FilePath&)>& filter, int open_subdirectories, const spellbook::function<void(const FilePath&)>& context_callback) {
    bool changed = false;
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (p_selected && path == *p_selected)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    SetNextItemOpen(open_subdirectories > 0, ImGuiCond_Once);

    fs::path fs_path = path.abs_path();
    string dir_string = fs_path.has_stem() ? fs_path.stem().string() : path.rel_string();
    if (dir_string.empty())
        dir_string = fs_path.parent_path().stem().string();
    bool node_open = TreeNodeEx(dir_string.c_str(), node_flags);
    if (IsItemClicked() && !IsItemToggledOpen() && p_selected) {
        *p_selected = path;
        changed = true;
    }
    
    PathSource(path);

    if (node_open) {
        for (auto& dir_entry : fs::directory_iterator(path.abs_path())) {
            string dir_string = dir_entry.path().string();
            bool   hidden     = dir_entry.path().stem().string().starts_with('.');
            if (hidden && dir_string != ".")
                continue;

            if (dir_entry.is_directory())
                changed |= InspectDirectory(FilePath(dir_entry), p_selected, filter, open_subdirectories - 1, context_callback);
        }
        for (auto& dir_entry : fs::directory_iterator(path.abs_path())) {
            FilePath entry_path = FilePath(dir_entry.path());
            bool hidden = dir_entry.path().stem().string().starts_with('.');
            if (hidden)
                continue;

            if (filter) {
                if (dir_entry.is_regular_file() && filter(entry_path))
                    changed |= InspectFile(entry_path, p_selected, context_callback);
            }
            else if (dir_entry.is_regular_file())
                changed |= InspectFile(entry_path, p_selected, context_callback);
        }
        TreePop();
    }
    return changed;
}


void PathSource(const FilePath& in_path, const char* dnd_key) {
    if (strlen(dnd_key) == 0) {
        return;
        __debugbreak();
    }
    
    if (BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        string full_string = in_path.abs_string();
        SetDragDropPayload(dnd_key, full_string.c_str(), full_string.size());
        Text("%s", in_path.rel_c_str());
        EndDragDropSource();
    }
}

void PathTarget(FilePath* out, const char* dnd_key) {
    if (BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = AcceptDragDropPayload(dnd_key)) {
            string out_string;
            out_string.resize(payload->DataSize);
            strncpy(out_string.data(), (char*) payload->Data, payload->DataSize);
            *out = FilePath(out_string);
        }
        EndDragDropTarget();
    }
}

bool PathSelect(const char* hint, FilePath* out, const FilePath& base_folder, spellbook::function<bool(const FilePath&)> path_filter, const char* dnd_key, int open_subdirectories, const spellbook::function<void(const FilePath&)>& context_callback) {
    if (!base_folder.exists())
        create_directories(base_folder);

    bool has_width = false;
    float desired_width = 0.0f;
    if (GImGui->NextItemData.Flags & ImGuiNextItemDataFlags_HasWidth) {
        float hint_width = ImGui::CalcTextSize(hint, 0, true).x;
        desired_width = GImGui->NextItemData.Width - hint_width;
        has_width = true;
        GImGui->NextItemData.Flags &= ~ImGuiNextItemDataFlags_HasWidth;
    }

    string s_hint = string(hint);
    string vis_hint = string(s_hint, 0, s_hint.find_first_of("##"));
    bool changed = false;
    PushID(hint);
    BeginGroup();
    {
        if (Button(ICON_FA_FOLDER))
            OpenPopup("File Select");
        SameLine();
        string out_string = out->rel_string();
        if (has_width) {
            ImGui::SetNextItemWidth(desired_width - 80.0f);
        } else {
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 180.0f);
        }
        string used_hint = fmt_("##{}", hint);
        if (InputText(used_hint.c_str(), &out_string)) {
            *out = FilePath(out_string, out->location);
            changed = true;
        }
        SameLine();
        ImGui::SetNextItemWidth(80.0f);
        changed |= EnumCombo("##FileSelectType", &out->location);
        SameLine();
        Text("%s", vis_hint.c_str());
    }
    EndGroup();
    PathTarget(out, dnd_key);

    bool open = true;
    
    SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
    SetNextWindowPos(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    if (BeginPopupModal("File Select", &open)) {
        bool close_popup = false;
        changed |= PathSelectBody(out, base_folder, path_filter, &close_popup, open_subdirectories, context_callback);
        if (close_popup)
            CloseCurrentPopup();
        EndPopup();
    }
    PopID();
    
    return changed;
}

bool PathSelectBody(FilePath* out, const FilePath& base_folder, const spellbook::function<bool(const FilePath&)>& filter, bool* close_popup, int open_subdirectories, const spellbook::function<void(const FilePath&)>& context_callback) {
    spellbook::v2 size = close_popup == nullptr ?
        spellbook::v2(GetContentRegionAvail()) :
        spellbook::v2(GetContentRegionAvail()) - spellbook::v2(0, GetFrameHeightWithSpacing());
    BeginChild("Directory", ImVec2(size));
    SetNextItemOpen(true, ImGuiCond_Appearing);
    bool changed = InspectDirectory(base_folder, out, filter, open_subdirectories, context_callback);
    EndChild();

    if (close_popup != nullptr)
        *close_popup = Button("Select", ImVec2(-FLT_MIN, 0));

    return changed;
}

void StyleColorsSpellbook(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = (ImVec4) spellbook::Color(spellbook::palette::white, 1.0f);
    colors[ImGuiCol_TextDisabled]           = (ImVec4) spellbook::Color(spellbook::palette::gray, 1.0f); // ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // grey
    colors[ImGuiCol_WindowBg]               = (ImVec4) spellbook::Color(spellbook::palette::near_black, 0.96f); // ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // dark
    colors[ImGuiCol_ChildBg]                = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // empty
    colors[ImGuiCol_PopupBg]                = (ImVec4) spellbook::Color(spellbook::palette::near_black, 0.95f); // ImVec4(0.08f, 0.08f, 0.08f, 0.94f); // dark
    colors[ImGuiCol_Border]                 = (ImVec4) spellbook::Color(spellbook::palette::spellbook_gray, 0.5f); // ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = (ImVec4) spellbook::Color(spellbook::palette::spellbook_7, 0.6f); // ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.4f); // ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.7f); // ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = (ImVec4) spellbook::Color(spellbook::palette::near_black, 1.0f); // ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = (ImVec4) spellbook::Color(spellbook::palette::spellbook_7, 1.0f); // ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = (ImVec4) spellbook::Color(spellbook::palette::black, 0.5f); // ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = (ImVec4) spellbook::Color(spellbook::palette::gray_1, 1.0f); // ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = (ImVec4) spellbook::Color(spellbook::palette::black, 0.5f); // ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = (ImVec4) spellbook::Color(spellbook::palette::gray_3, 1.0f); // ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = (ImVec4) spellbook::Color(spellbook::palette::gray_4, 1.0f); // ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = (ImVec4) spellbook::Color(spellbook::palette::gray_5, 1.0f); // ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = (ImVec4) spellbook::Color(spellbook::palette::spellbook_5, 1.0f); // ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.4f); // ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = (ImVec4) spellbook::Color(spellbook::palette::spellbook_6, 1.0f); // ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.3f); // ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.8f); // ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = (ImVec4) spellbook::Color(spellbook::palette::spellbook_7, 0.8f); // ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = (ImVec4) spellbook::Color(spellbook::palette::spellbook_7, 1.0f); // ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.2f); // ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.7f); // ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.9f); // ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_DockingPreview]         = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = (ImVec4) spellbook::Color(spellbook::palette::gray_6, 1.0f); // ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = (ImVec4) spellbook::Color(spellbook::palette::tomato, 1.0f); // ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = (ImVec4) spellbook::Color(spellbook::palette::dark_orange, 1.0f); // ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = (ImVec4) spellbook::Color(spellbook::palette::orange, 1.0f); // ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = (ImVec4) spellbook::Color(spellbook::palette::gray_3, 1.0f); // ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = (ImVec4) spellbook::Color(spellbook::palette::white, 0.05f); // ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 0.3f); // ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = (ImVec4) spellbook::Color(spellbook::palette::yellow_green, 0.8f); // ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = (ImVec4) spellbook::Color(spellbook::palette::spellbook_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = (ImVec4) spellbook::Color(spellbook::palette::white, 0.7f); // ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = (ImVec4) spellbook::Color(spellbook::palette::gray_8, 0.2f); // ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = (ImVec4) spellbook::Color(spellbook::palette::gray_8, 0.4f); // ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
}

void StyleColorsAcademy(ImGuiStyle* dst)
{
    ImGuiStyle* style = dst ? dst : &GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text]                   = (ImVec4) spellbook::Color(spellbook::palette::white, 1.0f);
    colors[ImGuiCol_TextDisabled]           = (ImVec4) spellbook::Color(spellbook::palette::gray, 1.0f); // ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // grey
    colors[ImGuiCol_WindowBg]               = (ImVec4) spellbook::Color(spellbook::palette::near_black, 0.96f); // ImVec4(0.06f, 0.06f, 0.06f, 0.94f); // dark
    colors[ImGuiCol_ChildBg]                = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f); // empty
    colors[ImGuiCol_PopupBg]                = (ImVec4) spellbook::Color(spellbook::palette::near_black, 0.95f); // ImVec4(0.08f, 0.08f, 0.08f, 0.94f); // dark
    colors[ImGuiCol_Border]                 = (ImVec4) spellbook::Color(spellbook::palette::academy_gray, 0.5f); // ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = (ImVec4) spellbook::Color(spellbook::palette::academy_7, 0.6f); // ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.4f); // ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.7f); // ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = (ImVec4) spellbook::Color(spellbook::palette::near_black, 1.0f); // ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = (ImVec4) spellbook::Color(spellbook::palette::academy_7, 1.0f); // ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = (ImVec4) spellbook::Color(spellbook::palette::black, 0.5f); // ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = (ImVec4) spellbook::Color(spellbook::palette::gray_1, 1.0f); // ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = (ImVec4) spellbook::Color(spellbook::palette::black, 0.5f); // ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = (ImVec4) spellbook::Color(spellbook::palette::gray_3, 1.0f); // ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = (ImVec4) spellbook::Color(spellbook::palette::gray_4, 1.0f); // ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = (ImVec4) spellbook::Color(spellbook::palette::gray_5, 1.0f); // ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = (ImVec4) spellbook::Color(spellbook::palette::academy_5, 1.0f); // ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.4f); // ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = (ImVec4) spellbook::Color(spellbook::palette::academy_6, 1.0f); // ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.3f); // ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.8f); // ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = (ImVec4) spellbook::Color(spellbook::palette::academy_7, 0.8f); // ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = (ImVec4) spellbook::Color(spellbook::palette::academy_7, 1.0f); // ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.2f); // ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.7f); // ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.9f); // ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_DockingPreview]         = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.7f);
    colors[ImGuiCol_DockingEmptyBg]         = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = (ImVec4) spellbook::Color(spellbook::palette::gray_6, 1.0f); // ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = (ImVec4) spellbook::Color(spellbook::palette::tomato, 1.0f); // ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = (ImVec4) spellbook::Color(spellbook::palette::dark_orange, 1.0f); // ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = (ImVec4) spellbook::Color(spellbook::palette::orange, 1.0f); // ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = (ImVec4) spellbook::Color(spellbook::palette::gray_3, 1.0f); // ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight]       = (ImVec4) spellbook::Color(spellbook::palette::gray_2, 1.0f); // ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg]             = (ImVec4) spellbook::Color(spellbook::palette::black, 0.0f); // ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = (ImVec4) spellbook::Color(spellbook::palette::white, 0.05f); // ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 0.3f); // ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = (ImVec4) spellbook::Color(spellbook::palette::yellow_green, 0.8f); // ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = (ImVec4) spellbook::Color(spellbook::palette::academy_4, 1.0f); // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = (ImVec4) spellbook::Color(spellbook::palette::white, 0.7f); // ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = (ImVec4) spellbook::Color(spellbook::palette::gray_8, 0.2f); // ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = (ImVec4) spellbook::Color(spellbook::palette::gray_8, 0.4f); // ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
    colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabActive]              = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused]           = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f);
}

bool DragEuler2(const char* label, spellbook::euler* e, bool input_is_radians, float v_speed, ImGuiSliderFlags flags) {
    if (input_is_radians) {
        spellbook::euler e_deg = input_is_radians ? spellbook::math::r2d(*e) : *e;
        PushMultiItemsWidths(2, CalcItemWidth());
        ImGui::PushID(label);
        ImGui::PushID(0);
        bool changed = ImGui::DragFloat("", &e_deg.yaw, v_speed, -FLT_MAX, FLT_MAX, "yaw: %.1fd", flags);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(1);
        changed |= ImGui::DragFloat("", &e_deg.pitch, v_speed, -FLT_MAX, FLT_MAX, "pitch: %.1fd", flags);
        ImGui::PopID();
        ImGui::PopID();
        if (changed) {
            *e = spellbook::math::d2r(e_deg);
            return true;
        }
        return false;
    }

    PushMultiItemsWidths(2, CalcItemWidth());
    ImGui::PushID(label);
    ImGui::PushID(0);
    bool changed = ImGui::DragFloat("", &e->yaw, v_speed, -FLT_MAX, FLT_MAX, "yaw: %.1fd", flags);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(1);
    changed |= ImGui::DragFloat("", &e->pitch, v_speed, -FLT_MAX, FLT_MAX, "pitch: %.1fd", flags);
    ImGui::PopID();
    ImGui::PopID();
    return changed;
}

bool DragEuler3(const char* label, spellbook::euler* e, bool input_is_radians, float v_speed, ImGuiSliderFlags flags) {
    if (input_is_radians) {
        spellbook::euler e_deg = input_is_radians ? spellbook::math::r2d(*e) : *e;
        PushMultiItemsWidths(3, CalcItemWidth());
        ImGui::PushID(label);
        ImGui::PushID(0);
        bool changed = ImGui::DragFloat("", &e_deg.yaw, v_speed, -FLT_MAX, FLT_MAX, "yaw: %.1fd", flags);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(1);
        changed |= ImGui::DragFloat("", &e_deg.pitch, v_speed, -FLT_MAX, FLT_MAX, "pitch: %.1fd", flags);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::PushID(2);
        changed |= ImGui::DragFloat("", &e_deg.roll, v_speed, -FLT_MAX, FLT_MAX, "roll: %.1fd", flags);
        ImGui::PopID();
        ImGui::PopID();
        if (changed) {
            *e = spellbook::math::d2r(e_deg);
            return true;
        }
        return false;
    }

    PushMultiItemsWidths(3, CalcItemWidth());
    ImGui::PushID(label);
    ImGui::PushID(0);
    bool changed = ImGui::DragFloat("", &e->yaw, v_speed, -FLT_MAX, FLT_MAX, "yaw: %.1fd", flags);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(1);
    changed |= ImGui::DragFloat("", &e->pitch, v_speed, -FLT_MAX, FLT_MAX, "pitch: %.1fd", flags);
    ImGui::PopID();
    ImGui::SameLine();
    ImGui::PushID(2);
    changed |= ImGui::DragFloat("", &e->roll, v_speed, -FLT_MAX, FLT_MAX, "roll: %.1fd", flags);
    ImGui::PopID();
    ImGui::PopID();
    return changed;
}

}



