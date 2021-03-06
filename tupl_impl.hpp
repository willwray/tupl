/* WARNING: Do not edit this file "tupl_impl.hpp"
   =======
* it's generated by preprocessing "tupl_impl_pre.hpp" so is overwritten
* each time the preprocessing step is rerun, preferably automatically.
***********************************************************************/
/*
     Copyright (c) 2021 Will Wray https://keybase.io/willwray

     Distributed under the Boost Software License, Version 1.0.
           http://www.boost.org/LICENSE_1_0.txt

     Repo: https://github.com/willwray/tupl
*/
namespace ltl { // OPEN  namespace NAMESPACE_ID
template <typename T> using DEFAULT_ASSIGNABLE = std::bool_constant<
                            default_assignable<T>>;
template <typename T> using THREE_WAY_COMPARABLE = std::bool_constant<
                            three_way_comparable<T>>;
template <typename T> using EQUALITY_COMPARABLE = std::bool_constant<
                            equality_comparable<T>>;
template <typename T> using MEMBER_DEFAULT_3WAY = std::bool_constant<
                            member_default_3way<T>>;
template <typename T> using ASSIGN_OVERLOAD = std::bool_constant<
                            assignable_from<T&,T&>
                  && ! std::assignable_from<T&,T&>>;
inline constexpr std::size_t tupl_max_arity = 0xf +1;
//
// Primary template declaration, requires sizeof...(T) < TUPL_MAX_ARITY
template <typename...T> struct tupl;
//
namespace impl {
// decay_f<T> decay function type (hard fail for abominable functions)
template <typename T, typename U = std::remove_cvref_t<T>>
using decay_f = std::conditional_t<std::is_function_v<U>,U*,U>;
}
// CTAD deduce values including arrays (i.e. no decay, unlike std tuple)
template <typename...E>
tupl(E&&...) -> tupl<impl::decay_f<E>...>;
// CTAD deduce first argument as an rvalue array from braced init-list
template <typename T, int N, typename...E>
tupl(T(&&)[N], E&&...) -> tupl<T[N], impl::decay_f<E>...>;
//
//template<class T,class...A> auto to_tupl(T<A...>const&) -> TUPL_ID<A...>;
//template<class T> using to_tupl = decltype(as_tupl(std::declval<T>()));
// is_tupl trait
//template<class T> inline constexpr bool is_tupl = false;
//template<class...A> inline constexpr bool is_tupl<TUPL_ID<A...>> = true;
//
// tuplish concept, requires a 'tupl_t' member alias
template <typename T> concept tuplish = std::is_convertible_v<T,
          typename std::remove_cvref_t<T>::tupl_t>;
// tupl_size variable template (unrelated to std::tuple_size)
template <tuplish T> extern const std::size_t tupl_size;
template <typename...U> inline constexpr auto tupl_size<tupl<U...>>
         = sizeof...(U);
// size<Tupl>(): the number of tupl elements as an integral constant
template <tuplish T> constexpr auto size()
-> std::integral_constant<std::size_t,tupl_size<std::remove_cvref_t<T>>>
{ return {}; }
// size(tupl): the number of tupl elements as an integral constant
template <tuplish T> constexpr auto size(T const&)
-> std::integral_constant<std::size_t,tupl_size<std::remove_cvref_t<T>>>
{ return {}; }
// tupl_types_all meta function
template <typename, template<typename>class P>
inline constexpr bool tupl_types_all = false;
//
template <typename...T, template<typename>class P>
inline constexpr bool tupl_types_all<tupl<T...>,P> = (P<T>() && ...);
//
// tupl_val concept: tuplish type with all object-type elements
// (note: is_object matches unbounded array T[], allows for FAM)
template <typename T>
concept tupl_val = tuplish<T>
     && tupl_types_all<std::remove_cvref_t<T>, std::is_object>;
template <typename T> using is_object_lval_ref =
   std::bool_constant< std::is_lvalue_reference<T>()
                    && std::is_object<std::remove_cvref_t<T>>()>;
// tupl_tie concept: tuplish type with all reference-to-object elements
// (note: is_object matches unbounded array T[] so T(&)[] is admitted)
template <typename T>
concept tupl_tie =
        tuplish<T>
     && ! std::same_as<std::remove_cvref_t<T>, tupl<>>
     && tupl_types_all<std::remove_cvref_t<T>, is_object_lval_ref>;
// tupl_type_map<tupl<T...>, map> -> tupl<map<T>...>
template <template<typename...>class, typename>
struct tupl_type_map;
template <template<typename...>class M, typename...T>
struct tupl_type_map<M, tupl<T...>> {
    using type = tupl<M<T>...>;
};
//
namespace impl {
template <typename U>
auto assign_fwd_f() {
  using V = std::remove_cvref_t<U>;
  if constexpr (std::is_move_assignable_v<std::remove_all_extents_t<V>>)
  {
    if constexpr (std::is_trivially_copyable_v<V> && !std::is_array_v<V>
                  && sizeof(V)<=16)
        return std::type_identity<V>{};
    else
    if constexpr (std::is_lvalue_reference_v<U> || std::is_array_v<V>)
        return std::type_identity<V const&>{};
    else
        return std::type_identity<V&&>{};
  }
  else return std::type_identity<void>{};
}
}
// assign_fwd<U> 'forwarding' carrier type for assignment operator=
//  -> void       for a non-assignable type (renders it incomplete)
//          else V = remove_cvref_t<U> and:
//  -> V          for trivially_copyable non-array V of 16 bytes or less
//  -> V const&   for lvalue_ref U or array V (no move for & or array)
//  -> V&&        otherwise
//    (V&& won't accept V&, so either move or const_cast to V const&)
template <typename U>
using assign_fwd = typename decltype(impl::assign_fwd_f<U>())::type;
//
//
template <typename Tupl>
using tupl_tie_t =
         typename tupl_type_map<std::add_lvalue_reference_t,Tupl>::type;
//
template <typename Tupl>
using tupl_assign_fwd_t = typename tupl_type_map<assign_fwd,Tupl>::type;
//
// tupl<> zero-size pack specialization
template <> // tupl<T...> specializations defined in TUPL_PASS == 1
struct tupl<>
{
//  using tie_t = TUPL_ID;
  using tupl_t = tupl;
  static consteval std::size_t size() noexcept { return {}; }
  auto operator<=>(tupl const&) const = default;
  constexpr decltype(auto) operator()(auto f) const
     noexcept(noexcept(f())) { return f(); }
};
//
template <typename L, typename R>
inline constexpr bool tupls_assignable {};
template <typename...U, typename...V>
inline constexpr bool tupls_assignable<tupl<U...>,tupl<V...>> =
                           (assignable_from<U&,V> && ...);
// customization for tuple-tuple assignment
template <tuplish Lr>
struct assign_to<Lr>
{
  using L = std::remove_cvref_t<Lr>;
  using value_type = std::conditional_t<tupl_val<L>, L, const L>;
  value_type& l;
  template <tuplish R>
    requires (tupls_assignable<L, std::remove_cvref_t<R>>)
  constexpr value_type& operator=(R&& r) const noexcept(
    std::is_reference_v<R>
    ? tupl_types_all<R, std::is_nothrow_copy_assignable> // array R?
    : tupl_types_all<R, std::is_nothrow_move_assignable>)// array R?
  {
      map(l, [&r](auto&...t) {
          map(r, [&t...](auto&...u) {
            if constexpr (std::is_reference_v<R>)
              (assign(t,u), ...);
            else
              (assign(t,mv(u)), ...);
          });
      });
      return l;
  }
};
template <typename... T>
constexpr void swap(tupl<T...>& l, decltype(l) r)
  noexcept((std::is_nothrow_swappable_v<T> && ...))
    requires (std::swappable<T> && ...)
{
    map(l, [&r](T&...t) {
        map(r, [&t...](T&...u) {
            (std::ranges::swap(t, u), ...);
        });
    });
}
// tupl<T...> size N pack specializations
//#define TUPL_TIE_T tupl_tie_t<TUPL_ID>
template <typename X0>
struct tupl<X0>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;
 static consteval auto size() noexcept { return 0x0 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0))) { return f(((T&&)t).x0); }
};
template <typename X0,typename X1>
struct tupl<X0,X1>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;
 static consteval auto size() noexcept { return 0x1 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r)
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1))) { return f(((T&&)t).x0,((T&&)t).x1); }
};
template <typename X0,typename X1,typename X2>
struct tupl<X0,X1,X2>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;
 static consteval auto size() noexcept { return 0x2 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2); }
};
template <typename X0,typename X1,typename X2,typename X3>
struct tupl<X0,X1,X2,X3>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;
 static consteval auto size() noexcept { return 0x3 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4>
struct tupl<X0,X1,X2,X3,X4>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;
 static consteval auto size() noexcept { return 0x4 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5>
struct tupl<X0,X1,X2,X3,X4,X5>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;
 static consteval auto size() noexcept { return 0x5 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6>
struct tupl<X0,X1,X2,X3,X4,X5,X6>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;
 static consteval auto size() noexcept { return 0x6 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;
 static consteval auto size() noexcept { return 0x7 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;
 static consteval auto size() noexcept { return 0x8 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;
 static consteval auto size() noexcept { return 0x9 +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;
 static consteval auto size() noexcept { return 0xa +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa,typename Xb>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa,Xb>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;[[no_unique_address]] Xb xb;
 static consteval auto size() noexcept { return 0xb +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa,typename Xb,typename Xc>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa,Xb,Xc>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;[[no_unique_address]] Xb xb;[[no_unique_address]] Xc xc;
 static consteval auto size() noexcept { return 0xc +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa,typename Xb,typename Xc,typename Xd>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa,Xb,Xc,Xd>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;[[no_unique_address]] Xb xb;[[no_unique_address]] Xc xc;[[no_unique_address]] Xd xd;
 static consteval auto size() noexcept { return 0xd +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa,typename Xb,typename Xc,typename Xd,typename Xe>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa,Xb,Xc,Xd,Xe>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;[[no_unique_address]] Xb xb;[[no_unique_address]] Xc xc;[[no_unique_address]] Xd xd;[[no_unique_address]] Xe xe;
 static consteval auto size() noexcept { return 0xe +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd,((T&&)t).xe))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd,((T&&)t).xe); }
};
template <typename X0,typename X1,typename X2,typename X3,typename X4,typename X5,typename X6,typename X7,typename X8,typename X9,typename Xa,typename Xb,typename Xc,typename Xd,typename Xe,typename Xf>
struct tupl<X0,X1,X2,X3,X4,X5,X6,X7,X8,X9,Xa,Xb,Xc,Xd,Xe,Xf>
{
// using tie_t = TUPL_TIE_T const;
 using tupl_t = tupl;
 [[no_unique_address]] X0 x0;[[no_unique_address]] X1 x1;[[no_unique_address]] X2 x2;[[no_unique_address]] X3 x3;[[no_unique_address]] X4 x4;[[no_unique_address]] X5 x5;[[no_unique_address]] X6 x6;[[no_unique_address]] X7 x7;[[no_unique_address]] X8 x8;[[no_unique_address]] X9 x9;[[no_unique_address]] Xa xa;[[no_unique_address]] Xb xb;[[no_unique_address]] Xc xc;[[no_unique_address]] Xd xd;[[no_unique_address]] Xe xe;[[no_unique_address]] Xf xf;
 static consteval auto size() noexcept { return 0xf +1; }
 friend auto operator<=>(tupl const&,tupl const&)
   requires tupl_types_all<tupl,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr auto& operator=(tupl<>)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = {};}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r)
   requires (tupl_val<tupl>&&tupl_types_all<tupl,ASSIGN_OVERLOAD>)
   {return assign_to<tupl>{*this} = r;}
 template<typename...>constexpr auto& operator=(tupl_assign_fwd_t<tupl> r) const
   requires tupl_tie<tupl>
   {return assign_to<tupl>{*this} = r;}
 constexpr auto& operator=(std::same_as<tupl_tie_t<tupl>> auto const& r) const
   {return assign_to<tupl>{*this} = r;}
 template <same_ish<tupl> T> friend constexpr decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd,((T&&)t).xe,((T&&)t).xf))) { return f(((T&&)t).x0,((T&&)t).x1,((T&&)t).x2,((T&&)t).x3,((T&&)t).x4,((T&&)t).x5,((T&&)t).x6,((T&&)t).x7,((T&&)t).x8,((T&&)t).x9,((T&&)t).xa,((T&&)t).xb,((T&&)t).xc,((T&&)t).xd,((T&&)t).xe,((T&&)t).xf); }
};
template <tuplish T> requires (! tupl_types_all<T,MEMBER_DEFAULT_3WAY>
                              && tupl_types_all<T,THREE_WAY_COMPARABLE>)
constexpr auto operator<=>(T const& l,T const& r) noexcept {
 constexpr auto s = tupl_size<T>;
 constexpr compare_three_way cmp;
 if constexpr(0x0<s){if(auto c=cmp(l.x0,r.x0);c!=0||1+0x0==s)return c;}if constexpr(0x1<s){if(auto c=cmp(l.x1,r.x1);c!=0||1+0x1==s)return c;}if constexpr(0x2<s){if(auto c=cmp(l.x2,r.x2);c!=0||1+0x2==s)return c;}if constexpr(0x3<s){if(auto c=cmp(l.x3,r.x3);c!=0||1+0x3==s)return c;}if constexpr(0x4<s){if(auto c=cmp(l.x4,r.x4);c!=0||1+0x4==s)return c;}if constexpr(0x5<s){if(auto c=cmp(l.x5,r.x5);c!=0||1+0x5==s)return c;}if constexpr(0x6<s){if(auto c=cmp(l.x6,r.x6);c!=0||1+0x6==s)return c;}if constexpr(0x7<s){if(auto c=cmp(l.x7,r.x7);c!=0||1+0x7==s)return c;}if constexpr(0x8<s){if(auto c=cmp(l.x8,r.x8);c!=0||1+0x8==s)return c;}if constexpr(0x9<s){if(auto c=cmp(l.x9,r.x9);c!=0||1+0x9==s)return c;}if constexpr(0xa<s){if(auto c=cmp(l.xa,r.xa);c!=0||1+0xa==s)return c;}if constexpr(0xb<s){if(auto c=cmp(l.xb,r.xb);c!=0||1+0xb==s)return c;}if constexpr(0xc<s){if(auto c=cmp(l.xc,r.xc);c!=0||1+0xc==s)return c;}if constexpr(0xd<s){if(auto c=cmp(l.xd,r.xd);c!=0||1+0xd==s)return c;}if constexpr(0xe<s){if(auto c=cmp(l.xe,r.xe);c!=0||1+0xe==s)return c;}if constexpr(0xf<s){if(auto c=cmp(l.xf,r.xf);c!=0||1+0xf==s)return c;}
 UNREACHABLE();
}
template <tuplish T> requires (! tupl_types_all<T,MEMBER_DEFAULT_3WAY>
                              && tupl_types_all<T,THREE_WAY_COMPARABLE>)
constexpr auto operator==(T const& l,T const& r) noexcept {
    return l <=> r == 0;
}
template <tuplish T> requires (! tupl_types_all<T,MEMBER_DEFAULT_3WAY>
                            && ! tupl_types_all<T,THREE_WAY_COMPARABLE>
                              && tupl_types_all<T,EQUALITY_COMPARABLE>)
constexpr bool operator==(T const& l,T const& r) noexcept {
    constexpr auto s = tupl_size<T>;
 if constexpr(0x0<s){if(bool c=l.x0==r.x0;!c||1+0x0==s)return c;}if constexpr(0x1<s){if(bool c=l.x1==r.x1;!c||1+0x1==s)return c;}if constexpr(0x2<s){if(bool c=l.x2==r.x2;!c||1+0x2==s)return c;}if constexpr(0x3<s){if(bool c=l.x3==r.x3;!c||1+0x3==s)return c;}if constexpr(0x4<s){if(bool c=l.x4==r.x4;!c||1+0x4==s)return c;}if constexpr(0x5<s){if(bool c=l.x5==r.x5;!c||1+0x5==s)return c;}if constexpr(0x6<s){if(bool c=l.x6==r.x6;!c||1+0x6==s)return c;}if constexpr(0x7<s){if(bool c=l.x7==r.x7;!c||1+0x7==s)return c;}if constexpr(0x8<s){if(bool c=l.x8==r.x8;!c||1+0x8==s)return c;}if constexpr(0x9<s){if(bool c=l.x9==r.x9;!c||1+0x9==s)return c;}if constexpr(0xa<s){if(bool c=l.xa==r.xa;!c||1+0xa==s)return c;}if constexpr(0xb<s){if(bool c=l.xb==r.xb;!c||1+0xb==s)return c;}if constexpr(0xc<s){if(bool c=l.xc==r.xc;!c||1+0xc==s)return c;}if constexpr(0xd<s){if(bool c=l.xd==r.xd;!c||1+0xd==s)return c;}if constexpr(0xe<s){if(bool c=l.xe==r.xe;!c||1+0xe==s)return c;}if constexpr(0xf<s){if(bool c=l.xf==r.xf;!c||1+0xf==s)return c;}
 UNREACHABLE();
}
// get<I>(t)
template <int I> constexpr auto&& get(tuplish auto&& t) noexcept
  requires (I < t.size())
{
 using T = decltype(t);
 if constexpr(I==0x0)return((T)t).x0;else if constexpr(I==0x1)return((T)t).x1;else if constexpr(I==0x2)return((T)t).x2;else if constexpr(I==0x3)return((T)t).x3;else if constexpr(I==0x4)return((T)t).x4;else if constexpr(I==0x5)return((T)t).x5;else if constexpr(I==0x6)return((T)t).x6;else if constexpr(I==0x7)return((T)t).x7;else if constexpr(I==0x8)return((T)t).x8;else if constexpr(I==0x9)return((T)t).x9;else if constexpr(I==0xa)return((T)t).xa;else if constexpr(I==0xb)return((T)t).xb;else if constexpr(I==0xc)return((T)t).xc;else if constexpr(I==0xd)return((T)t).xd;else if constexpr(I==0xe)return((T)t).xe;else if constexpr(I==0xf)return((T)t).xf;
 UNREACHABLE();
}
// Index of first element of type X
template <typename X, typename...A> requires (std::same_as<X,A> || ...)
inline constexpr int indexof = []() consteval {
 using std::same_as;
 using T = tupl<A...>;
 if constexpr(same_as<X,decltype(T::x0)>)return 0x0;else if constexpr(same_as<X,decltype(T::x1)>)return 0x1;else if constexpr(same_as<X,decltype(T::x2)>)return 0x2;else if constexpr(same_as<X,decltype(T::x3)>)return 0x3;else if constexpr(same_as<X,decltype(T::x4)>)return 0x4;else if constexpr(same_as<X,decltype(T::x5)>)return 0x5;else if constexpr(same_as<X,decltype(T::x6)>)return 0x6;else if constexpr(same_as<X,decltype(T::x7)>)return 0x7;else if constexpr(same_as<X,decltype(T::x8)>)return 0x8;else if constexpr(same_as<X,decltype(T::x9)>)return 0x9;else if constexpr(same_as<X,decltype(T::xa)>)return 0xa;else if constexpr(same_as<X,decltype(T::xb)>)return 0xb;else if constexpr(same_as<X,decltype(T::xc)>)return 0xc;else if constexpr(same_as<X,decltype(T::xd)>)return 0xd;else if constexpr(same_as<X,decltype(T::xe)>)return 0xe;else if constexpr(same_as<X,decltype(T::xf)>)return 0xf;
 UNREACHABLE();
}();
//
template <typename X, typename T>
inline constexpr int tupl_indexof = not defined(tupl_indexof<X,T>);
template <typename X, typename...A>
inline constexpr int tupl_indexof<X,tupl<A...>> = indexof<X,A...>;
template <typename...T>
constexpr auto tie(T&...t) noexcept
  -> tupl<decltype(t)...> const
{
    return { t... };
};
template <int...I>
constexpr auto getie(tuplish auto&& t) noexcept
  -> tupl<decltype(get<I>((decltype(t))t))...> const
     requires ((I < t.size()) && ...)
    { return {get<I>((decltype(t))t)...}; };
template <auto...A, typename...E>
constexpr auto dupl(tupl<E...>const& a)
  -> tupl<decltype(+get<A>(a))...>
    { return {get<A>(a)...}; };
template <typename...A, typename...E>
constexpr auto dupl(tupl<E...>const& a) -> tupl<A...>
    { return {get<A>(a)...}; };
} // CLOSE namespace NAMESPACE_ID
