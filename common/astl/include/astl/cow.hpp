// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// copy on write helpers

namespace astl {

// single producer cow
template <typename T>
class sp_cow {
    std::shared_ptr<T> m_data;
public:
    sp_cow()
        : m_data(std::make_shared<T>())
    {}

    template <typename... Args>
    sp_cow(Args&&... args)
        : m_data(std::make_shared<T>(std::forward<Args>(args)...))
    {}

    sp_cow(const sp_cow& other) = default;
    sp_cow& operator=(const sp_cow& other) = default;
    sp_cow(sp_cow&& other) noexcept = default;
    sp_cow& operator=(sp_cow&& other) noexcept = default;

    const T& operator*() const { return *m_data; }
    const T* operator->() const { return m_data.get(); }

    // explicit write
    T& w() {
        if (m_data.use_count() > 1) {
            m_data = std::make_shared<T>(*m_data);
        }
        return *m_data;
    }
};

} // namespace astl
