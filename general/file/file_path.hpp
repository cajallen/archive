#pragma once

#include <robin_hood.h>

#include "json.hpp"

namespace spellbook {

enum FilePathLocation {
    FilePathLocation_Content,
    FilePathLocation_Config,
    FilePathLocation_Symbolic,
    FilePathLocation_Distributed
};

struct FilePath {
    string value;
    FilePathLocation location = FilePathLocation_Content;

    FilePath(); // defaults to root dir
    explicit FilePath(string_view val, FilePathLocation location = FilePathLocation_Content);
    explicit FilePath(const string& val, FilePathLocation location = FilePathLocation_Content);
    explicit FilePath(const fs::path& val, FilePathLocation location = FilePathLocation_Content);
    explicit FilePath(const char* val, FilePathLocation location = FilePathLocation_Content);

    string abs_string() const;
    fs::path abs_path() const;

    string_view rel_string_view() const;
    const string& rel_string() const;
    fs::path rel_path() const;
    const char* rel_c_str() const;

    bool is_file() const;
    string filename() const;
    string extension() const;
    string stem() const;
    fs::path parent_path() const;

    bool operator==(const FilePath& rhs) const {
        // symbolic doesn't distinguish
        return value == rhs.value;
    }

    FilePath operator + (string_view rhs) const;

    string root_dir() const;
    void standardize();
    static void standardize(string& s, bool directory);
};

FilePath from_jv_impl(const json_value& jv, FilePath* _);
json_value to_jv(const FilePath& value);

uint64 hash_path(const FilePath& file_path);

FilePath operator""_content(const char* str, uint64 length);
FilePath operator""_symbolic(const char* str, uint64 length);
FilePath operator""_config(const char* str, uint64 length);
FilePath operator""_distributed(const char* str, uint64 length);

string get_contents(const FilePath& file, bool binary = false);
vector<uint32> get_contents_uint32(const FilePath& file, bool binary = true);

const FilePath& get_config_path();
const string& get_default_content_path();
string& get_appdata_local_path();
string& get_distributed_dir_path();
string& get_content_dir_path();
void set_content_dir_path(string_view abs_path);

string& get_filepath_app_name();

}

template<>
struct std::hash<spellbook::FilePath> {
    std::size_t operator()(const spellbook::FilePath& v) const noexcept {
        return robin_hood::hash<std::string>{}(v.value);
    }
};
