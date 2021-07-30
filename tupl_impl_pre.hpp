#ifndef TUPL_PASS    // Three conditional sections, empty,1,2

#ifndef TUPL_ID      // TUPL_ID = your name for the tuple template id
#define TUPL_ID tupl // E.g. configure with -DTUPL_ID=tuple
#endif

#ifndef TUPL_MAX_INDEX // TUPL_MAX_INDEX = tupl max 'arity' minus one
#define TUPL_MAX_INDEX (f) // specified as parenthesized hex digits
#endif

#ifndef TUPL_NUA // TUPL_NUA : define empty to disable no_unique_address
#define TUPL_NUA [[no_unique_address]]
#endif

#if __has_include("tupl_impl_noedit_warn.hpp") // Warning banner for the
#include "tupl_impl_noedit_warn.hpp" // generated file; Don't edit this!
#endif

#include "IREPEAT.hpp"

#define TUPL_MAX_ARITY CAT(TUPL_ID,_max_arity) // tupl_max_arity value
#define TUPL_CONSTRUCT CAT(TUPL_ID,_construct) // tupl_construct type
#define TUPL_MAKE      CAT(TUPL_ID,_make)      // tupl_make function

#include "namespace.hpp" // Optional namespace, defaults to ltl

template <typename T, typename U>
concept same_ish = std::same_as<U, std::remove_cvref_t<T>>;

template <typename T> using DEFAULT_ASSIGNABLE = std::bool_constant<
                            default_assignable<T>>;
template <typename T> using THREE_WAY_COMPARABLE = std::bool_constant<
                            three_way_comparable<T>>;
template <typename T> using EQUALITY_COMPARABLE  = std::bool_constant<
                            equality_comparable<T>>;
template <typename T> using MEMBER_DEFAULT_3WAY  = std::bool_constant<
                            member_default_3way<T>>;

inline constexpr std::size_t TUPL_MAX_ARITY = HEXLIT(TUPL_MAX_INDEX)+1;

//
// Primary template declaration, requires sizeof...(T) < TUPL_MAX_ARITY
template <typename...T> struct TUPL_ID;
//

// CTAD deduce values including arrays (i.e. no decay, unlike std tuple)
template <typename...E>
TUPL_ID(E&&...) -> TUPL_ID<std::remove_cvref_t<E>...>;
//

// size(tupl): the number of tupl elements as an integral constant
template <typename...A> constexpr auto size(TUPL_ID<A...> const&) ->
  std::integral_constant<std::size_t,sizeof...(A)> { return {}; }

// is_tupl trait
template<class T>   inline constexpr bool is_tupl = false;
template<class...A> inline constexpr bool is_tupl<TUPL_ID<A...>> = true;
//
// tuplish concept
template <typename T> concept tuplish = is_tupl<std::remove_cvref_t<T>>;

// tupl_types_all meta function
template <typename, template<typename>class P>
inline constexpr bool tupl_types_all = false;
//
template <typename...T, template<typename>class P>
inline constexpr bool tupl_types_all<TUPL_ID<T...>,P> = (P<T>() && ...);
//

// tupl_val concept: tuplish type with all object-type elements
template <typename T>
concept tupl_val = tuplish<T>
     && tupl_types_all<std::remove_cvref_t<T>,std::is_object>;

// tupl_type_map<tupl<T...>, map>
//            -> tupl<typename map<T>::type...>
template <template<typename...>class, typename>
struct tupl_type_map;

template <template<typename...>class M, typename...T>
struct tupl_type_map<M, TUPL_ID<T...>> {
    using type = TUPL_ID<M<T>...>;
};
//

// assign_fwd<U>  'forwarding' carrier type for assignment operator=
//  -> V           for trivially_copyable V of 16 bytes or less
//  -> V const&    for lvalue_ref U or array V (no move for & or array)
//  -> V&&         otherwise (won't accept V& so user must choose)
template <typename U, typename V = std::remove_cvref_t<U>>
  requires std::is_move_assignable_v<std::remove_all_extents_t<V>>
