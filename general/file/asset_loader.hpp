#pragma once

#include <array>

#include "general/file/json.hpp"
#include "general/file/file_path.hpp"

using std::array;

namespace spellbook {

struct AssetFile {
    FilePath file_path;

    int            version;
    json           asset_json;
    vector<uint8>  binary_blob;
};

void      save_asset_file(const AssetFile& file);
AssetFile load_asset_file(const FilePath& path);

}
