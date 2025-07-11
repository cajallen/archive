#pragma once

#include <array>

#include "general/file/json.hpp"
#include "general/math/geometry.hpp"

using std::array;
using std::exchange;
using std::move;

namespace spellbook {

struct m44 {
    float data[16] = {};

    m44() = default;

    m44(float* array) {
        for (int32 i = 0; i < 16; i++)
            data[i] = array[i];
    }

    // 3x3 Constructor
    constexpr m44(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
        : data {_00, _01, _02, 0, _10, _11, _12, 0, _20, _21, _22, 0, 0, 0, 0, 1} {}

    // 4x4 Constructor
    constexpr m44(float _00, float _01, float _02, float _03, float _10, float _11, float _12, float _13, float _20, float _21, float _22, float _23, float _30,
        float _31, float _32, float _33)
        : data {_00, _01, _02, _03, _10, _11, _12, _13, _20, _21, _22, _23, _30, _31, _32, _33} {}

    static constexpr m44 identity() {
        m44 mat = {};
        for (int32 i = 0; i < 4; i++)
            mat[i * 4 + i] = 1;
        return mat;
    }

    constexpr float& rc(int32 r, int32 c) {
        return data[r * 4 + c];
    }
    constexpr float rc(int32 r, int32 c) const {
        return data[r * 4 + c];
    }

    constexpr float& cr(int32 c, int32 r) {
        return data[r * 4 + c];
    }
    constexpr float cr(int32 c, int32 r) const {
        return data[r * 4 + c];
    }

    constexpr float& operator[](int32 index) {
        return data[index];
    }

    constexpr const float& operator[](int32 index) const {
        return data[index];
    }

    m44 operator-(m44 other) const {
        m44 out;
        for (int32 i = 0; i < 16; i++)
            out[i] = data[i] - other.data[i];
        return out;
    }

    m44 operator+(m44 other) const {
        m44 out;
        for (int32 i = 0; i < 16; i++)
            out[i] = data[i] + other.data[i];
        return out;
    }
    constexpr float* ptr() {
        return data;
    }
};

inline m44 from_jv_impl(const json_value& jv, m44* _) {
    auto vec = from_jv<vector<float>>(jv);
    m44 m;
    std::copy_n(vec.begin(), 16, m.data);
    return m;
}
inline json_value to_jv(const m44& m) {
    vector<float> vec(m.data, m.data + 16);
    return to_jv(vec);
}

struct m33 {
    float data[9] = {};

    m33() = default;

    m33(float* array) {
        for (int32 i = 0; i < 9; i++)
            data[i] = array[i];
    }

    // 2x2 Constructor
    constexpr m33(float _00, float _01, float _10, float _11) : data {_00, _01, _10, _11} {}

    // 3x3 Constructor
    constexpr m33(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
        : data {_00, _01, _02, _10, _11, _12, _20, _21, _22} {}

    constexpr m33(const m44& other) : data{
            other.rc(0, 0),
            other.rc(0, 1),
            other.rc(0, 2),
            other.rc(1, 0),
            other.rc(1, 1),
            other.rc(1, 2),
            other.rc(2, 0),
            other.rc(2, 1),
            other.rc(2, 2)
        } {}

    constexpr static m33 identity() {
        m33 mat = {};
        for (int32 i = 0; i < 3; i++)
            mat[i * 3 + i] = 1;
        return mat;
    }

    constexpr float& rc(int32 r, int32 c) {
        return data[r * 3 + c];
    }
    constexpr float rc(int32 r, int32 c) const {
        return data[r * 3 + c];
    }

    constexpr float& cr(int32 c, int32 r) {
        return data[r * 3 + c];
    }

    constexpr float cr(int32 c, int32 r) const {
        return data[r * 3 + c];
    }

    constexpr float& operator[](int32 index) {
        return data[index];
    }

    constexpr const float& operator[](int32 index) const {
        return data[index];
    }

    constexpr m33 operator-(m33 other) const {
        m33 out;
        for (int32 i = 0; i < 9; i++)
            out[i] = data[i] - other.data[i];
        return out;
    }

    constexpr m33 operator+(m33 other) const {
        m33 out;
        for (int32 i = 0; i < 9; i++)
            out[i] = data[i] + other.data[i];
        return out;
    }

    constexpr operator m44() const {
        return m44(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
    }
};

struct m44GPU {
    float data[16] = {};
    explicit constexpr m44GPU(const m44& m) : data{
            m.data[0],
            m.data[4],
            m.data[8],
            m.data[12],
            m.data[1],
            m.data[5],
            m.data[9],
            m.data[13],
            m.data[2],
            m.data[6],
            m.data[10],
            m.data[14],
            m.data[3],
            m.data[7],
            m.data[11],
            m.data[15]
        } {}
    constexpr m44GPU() : data{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {}

    explicit constexpr operator m44() const {
        return m44{
            data[0],
            data[4],
            data[8],
            data[12],
            data[1],
            data[5],
            data[9],
            data[13],
            data[2],
            data[6],
            data[10],
            data[14],
            data[3],
            data[7],
            data[11],
            data[15]
        };
    }
};

struct m34 {
    float data[12];
    explicit constexpr m34(const m44& m) : data{
            m.data[0],
            m.data[1],
            m.data[2],
            m.data[3],
            m.data[4],
            m.data[5],
            m.data[6],
            m.data[7],
            m.data[8],
            m.data[9],
            m.data[10],
            m.data[11]
        } {}
};

struct m34GPU {
    float data[12];
    explicit constexpr m34GPU(const m34& m) : data{
            m.data[0],
            m.data[4],
            m.data[8],
            m.data[1],
            m.data[5],
            m.data[9],
            m.data[2],
            m.data[6],
            m.data[10],
            m.data[3],
            m.data[7],
            m.data[11]
        } {}
};

}
