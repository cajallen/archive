#include "file_path.hpp"

#include <magic_enum.hpp>

#include "general/logger.hpp"

namespace spellbook {

FilePath::FilePath() {}
FilePath::FilePath(string_view val, bool symbolic) : value(val), symbolic(symbolic) {
    standardize();
}
FilePath::FilePath(const string& val, bool symbolic) : value(val), symbolic(symbolic) {
    standardize();
}
FilePath::FilePath(const fs::path& val) : value(val.string()) {
    standardize();
}
FilePath::FilePath(const char* val, bool symbolic) : value(val), symbolic(symbolic) {
    standardize();
}

string FilePath::abs_string() const {
    if (symbolic)
        return value;
    return root_dir() + value;
}
fs::path FilePath::abs_path() const {
    return fs::path(abs_string());
}

string_view FilePath::rel_string_view() const {
    return string_view(value);
}
const string& FilePath::rel_string() const {
    return value;
}
fs::path FilePath::rel_path() const {
    return fs::path(value);
}
const char* FilePath::rel_c_str() const {
    return value.c_str();
}

bool FilePath::is_file() const {
    if (symbolic)
        return !value.empty();
    return value.find_first_of('.') != string::npos;
}
string FilePath::extension() const {
    return rel_path().extension().string();
}
string FilePath::stem() const {
    return rel_path().stem().string();
}

void FilePath::standardize() {
    if (symbolic)
        return;
    standardize(value, false);
    if (value.starts_with(root_dir()))
        value = value.substr(root_dir().size());

    // These aren't supported.
    sb_assert(value.find("..") == string::npos && value.find("./") == string::npos);
}
void FilePath::standardize(string& s, bool directory) const {
    std::replace(s.begin(), s.end(), '\\', '/');
    if (directory)
        if (s.back() != '/')
            s.push_back('/');
}

const string& FilePath::root_dir() const {
    if (root_dir_value.empty()) {
#ifdef RESOURCE_PARENT_DIR
        fs::path p(RESOURCE_PARENT_DIR);
#else
        fs::path p = fs::current_path();
#endif
        root_dir_value = p.string();
        standardize(root_dir_value, true);
    }
    return root_dir_value;
}

FilePath from_jv_impl(const json_value& jv, FilePath* _) {
    string s = from_jv<string>(jv);
    // we don't save directories, so can check for extension to see if it's symbolic
    return FilePath(s, s.find('.') == std::string::npos);
}
json_value to_jv(const FilePath& value) {
    return to_jv(value.rel_string());
}

FilePath operator""_fp(const char* str, uint64 length) {
    return FilePath(std::string(str, length));
}

uint64 hash_path(const FilePath& file_path) {
    if (file_path.symbolic)
        return hash_view(file_path.value);
    return hash_view(file_path.rel_string_view());
}

}
