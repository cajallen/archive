#include "file_cache.hpp"

#include "extension/fmt.hpp"
#include "general/logger.hpp"

namespace spellbook {

constexpr bool print_file_load_info = false;

FileCache& get_file_cache() {
    static FileCache file_cache;
    return file_cache;
}

json& FileCache::load_json(const FilePath& file_path) {
    if (parsed_jsons.contains(file_path))
        return parsed_jsons[file_path];

    if (print_file_load_info)
        log(BasicMessage{.str=fmt_("Loading json: {}", file_path.abs_string()), .group = "asset"});
    parsed_jsons[file_path] = parse_file(file_path.abs_string());
    load_dependencies(parsed_jsons[file_path]);
    return parsed_jsons[file_path];
}

AssetFile& FileCache::load_asset(const FilePath& file_path) {
    if (parsed_assets.contains(file_path))
        return parsed_assets[file_path];

    if (print_file_load_info)
        log(BasicMessage{.str=fmt_("Loading asset: {}", file_path.abs_string()), .group = "asset"});
    return parsed_assets[file_path] = load_asset_file(file_path);
}

vector<FilePath> FileCache::load_dependencies(json& j) {
    vector<FilePath> list;
    if (j.contains("dependencies")) {
        for (const json_value& jv : j["dependencies"]->get_list()) {
            const FilePath& file_path = list.push_back(from_jv_impl(jv, (FilePath*) 0));

            if (file_path.extension().starts_with(".sba"))
                this->load_asset(file_path);
            else if (file_path.extension().starts_with(".sbj"))
                this->load_json(file_path);
        }
    }
    return list;
}

}