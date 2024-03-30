#pragma once

namespace spellbook {

// https://github.com/martinus/robin-hood-hashing/blob/7697343363af4cc3f42cab17be49e6af9ab181e2/src/test/app/checksum.h#L37
inline uint64 hash_combine(uint64 seed, uint64 value) {
    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

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
