//  Copyright (c) 2021 Will Wray https://keybase.io/willwray
//
//  Distributed under the Boost Software License, Version 1.0.
//        http://www.boost.org/LICENSE_1_0.txt
//
//  Repo: https://github.com/willwray/tupl

#ifndef TUPL_SWAP_HPP
#define TUPL_SWAP_HPP

#include <utility>

#include "tupl.hpp"

#include "namespace.hpp" // Optional namespace, defaults to ltl

template <typename... T>
void swap(tupl<T...>& l, decltype(l) r)
noexcept((std::is_nothrow_swappable_v<T> && ...))
requires (std::is_swappable_v<T> && ...)
{
    using std::swap;
    map(l, [&r](T&...t) {
        map(r, [&t...](T&...u) {
            (swap(t, u), ...);
        });
    });
}

#include "namespace.hpp"

#endif