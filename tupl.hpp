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
  mate this preprocessing step. The 'pre' header is used as fallback if
  the tupl_impl.hpp header is not on the include path.

  You can manually preprocess tupl_impl_pre.hpp -> tupl_impl.hpp
  with a command line like:

   > cc -I./subprojects/IREPEAT -MMD -C -E -P -o tupl_impl.hpp
                                                 tupl_impl_pre.hpp

   > cl /I:../subprojects/IREPEAT /Zc:preprocessor /C /EP 

  Requires __VA_OPT__  preprocessor support (MSVC cl /Zc:preprocessor)
  lack of __VA_OPT__ is auto-detected via a static_assert error message.
  (other useful cpp flags are -nostdinc -Wno-c++20-compat)

  Namespace id 'ltl' and class template id 'tupl' are configurable via
  NAMESPACE_ID and TUPL_ID defines. The arity is also configurable.

  E.g. to generate ns::tuple add -DNAMESPACE_ID=ns -DTUPL_ID=tuple,
  or to generate with no namespace -DNAMESPACE_ID=
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
