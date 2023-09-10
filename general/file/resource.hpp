#pragma once

#include <filesystem>

#include "extension/imgui_extra.hpp"
#include "general/logger.hpp"
#include "general/file/file_path.hpp"
#include "general/file/file_cache.hpp"

namespace fs = std::filesystem;

namespace spellbook {

enum FileCategory {
    FileCategory_Json,
    FileCategory_Asset,
    FileCategory_Other
};

bool inspect_dependencies(vector<FilePath>& dependencies, const FilePath& current_path);

FilePath get_resource_folder();
FilePath resource_path(string_view val);
FilePath operator""_rp(const char* str, uint64 length);

struct Resource {
    FilePath file_path;
    vector<FilePath> dependencies;

    static constexpr string_view extension() { return ".sbgen"; }
    static constexpr string_view dnd_key() { return "DND_GENERAL"; }
    static constexpr FileCategory file_category() { return FileCategory_Json; }
    static string folder() { return (get_resource_folder()).abs_string(); }
    static std::function<bool(const fs::path&)> path_filter() { return [](const fs::path& path) { return path.extension().string() == Resource::extension(); }; }
};

template <typename T>
umap<FilePath, std::unique_ptr<T>>& cpu_resource_cache() {
    static umap<FilePath, std::unique_ptr<T>> t_cache;
    return t_cache;
}

template <typename T>
bool save_resource(const T& resource_value) {
    auto j = from_jv<json>(to_jv(resource_value));
    j["dependencies"] = make_shared<json_value>(to_jv(resource_value.dependencies));

    assert_else(resource_value.file_path.extension() == T::extension())
    return false;

    cpu_resource_cache<T>()[resource_value.file_path] = std::make_unique<T>(resource_value);
    file_dump(j, resource_value.file_path.abs_string());
    return true;
}

// Usually the ref will want to be copied
template <typename T>
T& load_resource(const FilePath& file_path, bool assert_exists = false, bool clear_cache = false) {
    fs::path absolute_path = file_path.abs_path();
    if (clear_cache && cpu_resource_cache<T>().contains(file_path))
        cpu_resource_cache<T>().erase(file_path);
    else if (cpu_resource_cache<T>().contains(file_path))
        return *cpu_resource_cache<T>()[file_path];

    bool exists = fs::exists(absolute_path);
    string ext = absolute_path.extension().string();
    bool corrext = ext == T::extension();
    if (assert_exists) {
        assert_else(exists && corrext)
        return *cpu_resource_cache<T>().emplace(file_path, std::make_unique<T>()).first->second;
    } else {
        check_else(exists && corrext)
        return *cpu_resource_cache<T>().emplace(file_path, std::make_unique<T>()).first->second;
    }

    json& j = FileCache::get().load_json(file_path);

    T& t = *cpu_resource_cache<T>().emplace(file_path, std::make_unique<T>(from_jv<T>(to_jv(j)))).first->second;
    t.dependencies = FileCache::get().load_dependencies(j);
    t.file_path = file_path;
    return t;
}

}

namespace ImGui {

template <typename T>
bool PathSelect(const string& hint, spellbook::FilePath* out, const spellbook::FilePath& base_folder, int open_subdirectories = 1, const std::function<void(const fs::path&)>& context_callback = {}) {
    fs::path out_path = out->abs_path();
    bool changed = PathSelect(hint, &out_path, base_folder.abs_string(), T::path_filter(), string(T::dnd_key()), open_subdirectories, context_callback);
    if (changed) {
        *out = spellbook::FilePath(out_path);
        return true;
    }
    return false;
}
template <typename T>
bool PathSelect(const string& hint, spellbook::FilePath* out, int open_subdirectories = 1, const std::function<void(const fs::path&)>& context_callback = {}) {
    fs::path out_path = out->abs_path();
    bool changed = PathSelect(hint, &out_path, string(T::folder()), T::path_filter(), string(T::dnd_key()), open_subdirectories, context_callback);
    if (changed) {
        *out = spellbook::FilePath(out_path);
        return true;
    }
    return false;
}

}