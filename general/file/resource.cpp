#include "resource.hpp"

namespace spellbook {

bool inspect_dependencies(vector<FilePath>& dependencies, const FilePath& current_path) {
    bool changed = false;

    if (ImGui::TreeNode("Dependencies")) {
        if (ImGui::Button("Auto Populate")) {
            string contents = get_contents(current_path);

            uint64 start_at = 0;
            while (start_at < contents.size()) {
                uint64 sb_index = contents.find(".sb", start_at);
                if (sb_index == string::npos)
                    break;
                uint64 start_quote = contents.rfind('"', sb_index) + 1;
                uint64 end_quote = contents.find_first_of('"', sb_index);
                string quote_contents = string(contents.data() + start_quote, end_quote - start_quote);
                dependencies.push_back(FilePath(quote_contents));

                start_at = end_quote;
            }

        }
        changed |= ImGui::UnorderedVector(dependencies,
            [](FilePath& dep) {
                float width = ImGui::GetContentRegionAvail().x;
                float text_width = ImGui::CalcTextSize("Path").x;
                ImGui::SetNextItemWidth(width - 8.f - text_width);
                return ImGui::PathSelect<Resource>("Path", &dep);
            },
            [](vector<FilePath>& deps, bool pressed) {
                if (pressed) {
                    deps.emplace_back();
                }
            },
            true);
        ImGui::TreePop();
    }
    return changed;
}

FilePath get_resource_folder() { return "resources"_content; }
FilePath get_external_resource_folder() { return "external"_content; }
FilePath resource_path(string_view val) { return get_resource_folder() + string(val); }
FilePath operator""_resource(const char* str, uint64 length) { return resource_path(string_view{str, length}); }

}