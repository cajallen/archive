#pragma once

#include "general/file/json.hpp"
#include "general/file/file_path.hpp"
#include "asset_loader.hpp"

namespace spellbook {

struct FileCache {
    umap<FilePath, json> parsed_jsons;
    umap<FilePath, AssetFile> parsed_assets;

    json& load_json(const FilePath& file_path);
    AssetFile& load_asset(const FilePath& file_path);

    vector<FilePath> load_dependencies(json& j);

};

FileCache& get_file_cache();


}