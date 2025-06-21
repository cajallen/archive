#pragma once

#include <cassert>

namespace spellbook {

template<typename Signature>
class function;

template<typename Ret, typename... Args>
class function<Ret(Args...)> {
public:
    function() noexcept : _callable_ptr(nullptr), _invoker(nullptr), _manager(nullptr) {}

    function(std::nullptr_t) noexcept : function() {}

    function(const function& other) {
        if (other._manager) {
            _manager = other._manager;
            _invoker = other._invoker;
            _callable_ptr = _manager(MANAGER_OP::CLONE, other._callable_ptr, nullptr);
        } else {
            _callable_ptr = nullptr;
            _invoker = nullptr;
            _manager = nullptr;
        }
    }

    function(function&& other) noexcept {
        _callable_ptr = other._callable_ptr;
        _invoker = other._invoker;
        _manager = other._manager;

        other._callable_ptr = nullptr;
        other._invoker = nullptr;
        other._manager = nullptr;
    }

    template<typename Callable, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Callable>, function>>>
    function(Callable&& callable) {
        using Decayed = std::decay_t<Callable>;

        static_assert(std::is_invocable_r_v<Ret, Decayed, Args...>,
                      "Callable does not match function signature");

        _callable_ptr = new Decayed(std::forward<Callable>(callable));

        if constexpr (std::is_void_v<Ret>) {
            _invoker = [](void* ptr, Args&&... args) {
                Decayed* f = reinterpret_cast<Decayed*>(ptr);
                (*f)(std::forward<Args>(args)...);
            };
        } else {
            _invoker = [](void* ptr, Args&&... args) -> Ret {
                Decayed* f = reinterpret_cast<Decayed*>(ptr);
                return (*f)(std::forward<Args>(args)...);
            };
        }

        _manager = [](MANAGER_OP op, void* src_ptr, void* dst_ptr) -> void* {
            switch (op) {
                case MANAGER_OP::DESTROY:
                    delete reinterpret_cast<Decayed*>(src_ptr);
                    return nullptr;
                case MANAGER_OP::CLONE:
                    return new Decayed(*reinterpret_cast<Decayed*>(src_ptr));
                case MANAGER_OP::MOVE:
                    *reinterpret_cast<Decayed**>(dst_ptr) = reinterpret_cast<Decayed*>(src_ptr);
                    return nullptr;
                default:
                    return nullptr;
            }
        };
    }

    ~function() {
        if (_manager && _callable_ptr) {
            _manager(MANAGER_OP::DESTROY, _callable_ptr, nullptr);
        }
    }

    function& operator=(const function& other) {
        if (this != &other) {
            this->~function();
            if (other._manager) {
                _manager = other._manager;
                _invoker = other._invoker;
                _callable_ptr = _manager(MANAGER_OP::CLONE, other._callable_ptr, nullptr);
            } else {
                _callable_ptr = nullptr;
                _invoker = nullptr;
                _manager = nullptr;
            }
        }
        return *this;
    }

    function& operator=(function&& other) noexcept {
        if (this != &other) {
            this->~function();

            _callable_ptr = other._callable_ptr;
            _invoker = other._invoker;
            _manager = other._manager;

            other._callable_ptr = nullptr;
            other._invoker = nullptr;
            other._manager = nullptr;
        }
        return *this;
    }

    function& operator=(std::nullptr_t) noexcept {
        this->~function();
        _callable_ptr = nullptr;
        _invoker = nullptr;
        _manager = nullptr;
        return *this;
    }

    Ret operator()(Args... args) const {
        assert(_invoker && "Attempting to invoke an empty function");

        if constexpr (std::is_void_v<Ret>) {
            _invoker(_callable_ptr, std::forward<Args>(args)...);
        } else {
            return _invoker(_callable_ptr, std::forward<Args>(args)...);
        }
    }

    explicit operator bool() const noexcept {
        return _invoker != nullptr;
    }

private:
    enum class MANAGER_OP { DESTROY, CLONE, MOVE };

    using Invoker = Ret (*)(void*, Args&&...);
    using Manager = void* (*)(MANAGER_OP, void*, void*);

    void* _callable_ptr;
    Invoker _invoker;
    Manager _manager;
};

}