#pragma once

#include <robin_hood.h>

#include "json.hpp"

namespace spellbook {

struct FilePath {
    string value;

    // for systems that require file inputs, but allow manual user inputs with no corresponding file
    bool symbolic = false;

    FilePath(); // defaults to root dir
    explicit FilePath(string_view val, bool symbolic = false);
    explicit FilePath(const string& val, bool symbolic = false);
    explicit FilePath(const fs::path& val);
    explicit FilePath(const char* val, bool symbolic = false);

    string abs_string() const;
    fs::path abs_path() const;

    string_view rel_string_view() const;
    const string& rel_string() const;
    fs::path rel_path() const;
    const char* rel_c_str() const;

    bool is_file() const;
    string extension() const;
    string stem() const;

    bool operator==(const FilePath& rhs) const {
        // symbolic doesn't distinguish
        return value == rhs.value;
    }

    FilePath operator + (string_view rhs) const;

    inline static string root_dir_value;
    void standardize();
    void standardize(string& s, bool directory) const;
    const string& root_dir() const;
};

FilePath from_jv_impl(const json_value& jv, FilePath* _);
json_value to_jv(const FilePath& value);

uint64 hash_path(const FilePath& file_path);

FilePath operator""_fp(const char* str, uint64 length);

string get_contents(const FilePath& file, bool binary = false);
vector<uint32> get_contents_uint32(const FilePath& file, bool binary = true);

}

template<>
struct std::hash<spellbook::FilePath> {
    std::size_t operator()(const spellbook::FilePath& v) const noexcept {
        return robin_hood::hash<std::string>{}(v.value);
    }
};
