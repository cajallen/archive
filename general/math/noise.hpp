#pragma once

#include "general/math/geometry.hpp"

namespace spellbook {

uint32 to_uint_seed(v2i i);
uint32 to_uint_seed(v3i i);
uint32 to_uint_seed(float i);
uint32 to_uint_seed(v2 i);
uint32 to_uint_seed(v3 i);
uint32 uint_noise(uint32 i, uint32 seed);
float float_noise(uint32 i, uint32 seed);
v2 v2_noise_unsigned(uint32 i, uint32 seed);
v2 v2_noise_signed(uint32 i, uint32 seed);
v2 v2_noise_signed(v2i i, uint32 seed);
v3 v3_noise_unsigned(uint32 i, uint32 seed);
v3 v3_noise_signed(uint32 i, uint32 seed);
v3 v3_noise_signed(v3i i, uint32 seed);


float perlin_noise(v2 pos, uint32 seed);
float perlin_noise(v3 pos, uint32 seed);

float fractal_perlin_noise(v3 pos, float scale, uint32 octaves, float roughness, uint32 seed);

}