using assign_fwd = std::conditional_t<
  std::is_trivially_copyable_v<V> && sizeof(V)<=16, V,
                 std::conditional_t<
  std::is_lvalue_reference_v<U> || std::is_array_v<V>, V const&, V&&>>;
//

//
template <typename Tupl>
using tupl_assign_fwd_t = typename tupl_type_map<assign_fwd,Tupl>::type;
//

// tupl<> zero-size pack specialization
template <> // tupl<T...> specializations defined in TUPL_PASS == 1
struct TUPL_ID<> {
  auto operator<=>(TUPL_ID const&) const = default;
  constexpr decltype(auto) operator()(auto f) const
     noexcept(noexcept(f())) { return f(); }
};
//

// customization for tuple-tuple assignment
template <typename... T>
struct assign_to<TUPL_ID<T...>>
{
  using value_type = std::conditional_t<
      tupl_val<TUPL_ID<T...>>,
      TUPL_ID<T...>, const TUPL_ID<T...>>;
  
  value_type& l;

  template <typename... U>
  constexpr value_type& operator=(TUPL_ID<U...>& r)
     noexcept((std::is_nothrow_copy_assignable_v<T> && ...))
      requires (assignable_from<T&,U> && ...)
  {
    map(l, [&r](T&...t) {
      map(r, [&t...](U&...u) {
        (assign(t,u), ...);
      });
    });
    return l;
  }
};

// tupl<T...> size N pack specializations

#define TUPL_TYPE_ID XD
#define TUPL_DATA_ID xD
#define TYPENAME_DECL(n) typename TUPL_TYPE_ID(n)
#define TUPL_t_DATA_FWD(n) ((T&&)t).TUPL_DATA_ID(n)
#define MEMBER_DECL(n) TUPL_NUA TUPL_TYPE_ID(n) TUPL_DATA_ID(n);

#define TUPL_TYPE_IDS IREPEAT(VREPEAT_INDEX,TUPL_TYPE_ID,COMMA)
#define TUPL_DATA_IDS IREPEAT(VREPEAT_INDEX,TUPL_DATA_ID,COMMA)
#define TYPENAME_DECLS IREPEAT(VREPEAT_INDEX,TYPENAME_DECL,COMMA)
#define TUPL_t_DATA_FWDS IREPEAT(VREPEAT_INDEX,TUPL_t_DATA_FWD,COMMA)
#define MEMBER_DECLS IREPEAT(VREPEAT_INDEX,MEMBER_DECL,NOSEP)

#define MAP_V(...) template <same_ish<TUPL_ID> T> friend constexpr \
decltype(auto) map(T&& t, auto f) noexcept(noexcept(f(__VA_ARGS__)))\
 { return f(__VA_ARGS__); }

#define R_TUPL tupl_assign_fwd_t<TUPL_ID>

#define TUPL_PASS 1
#define VREPEAT_COUNT TUPL_MAX_INDEX
#define VREPEAT_MACRO ../../tupl_impl_pre.hpp
#include "VREPEAT.hpp"

#undef TUPL_PASS
#define TUPL_PASS 2
#include __FILE__

#elif (TUPL_PASS == 1)

template <TYPENAME_DECLS>
struct TUPL_ID<TUPL_TYPE_IDS> {
 MEMBER_DECLS
 friend auto operator<=>(TUPL_ID const&,TUPL_ID const&)
   requires tupl_types_all<TUPL_ID,MEMBER_DEFAULT_3WAY> = default;
 template<typename...>constexpr TUPL_ID& operator=(R_TUPL r)
   requires (tupl_val<TUPL_ID>) {return assign_to<TUPL_ID>{*this} = r;}
 template<typename...>constexpr TUPL_ID const& operator=(R_TUPL r) const
   requires (!tupl_val<TUPL_ID>) {return assign_to<TUPL_ID>{*this} = r;}
 MAP_V(TUPL_t_DATA_FWDS)
};

