#include "file_path.hpp"

#include <magic_enum.hpp>
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>

#include "general/logger.hpp"
#include "general/file/resource.hpp"


namespace spellbook {

FilePath::FilePath() {}
FilePath::FilePath(string_view val, FilePathLocation location) : value(val), location(location) {
    standardize();
}
FilePath::FilePath(const string& val, FilePathLocation location) : value(val), location(location) {
    standardize();
}
FilePath::FilePath(const fs::path& val, FilePathLocation location) : value(val.string()), location(location) {
    standardize();
}
FilePath::FilePath(const char* val, FilePathLocation location) : value(val), location(location) {
    standardize();
}

string FilePath::abs_string() const {
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
    if (location == FilePathLocation_Symbolic)
        return !value.empty();
    return value.find_first_of('.') != string::npos;
}

string FilePath::filename() const {
    return rel_path().filename().string();
}
string FilePath::extension() const {
    return rel_path().extension().string();
}
string FilePath::stem() const {
    return rel_path().stem().string();
}

fs::path FilePath::parent_path() const {
    return this->abs_path().parent_path();
}

FilePath FilePath::operator + (string_view rhs) const {
    FilePath new_path;
    new_path.value = this->value + string(rhs);
    new_path.standardize();
    return new_path;
}

string FilePath::root_dir() const {
    switch (location) {
        case FilePathLocation_Content: {
            string path = get_content_dir_path();
            return path;
        }
        case FilePathLocation_Config: {
            string path = get_appdata_local_path();
            assert_else(!get_filepath_app_name().empty() && "File system needs app name for reasonable defaults");
            return path + get_filepath_app_name() + "/";
        }
        case FilePathLocation_Symbolic: {
            return "";
        }
        case FilePathLocation_Distributed: {
            string path = get_distributed_dir_path();
            return path;
        }
        default: assert_else(false) return "";
    }
}

void FilePath::standardize() {
    if (location == FilePathLocation_Symbolic || value.empty())
        return;
    standardize(value, !is_file());


    if (value.starts_with(root_dir()))
        value = value.substr(root_dir().size());

    // These aren't supported.
    sb_assert(value.find("..") == string::npos && value.find("./") == string::npos);
}
void FilePath::standardize(string& s, bool directory) {
    std::replace(s.begin(), s.end(), '\\', '/');
    if (directory)
        if (s.back() != '/')
            s.push_back('/');
}

FilePath from_jv_impl(const json_value& jv, FilePath* _) {
    string s = from_jv<string>(jv);
    // we don't save directories, so can check for extension to see if it's symbolic
    return FilePath(s, s.find('.') == std::string::npos ? FilePathLocation_Symbolic : FilePathLocation_Content);
}
json_value to_jv(const FilePath& value) {
    FilePath copy = value;
    copy.standardize();
    return to_jv(copy.rel_string());
}

FilePath operator""_content(const char* str, uint64 length) {
    return FilePath(std::string(str, length), FilePathLocation_Content);
}
FilePath operator""_symbolic(const char* str, uint64 length) {
    return FilePath(std::string(str, length), FilePathLocation_Symbolic);
}
FilePath operator""_config(const char* str, uint64 length) {
    return FilePath(std::string(str, length), FilePathLocation_Config);
}
FilePath operator""_distributed(const char* str, uint64 length) {
    return FilePath(std::string(str, length), FilePathLocation_Distributed);
}

uint64 hash_path(const FilePath& file_path) {
    return hash_view(file_path.rel_string_view()) ^ uint64(file_path.location);
}

string& get_appdata_local_path() {
    static string appdata_local_path;
    if (appdata_local_path.empty()) {
        PWSTR wchar_path = NULL;
        assert_else(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &wchar_path) == S_OK) {

        }
        std::wstring wstring_path(wchar_path);
        CoTaskMemFree(wchar_path);

        std::transform(wstring_path.begin(), wstring_path.end(), std::back_inserter(appdata_local_path), [] (wchar_t c) { return (char)c; });

        FilePath::standardize(appdata_local_path, true);
    }
    return appdata_local_path;
}

const string& get_default_content_path() {
    static string default_content_path;
    if (default_content_path.empty()) {
        PWSTR wchar_path = NULL;
        assert_else(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &wchar_path) == S_OK) {

        }
        std::wstring wstring_path(wchar_path);

        CoTaskMemFree(wchar_path);

        std::transform(wstring_path.begin(), wstring_path.end(), std::back_inserter(default_content_path), [] (wchar_t c) { return (char)c; });

        FilePath::standardize(default_content_path, true);
        assert_else(!get_filepath_app_name().empty() && "File system needs app name for reasonable defaults");
        default_content_path += get_filepath_app_name() + "/";
    }
    return default_content_path;
}

string& get_distributed_dir_path() {
    static string distributed_path;
    if (distributed_path.empty()) {
#ifdef DISTRIBUTED_DIR
        fs::path p(DISTRIBUTED_DIR);
#else
        fs::path p = fs::current_path();
#endif
        distributed_path = p.string();
        FilePath::standardize(distributed_path, true);
    }
    return distributed_path;
}

const FilePath& get_config_path() {
    static FilePath config_path;
    if (config_path.value.empty()) {
        config_path = FilePath("config" + string(Resource::extension()), FilePathLocation_Config);
    }
    return config_path;
}

string& get_content_dir_path() {
    static string content_dir_path;

    if (content_dir_path.empty()) {
        const FilePath& config_path = get_config_path();
        if (fs::exists(config_path.abs_path())) {
            json j = parse_file(config_path.abs_string());
            if (j.contains("content_dir")) {
                content_dir_path = from_jv<string>(*j.at("content_dir"));
                return content_dir_path;
            }
        }

        content_dir_path = get_default_content_path();
        set_content_dir_path(content_dir_path);
    }

    return content_dir_path;
}

void set_content_dir_path(string_view abs_path) {
    const FilePath& config_path = get_config_path();
    json j = fs::exists(config_path.abs_path()) ? parse_file(config_path.abs_string()) : json{};
    j["content_dir"] = make_shared<json_value>(to_jv(string(abs_path)));
    file_dump(j, config_path.abs_string());
}

string get_contents(const FilePath& file_name, bool binary) {
    string path = file_name.abs_string();
    FILE* f = fopen(path.c_str(), !binary ? "r" : "rb");
    if (f == nullptr)
        return {};

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f) / sizeof(char);
    string contents;
    contents.resize(size);
    rewind(f);

    size_t read_bytes = fread(contents.data(), sizeof(char), size, f);

    fclose(f);

    contents.resize(std::min(strlen(contents.data()), read_bytes));
    return contents;
}

vector<uint32> get_contents_uint32(const FilePath& file_name, bool binary) {
    string path = file_name.abs_string();
    FILE* f = fopen(path.c_str(), !binary ? "r" : "rb");
    if (f == nullptr)
        return {};

    fseek(f, 0, SEEK_END);
    size_t    size = ftell(f) / sizeof(uint32);
    vector<uint32> contents;
    contents.resize(size);
    rewind(f);

    fread(&contents[0], sizeof(uint32), size, f);

    fclose(f);

    return contents;
}

string& get_filepath_app_name() {
    static string app_name;
    return app_name;
}

}
