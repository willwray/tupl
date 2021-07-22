//  Copyright (c) 2021 Will Wray https://keybase.io/willwray
//
//  Distributed under the Boost Software License, Version 1.0.
//        http://www.boost.org/LICENSE_1_0.txt
//
//  Repo: https://github.com/willwray/tupl

#ifndef TUPL_HPP
#define TUPL_HPP
/*
  ltl::tupl
 -=========- 
  
  A C++20 tuple type implemented as aggregate struct specializations.
  Configured by default for up to 16 members of any allowed type.
  
   Depends only on <compare> for three-way operator <=> comparisons.

  The macro implementation in "tupl_impl_pre.hpp" should be preprocessed
  on installation to generate "tupl_impl.hpp" header. It's best to auto-
  mate this preprocessing step. The gen header is used as fallback if
  the preprocessed .

  You can manually preprocess with a command line like:

    > c++ -nostdinc -C -E -P -o tupl_impl.hpp tupl_impl_pre.hpp

    > cl /Zc:preprocessor /EP 

  Requires __VA_OPT__  preprocessor support (MSVC cl /Zc:preprocessor)
  lack of __VA_OPT__ is auto-detected via a static_assert error message.

  Namespace id 'ltl' and class template id 'tupl' are configurable via
  TUPL_NAMESPACE_ID and TUPL_ID defines. The arity is also configurable.

  E.g. to generate ns::tuple add -DTUPL_NAMESPACE_ID=ns -DTUPL_ID=tuple,
  or to generate with no namespace -DTUPL_NAMESPACE_ID=
*/

#include <compare>

#include "array_compare.hpp"
#include "array_assign.hpp"

#if __has_include("tupl_impl.hpp")
#include "tupl_impl.hpp" // Do not edit generated file "tupl_impl.hpp"
#else
#include "tupl_impl_pre.hpp" // If you edit "tupl_impl_pre.hpp" then
                             // remember to regenerate "tupl_impl.hpp"
#endif

#endif
