// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

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

    sp_cow(const sp_cow& other) = delete;
    sp_cow& operator=(const sp_cow& other) = delete;
    sp_cow(sp_cow&& other) noexcept = default;
    sp_cow& operator=(sp_cow&& other) noexcept = default;

    const T& operator*() const { return *m_data; }
    const T* operator->() const { return m_data.get(); }

    std::shared_ptr<const T> detach() const noexcept { return m_data; }

    // explicit write
    T& w() {
        // this is the single-producer-critical part
        // when the producer is single it can safely check the use count as it can be sure that no other thread
        // will concurrently increase it to 2
        if (m_data.use_count() > 1) {
            m_data = std::make_shared<T>(*m_data);
        }
        return *m_data;
    }
};

} // namespace astl
