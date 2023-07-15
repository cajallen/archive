#pragma once

#include <queue>

#include "general/string.hpp"
#include "general/color.hpp"

namespace spellbook {

struct BasicMessage {
    string str;
    string group      = "None";
    Color  color      = palette::white;
    vector<string> frame_tags = {};
};

extern std::queue<BasicMessage> message_queue;

using namespace std::string_literals;

inline void log_warning(const std::string& msg) {
    message_queue.emplace("WARNING: "s + msg, "warning", palette::orange);
}

inline void log_error(const std::string& msg) {
    message_queue.emplace("ERROR: "s + msg, "assert", palette::crimson);
    __debugbreak();
}

inline void log_warning(const std::string& msg, const std::string& group) {
    message_queue.emplace("WARNING: "s + msg, group, palette::orange);
}

inline void log_error(const std::string& msg, const std::string& group) {
    message_queue.emplace("ERROR: "s + msg, group, palette::crimson);
    __debugbreak();
}

inline void log(const BasicMessage& msg) { message_queue.push(msg); }
inline void log(BasicMessage&& msg) { message_queue.push(msg); }


#define assert_else(cond)              \
if (!(cond)) {                         \
    spellbook::log_error("ASSERT_FAIL: !(" #cond ")"); \
} if (!(cond))

#define check_else(cond)                  \
if (!(cond)) {                            \
    spellbook::log_warning("CHECK_FAIL: !(" #cond ")"); \
} if (!(cond))



}