#include "noise.hpp"

#include "general/math/math.hpp"

namespace spellbook {

uint32 to_uint_seed(v2i i) {
    return i.x + i.y * 49213191;
}

uint32 to_uint_seed(v3i i) {
    return i.x + i.y * 496211 + i.z * 49213191;
}

uint32 to_uint_seed(float i) {
    return int(i * 4096.f);
}

uint32 to_uint_seed(v2 i) {
    return to_uint_seed(v2i(i * 4096.0f));
}

uint32 to_uint_seed(v3 i) {
    return to_uint_seed(v3i(i * 4096.0f));
}

uint32 uint_noise(uint32 i, uint32 seed) {
    uint32 mangled = i;
    mangled *= 0xB5297A4Du;
    mangled += seed;
    mangled ^= (mangled >> 8);
    mangled += 0x68E31DA4u;
    mangled ^= (mangled << 8);
    mangled *= 0x1B56C4E9u;
    mangled ^= (mangled >> 8);
    return mangled;
}

float float_noise(uint32 i, uint32 seed) {
    return float(uint_noise(i, seed) & 0xffffffu) / 16777215.0f;
}

v2 v2_noise_unsigned(uint32 i, uint32 seed) {
    return v2(float_noise(i + 1, seed), float_noise(i + 2, seed));
}

v2 v2_noise_signed(uint32 i, uint32 seed) {
    return -1.0f + v2_noise_unsigned(i, seed) * 2.0f;
}

v2 v2_noise_signed(v2i i, uint32 seed) {
    return v2_noise_signed(to_uint_seed(i), seed);
}

v3 v3_noise_unsigned(uint32 i, uint32 seed) {
    return v3(float_noise(i + 1, seed), float_noise(i + 2, seed), float_noise(i + 3, seed));
}

v3 v3_noise_signed(uint32 i, uint32 seed) {
    return v3(-1.0f) + v3_noise_unsigned(i, seed) * 2.0f;
}

v3 v3_noise_signed(v3i i, uint32 seed) {
    return v3_noise_signed(to_uint_seed(i), seed);
}

float perlin_noise(v2 pos, uint32 seed) {
    v2i cell     = v2i(math::floor(pos));
    v2  cell_pos = math::fract(pos);

    float val = math::mix(math::mix(math::dot(v2_noise_signed(cell + v2i(0, 0), seed), cell_pos - v2(0.0, 0.0)),
                        math::dot(v2_noise_signed(cell + v2i(1, 0), seed), cell_pos - v2(1.0, 0.0)),
                        math::smoothstep(0.0, 1.0, cell_pos.x)),
        math::mix(math::dot(v2_noise_signed(cell + v2i(0, 1), seed), cell_pos - v2(0.0, 1.0)),
            math::dot(v2_noise_signed(cell + v2i(1, 1), seed), cell_pos - v2(1.0, 1.0)),
            math::smoothstep(0.0, 1.0, cell_pos.x)),
        math::smoothstep(0.0, 1.0, cell_pos.y));
    return val * 0.5 + 0.5;
}

float perlin_noise(v3 pos, uint32 seed) {
    v3i cell     = v3i(math::floor(pos));
    v3  cell_pos = math::fract(pos);

    float val = math::mix(math::mix(math::mix(math::dot(v3_noise_signed(cell + v3i(0, 0, 0), seed), cell_pos - v3(0.0, 0.0, 0.0)),
                            math::dot(v3_noise_signed(cell + v3i(1, 0, 0), seed), cell_pos - v3(1.0, 0.0, 0.0)),
                            math::smoothstep(0.0, 1.0, cell_pos.x)),
                        math::mix(math::dot(v3_noise_signed(cell + v3i(0, 1, 0), seed), cell_pos - v3(0.0, 1.0, 0.0)),
                            math::dot(v3_noise_signed(cell + v3i(1, 1, 0), seed), cell_pos - v3(1.0, 1.0, 0.0)),
                            math::smoothstep(0.0, 1.0, cell_pos.x)),
                        math::smoothstep(0.0, 1.0, cell_pos.y)),
        math::mix(math::mix(math::dot(v3_noise_signed(cell + v3i(0, 0, 1), seed), cell_pos - v3(0.0, 0.0, 1.0)),
                math::dot(v3_noise_signed(cell + v3i(1, 0, 1), seed), cell_pos - v3(1.0, 0.0, 1.0)),
                math::smoothstep(0.0, 1.0, cell_pos.x)),
            math::mix(math::dot(v3_noise_signed(cell + v3i(0, 1, 1), seed), cell_pos - v3(0.0, 1.0, 1.0)),
                math::dot(v3_noise_signed(cell + v3i(1, 1, 1), seed), cell_pos - v3(1.0, 1.0, 1.0)),
                math::smoothstep(0.0, 1.0, cell_pos.x)),
            math::smoothstep(0.0, 1.0, cell_pos.y)),
        math::smoothstep(0.0, 1.0, cell_pos.z));
    return val * 0.5 + 0.5;
}

float fractal_perlin_noise(v3 pos, float scale, uint32 octaves, float roughness, uint32 seed) {
    float value = 0.0f;
    float total_max = 0.0f;

    float weight = 1.0f;
    for (uint32 i = 0; i < octaves; i++) {
        total_max += weight;

        value += perlin_noise(pos * v3(scale), seed);

        scale *= 2.0f;

        seed = uint_noise(0, seed);
        weight *= roughness;
    }

    return value / total_max;
}

}