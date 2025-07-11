#pragma once

#include "general/umap.hpp"
#include "general/logger.hpp"
#include "general/file/json.hpp"

namespace spellbook {

template <typename T>
struct Archive;

template <typename T>
struct id_ptr {
    uint64 id;

    static Archive<T>& archive();
    static id_ptr<T>   null();
    static id_ptr<T>   ptr(T& t);

    id_ptr();
    id_ptr(uint64 value);
    id_ptr(const T& t, uint64 value);

    template <typename... Args> static id_ptr<T> emplace(Args&&...args);

    bool valid() const;
    void remove() const;

    T& operator*() const;
    T* operator->() const;

    bool operator==(id_ptr<T> other) const;
    bool operator!=(id_ptr<T> other) const;
};

template <typename T>
id_ptr<T> from_jv_impl(const json_value& jv, id_ptr<T>* _) {
    json      j = from_jv<json>(jv);
    if (j.contains("node"))
        return id_ptr<T>(from_jv<T>(*j["node"]), from_jv<uint64>(*j["id"]));
    else
        return id_ptr<T>(from_jv<uint64>(*j["id"]));
}

template <typename T>
json_value to_jv(const id_ptr<T>& value) {
    auto j = json();
    j["id"]   = make_shared<json_value>(to_jv(value.id)); 
    return to_jv(j);
}

template <typename T>
json_value to_jv_full(const id_ptr<T>& value) {
    auto j = json();
    j["node"] = make_shared<json_value>(to_jv(*value));
    j["id"]   = make_shared<json_value>(to_jv(value.id)); 
    return to_jv(j);
}

template <typename T>
struct Archive {
    umap<uint64, T>  vals;
    umap<T*, uint64> ptrs;
};

template <typename T>
Archive<T>& id_ptr<T>::archive() {
    static Archive<T> arch;
    return arch;
}

template <typename T>
id_ptr<T> id_ptr<T>::null() {
    return id_ptr<T>(0);
}

template <typename T>
id_ptr<T> id_ptr<T>::ptr(T& t) {
    auto& arch = archive();
    return id_ptr<T>(arch.ptrs[&t]);
}

template <typename T>
id_ptr<T>::id_ptr(uint64 value)
    : id(value) {
}

template <typename T>
id_ptr<T>::id_ptr(const T& t, uint64 value)
    : id(value) {
    auto& arch                   = archive();
    arch.vals[value]             = t;
    arch.ptrs[&arch.vals[value]] = value;
}

template <typename T>
id_ptr<T>::id_ptr() : id(0) {
}

template <typename T>
template <typename... Args>
id_ptr<T> id_ptr<T>::emplace(Args&&...args) {
    auto&     arch = archive();
    id_ptr<T> new_id;
    new_id.id                        = math::random_uint64();
    arch.vals[new_id.id]             = T(std::forward<Args>(args)...);
    arch.ptrs[&arch.vals[new_id.id]] = new_id.id;
    return new_id;
}

template <typename T>
void id_ptr<T>::remove() const {
    if (!this->valid())
        return;
    auto& arch = archive();
    arch.ptrs.erase(&**this);
    arch.vals.erase(id);
}

template <typename T>
bool id_ptr<T>::valid() const {
    auto& arch = archive();
    if (*this == null())
        return false;
    return arch.vals.contains(id);
}

template <typename T>
T& id_ptr<T>::operator*() const {
    assert_else(this->valid());
    auto& arch = archive();
    return arch.vals[id];
}

template <typename T>
T* id_ptr<T>::operator->() const {
    assert_else(this->valid());
    auto& arch = archive();
    return &arch.vals[id];
}

template <typename T>
bool id_ptr<T>::operator==(id_ptr<T> other) const {
    return id == other.id;
}

template <typename T>
bool id_ptr<T>::operator!=(id_ptr<T> other) const {
    return id != other.id;
}

}
