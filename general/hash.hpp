#pragma once

namespace spellbook {

template <typename T>
constexpr uint64 hash_data(const T* data, uint32 len, uint64 seed = 14695981039346656037ull) {
    auto input = (const uint8*) data;
    while (len-- > 0)
        seed = 1099511628211ull * (seed ^ *input++);

    return seed;
}

constexpr uint64 hash_view(std::string_view data, uint64 seed = 14695981039346656037ull) {
    const char* input = data.data();
    uint32 len = data.size();
    while (len-- > 0)
        seed = 1099511628211ull * (seed ^ *input++);

    return seed;
}

}
