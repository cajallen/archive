﻿#pragma once

#include "extension/imgui_extra.hpp"
#include "general/logger.hpp"
#include "general/memory.hpp"
#include "general/function.hpp"
#include "general/file/file_path.hpp"
#include "general/file/file_cache.hpp"

namespace spellbook {

enum FileCategory {
    FileCategory_Json,
    FileCategory_Asset,
    FileCategory_Other
};

bool inspect_dependencies(vector<FilePath>& dependencies, const FilePath& current_path);

FilePath get_resource_folder();
FilePath get_external_resource_folder();
FilePath resource_path(string_view val);
FilePath operator""_resource(const char* str, uint64 length);

struct Resource {
    FilePath file_path;
    vector<FilePath> dependencies;

    static constexpr string_view extension() { return ".sbjgen"; }
    static constexpr string_view dnd_key() { return "DND_GENERAL"; }
    static FilePath folder() { return get_resource_folder(); }
    static function<bool(const FilePath&)> path_filter() { return [](const FilePath& path) { return path.extension() == Resource::extension(); }; }
};

struct Directory {
    static constexpr string_view extension() { return "?"; }
    static constexpr string_view dnd_key() { return "DND_DIRECTORY"; }
    static FilePath folder() { return get_resource_folder(); }
    static function<bool(const FilePath&)> path_filter() { return [](const FilePath& path) { return path.is_directory(); }; }
};

template <typename T>
umap<FilePath, unique_ptr<T>>& cpu_resource_cache() {
    static umap<FilePath, unique_ptr<T>> t_cache;
    return t_cache;
}

template <typename T>
bool save_resource(const T& resource_value) {
    auto j = from_jv<json>(to_jv(resource_value));
    j["dependencies"] = make_shared<json_value>(to_jv(resource_value.dependencies));

    assert_else(resource_value.file_path.extension() == T::extension())
    return false;

    cpu_resource_cache<T>()[resource_value.file_path] = make_unique<T>(resource_value);
    file_dump(j, resource_value.file_path.abs_string());
    return true;
}

// Usually the ref will want to be copied
template <typename T>
T& load_resource(const FilePath& file_path, bool assert_exists = false, bool clear_cache = false) {
    if (clear_cache && cpu_resource_cache<T>().contains(file_path))
        cpu_resource_cache<T>().erase(file_path);
    else if (cpu_resource_cache<T>().contains(file_path))
        return *cpu_resource_cache<T>()[file_path];

    bool exists = file_path.exists();
    string ext = file_path.extension();
    bool corrext = ext == T::extension();
    if (assert_exists) {
        assert_else(exists && corrext)
        return *cpu_resource_cache<T>().emplace(file_path, std::make_unique<T>()).first->second;
    } else {
        check_else(exists && corrext)
        return *cpu_resource_cache<T>().emplace(file_path, std::make_unique<T>()).first->second;
    }

    json& j = get_file_cache().load_json(file_path);

    T& t = *cpu_resource_cache<T>().emplace(file_path, make_unique<T>(from_jv<T>(to_jv(j)))).first->second;
    t.dependencies = get_file_cache().load_dependencies(j);
    t.file_path = file_path;
    return t;
}

}

namespace ImGui {

template <typename T>
bool PathSelect(const char* hint, spellbook::FilePath* out, const spellbook::FilePath& base_folder, int open_subdirectories = 1, const spellbook::function<void(const FilePath&)>& context_callback = {}) {
    return PathSelect(hint, out, base_folder, T::path_filter(), T::dnd_key().data(), open_subdirectories, context_callback);
}
template <typename T>
bool PathSelect(const char* hint, spellbook::FilePath* out, int open_subdirectories = 1, const spellbook::function<void(const FilePath&)>& context_callback = {}) {
    return PathSelect(hint, out, T::folder(), T::path_filter(), T::dnd_key().data(), open_subdirectories, context_callback);
}

}