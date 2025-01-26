// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <optional>
#include <bitset>
#include <concepts>

namespace ac::schema {

namespace impl {
template <typename T>
struct DefaultT { T value; };
template <>
struct DefaultT<void> {};
} // namespace impl

template <typename T>
impl::DefaultT<T> Default(T&& t) {
    return {std::forward<T>(t)};
}

inline impl::DefaultT<void> Default() {
    return {};
}

template <typename T>
class Field {
public:
    using Type = T;

    Field() {
        m_flags.set(Flag_Required);
    }

    Field(std::nullopt_t) {}

    template <std::convertible_to<T> U>
    Field(impl::DefaultT<U>&& def)
        : m_value(std::move(def.value))
    {
        m_flags.set(Flag_DefaultSet);
    }

    Field(impl::DefaultT<void>)
        : m_value(std::in_place)
    {
        m_flags.set(Flag_DefaultSet);
    }

    template <std::convertible_to<T> U>
    Field(U&& val) : m_value(std::forward<U>(val)) {}

    Field(const Field&) = default;
    Field& operator=(const Field&) = default;
    Field(Field&&) noexcept = default;
    Field& operator=(Field&&) noexcept = default;

    Field& operator=(T&& def) {
        m_value = std::forward<T>(def);
        m_flags.reset(Flag_DefaultSet);
        return *this;
    }

    bool hasValue() const noexcept {
        return m_value.has_value();
    }

    bool required() const noexcept {
        return m_flags.test(Flag_Required);
    }

    bool defaultSet() const noexcept {
        return m_flags.test(Flag_DefaultSet);
    }

    T valueOr(T&& def) const& {
        if (m_value.has_value()) {
            return *m_value;
        }
        return std::forward<T>(def);
    }

    T& materialize() {
        if (!m_value.has_value()) {
            m_value.emplace();
        }
        return *m_value;
    }

    T* operator->() noexcept {
        return &value();
    }
    const T* operator->() const noexcept {
        return &value();
    }

    // yes, all implicit operators are intentional

    T& value() noexcept { return *m_value; }
    const T& value() const noexcept { return *m_value; }

    operator T& ()& noexcept { return value(); }
    operator const T& () const& noexcept { return value(); }
    operator T()&& noexcept { return std::move(value()); }

    std::optional<T>& opt() noexcept { return m_value; }
    const std::optional<T>& opt() const noexcept { return m_value; }

    operator std::optional<T>& ()& noexcept { return opt(); }
    operator const std::optional<T>& () const& noexcept { return opt(); }
    operator std::optional<T>()&& noexcept { return std::move(opt()); }

    template <typename U>
    friend auto operator<=>(const Field& f, const U& t) noexcept {
        return f.m_value <=> t;
    }

    template <typename U>
    friend auto operator==(const Field& f, const U& t) noexcept {
        return f.m_value == t;
    }

    template <typename U>
    friend auto operator<=>(const U& t, const Field& f) noexcept {
        return t <=> f.m_value;
    }

private:
    // ideally, instead of using optional + flags, we would reimplement optional with flags
    // however that's too much work for now so we will have some bits wasted and an additional byte as member
    std::optional<T> m_value;

    enum Flags {
        Flag_Required,
        Flag_DefaultSet
    };
    std::bitset<2> m_flags;
};

} // namespace ac::schema
