// itlib-make-ptr v1.01
//
// Helper functions for making std::shared_ptr and std::unique_ptr
//
// SPDX-License-Identifier: MIT
// MIT License:
// Copyright(c) 2020-2022 Borislav Stanimirov
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files(the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and / or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions :
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
//                  VERSION HISTORY
//
//  1.01 (2022-13-12) Added make_aliased
//  1.00 (2020-10-15) Initial release
//
//
//                  DOCUMENTATION
//
// Simply include this file wherever you need.
// It defines the following functions:
//
// * itlib::make_shared(T&& arg)
//   Create a std::shared_ptr<T> by invoking std::make_shared by forwarding arg
//   to T's constructor. Thus allowing to make shared pointers which hold a
//   a copy of arg, or in case arg is an rvalue, it will me moved to the pointer.
//   This allowing to to save a retype of the name in case you want to copy or
//   move a value into a new std::shared_ptr
//
// * itlib::make_unique(T&& arg)
//   The same as itlib::make_shared but for std::unique_ptr
//   It's also written for C++11, so you don't need to enable C++14 to include
//   this header. However it's not a full substitution for std::make_unique
//
// * std::shared_ptr<T> make_aliased(const std::shared_ptr<U>& owner, T* ptr)
//   *SAFELY* create an aliased shared pointer.
//   If the use count of owner is zero, it will return null even if ptr is
//   not null.
//   https://ibob.bg/blog/2022/12/28/dont-use-shared_ptr-aliasing-ctor/
//
// Example:
//
// my<complex, template, type> val;
//
// // not nice
// auto ptr = std::make_shared<my<complex, template, type>>(std::move(val));
//
// // nice
// auto p1 = itlib::make_shared(val); // copy val into p1
// auto p2 = itlib::make_shared(std::move(val)); // move val into p2
//
//
//                  TESTS
//
// You can find unit tests in the official repo:
// https://github.com/iboB/itlib/blob/master/test/
//
#pragma once

#include <memory>
#include <type_traits>

namespace astl
{

template <typename T>
auto make_shared(T&& t) -> std::shared_ptr<typename std::remove_reference<T>::type>
{
    return std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(t));
}

template <typename T>
auto make_unique(T&& t) -> std::unique_ptr<typename std::remove_reference<T>::type>
{
    using RRT = typename std::remove_reference<T>::type;
    return  std::unique_ptr<RRT>(new RRT(std::forward<T>(t)));
}

template <typename U, typename T>
std::shared_ptr<T> make_aliased(const std::shared_ptr<U>& owner, T* ptr) {
    if (owner.use_count() == 0) return {};
    return std::shared_ptr<T>(owner, ptr);
}

}