#else

#ifdef __GNUC__
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(false)
#else
#define UNREACHABLE()
#endif

template <tuplish T> requires (! tupl_types_all<T,MEMBER_DEFAULT_3WAY>
                              && tupl_types_all<T,THREE_WAY_COMPARABLE>)
constexpr auto operator<=>(T const& l,T const& r) noexcept {
 constexpr auto s = decltype(size(l))();
 constexpr compare_three_way cmp;
#define MACRO(N) if constexpr(HEXLIT(N)<s){if(auto c=cmp(\
l.TUPL_DATA_ID(N),r.TUPL_DATA_ID(N));c!=0||1+HEXLIT(N)==s)return c;}
 IREPEAT(TUPL_MAX_INDEX,MACRO,NOSEP)
#undef MACRO
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
    constexpr auto s = decltype(size(l))();
#define MACRO(N) if constexpr(HEXLIT(N)<s){if(bool c=\
l.TUPL_DATA_ID(N)==r.TUPL_DATA_ID(N);!c||1+HEXLIT(N)==s)return c;}
 IREPEAT(TUPL_MAX_INDEX,MACRO,NOSEP)
#undef MACRO
 UNREACHABLE();
}

// get<I>(t)
template <int I> constexpr auto&& get(tuplish auto&& t) noexcept
  requires (I < decltype(size(t))())
{
 using T = decltype(t);
#define ELSE() else
#define MACRO(N) if constexpr(I==HEXLIT(N))return((T)t).TUPL_DATA_ID(N);
 IREPEAT(TUPL_MAX_INDEX,MACRO,ELSE)
#undef MACRO
 UNREACHABLE();
}

// Index of first element of type X
template <typename X, typename...A> requires (std::same_as<X,A> || ...)
inline constexpr int indexof = []() consteval {
 using std::same_as;
 using T = TUPL_ID<A...>;
#define TUPL_TYPE(N) decltype(T::TUPL_DATA_ID(N))
#define MACRO(N) if constexpr(same_as<X,TUPL_TYPE(N)>)return HEXLIT(N);
 IREPEAT(TUPL_MAX_INDEX,MACRO,ELSE)
#undef MACRO
#undef TUPL_TYPE
#undef ELSE
 UNREACHABLE();
}();
//
template <typename X, typename T>
inline constexpr int tupl_indexof = not defined(tupl_indexof<X,T>);
template <typename X, typename...A>
inline constexpr int tupl_indexof<X,TUPL_ID<A...>> = indexof<X,A...>;

#undef UNREACHABLE

template <auto...A, typename...E>
constexpr auto dupl(TUPL_ID<E...>const& a)
  -> TUPL_ID<decltype(+get<A>(a))...>
    { return {get<A>(a)...}; };

template <typename...A, typename...E>
constexpr auto dupl(TUPL_ID<E...>const& a) -> TUPL_ID<A...>
    { return {get<A>(a)...}; };

#include "IREPEAT_UNDEF.hpp"

#undef R_TUPL
#undef MAP_V

#undef MEMBER_DECLS
#undef TUPL_DATA_FWDS
#undef TYPENAME_DECLS
#undef TUPL_DATA_IDS
#undef TUPL_TYPE_IDS

#undef MEMBER_DECL
#undef TUPL_DATA_FWD
#undef TYPENAME_DECL
#undef TUPL_DATA_ID
#undef TUPL_TYPE_ID

#undef TUPL_MAX_INDEX
#undef TUPL_NUA

#undef TUPL_MAX_ARITY
#undef TUPL_CONSTRUCT
#undef TUPL_MAKE

#include "namespace.hpp"

#undef NAMESPACE_ID
#undef TUPL_ID

#undef TUPL_PASS

#endif
