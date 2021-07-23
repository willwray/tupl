#ifndef TUPL_PASS

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

#define TUPL_MAX_ARITY CAT(TUPL_ID,_max_arity) // tupl_max_arity
#define TUPL_CONSTRUCT CAT(TUPL_ID,_construct) // tupl_construct
#define TUPL_MAKE      CAT(TUPL_ID,_make)      // tupl_make

#include "namespace.hpp"

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

// CTAD deduce values including arrays (i.e. no decay to pointer)
template <typename...E>
TUPL_ID(E&&...) -> TUPL_ID<std::remove_cvref_t<E>...>;
//

// size(tupl) - for constexpr size decltype(size(tupl))()
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
using tupl_ass_t = typename tupl_type_map<assign_fwd,Tupl>::type;
//

// tupl<> zero-size pack specialization
template <> // tupl<T...> specializations defined in TUPL_PASS == 1
struct TUPL_ID<> {
  auto operator<=>(TUPL_ID const&) const = default;
  constexpr decltype(auto) operator()(auto f) const
     noexcept(noexcept(f())) { return f(); }
};

// ass(l): with a single-arg does value init(s); l = {}
template <typename L>
constexpr L& ass(L& l) noexcept(noexcept(flat_index(l)={}))
  requires requires { {flat_index(l)={}}; }
{
  if constexpr (!std::is_array_v<L>)
    return l = {};
  else {
    for (int i = 0; i != flat_size<L>; ++i)
      flat_index(l,i) = {};
    return l;
  }
}
//
// ass(l,r)
template <typename L, typename R>
constexpr L& ass(L& l, R&& r) noexcept(noexcept(ass(l)))
  requires same_extents_v<L,std::remove_cvref_t<R>>
        && requires { {flat_index(l) = flat_index((R&&)r)}; }
{
  if constexpr (!std::is_array_v<L>)
    return l = (R&&)r;
  else {
    for (int i = 0; i != flat_size<L>; ++i)
      flat_index(l,i) = flat_index((R&&)r,i);
    return l;
  }
}
//
template <typename L>
constexpr L& ass(L& l, L const& r) noexcept(noexcept(ass(l,r))) {
  return ass<L,L const&>(l,r);
}
//
template <tuplish R, typename...L>
constexpr TUPL_ID<L...>& ass(TUPL_ID<L...>& lhs, R&& rhs) noexcept(
 std::is_reference_v<R>
? (std::is_nothrow_move_assignable_v<std::remove_all_extents_t<L>>&&...)
: (std::is_nothrow_copy_assignable_v<std::remove_all_extents_t<L>>&&...)
) requires (decltype(size(rhs))() == sizeof...(L))
{
  lhs([&rhs](std::remove_reference_t<L>&...t) {
    rhs([&t...](auto&&...u) {
      if constexpr (std::is_reference_v<R>)
        (ass(t,(std::remove_reference_t<decltype(u)>&&)u),...);
      else
        (ass(t,u),...);
    });
  });
  return lhs;
}
//

// assign(tupl,datum...) foward decl
//template <typename...R, typename...A>
//constexpr TUPL_ID<A...>& assign(TUPL_ID<A...>&, R&&) noexcept(
//  (std::is_nothrow_copy_assignable_v<std::remove_all_extents_t<A>> && ...)
//);

//template <typename...E>
//constexpr auto make_tupl(E&&...e) -> tupl<std::remove_cvref_t<E>...> {
//    tupl<std::remove_cvref_t<E>...> o; // what if not direct initializable
//    o([&e...](std::remove_cvref_t<E>&...t){(assign(t,(E&&)e),...);});
//    return o;
//}

// tupl<T...> size N pack specialization
//#define TUPL_SPECIALIZATION(N) \
//template <TYPENAME_DECLS(N)> struct TUPL_ID<TUPL_TYPE_IDS(N)>\
//{ MEMBER_DECLS(N) OP3WAY OPASSIGN MAP_V(TUPL_DATA_IDS(N)) };
//

//#define VREPEAT_COUNT TUPL_MAX_INDEX
//#define VREPEAT_MACRO TUPL_SPECIALIZATION
//#include "VREPEAT.hpp"

// tupl<T...> size N pack specializations

#define TUPL_TYPE_ID XD
#define TUPL_DATA_ID xD
#define TYPENAME_DECL(n) typename TUPL_TYPE_ID(n)
#define TUPL_DATA_FWD(n) (TUPL_TYPE_ID(n)&&)TUPL_DATA_ID(n)
#define MEMBER_DECL(n) TUPL_NUA TUPL_TYPE_ID(n) TUPL_DATA_ID(n);

#define TUPL_TYPE_IDS IREPEAT(VREPEAT_INDEX,TUPL_TYPE_ID,COMMA)
#define TUPL_DATA_IDS IREPEAT(VREPEAT_INDEX,TUPL_DATA_ID,COMMA)
#define TYPENAME_DECLS IREPEAT(VREPEAT_INDEX,TYPENAME_DECL,COMMA)
#define TUPL_DATA_FWDS IREPEAT(VREPEAT_INDEX,TUPL_DATA_FWD,COMMA)
#define MEMBER_DECLS IREPEAT(VREPEAT_INDEX,MEMBER_DECL,NOSEP)

#define MAP_V(...) \
constexpr decltype(auto) operator()(auto f) noexcept(noexcept(f(__VA_ARGS__))) \
{ return f(__VA_ARGS__); }

#define TUPL_PASS 1
#define VREPEAT_COUNT TUPL_MAX_INDEX
#define VREPEAT_MACRO ../../tupl_impl_pre.cpp
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
 template<typename...>constexpr TUPL_ID& operator=(tupl_ass_t<TUPL_ID> r)
   requires (tupl_val<TUPL_ID>) {return ass(*this,r);}
 template<typename...>constexpr TUPL_ID const& operator=(tupl_ass_t<TUPL_ID> r) const
   requires (!tupl_val<TUPL_ID>) {return ass(*this,r);}
 MAP_V(TUPL_DATA_IDS)
};

#else

//#undef TUPL_SPECIALIZATION
#undef OP3WAY
#undef TYPENAMES
#undef TYPENAMEs
#undef TYPENAMEz
#undef TYPENAME_DECLs
#undef TYPENAME_DECLz
#undef TYPENAME_DECLS
#undef TYPENAME_DECL
#undef MEMBER_DECLs
#undef MEMBER_DECLS
#undef MEMBER_DECL
#undef TUPL_TYPE_ID

template <tuplish T> requires (! tupl_types_all<T,MEMBER_DEFAULT_3WAY>
                              && tupl_types_all<T,THREE_WAY_COMPARABLE>)
constexpr auto operator<=>(T const& l,T const& r) noexcept {
 constexpr int s = decltype(size(l))();
 constexpr compare_three_way cmp;
#define MACRO(N) if constexpr(HEXLIT(N)<s){if(auto c=cmp(\
l.TUPL_DATA_ID(N),r.TUPL_DATA_ID(N));c!=0||1+HEXLIT(N)==s)return c;}
 IREPEAT(TUPL_MAX_INDEX,MACRO,NOSEP)
#undef MACRO
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
    constexpr int s = decltype(size(l))();
#define MACRO(N) if constexpr(HEXLIT(N)<s){if(bool c=\
l.TUPL_DATA_ID(N)==r.TUPL_DATA_ID(N);!c||1+HEXLIT(N)==s)return c;}
 IREPEAT(TUPL_MAX_INDEX,MACRO,NOSEP)
#undef MACRO
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
}();
//
template <typename X, typename T>
inline constexpr int tupl_indexof = not defined(tupl_indexof<X,T>);
template <typename X, typename...A>
inline constexpr int tupl_indexof<X,TUPL_ID<A...>> = indexof<X,A...>;

#undef MACRO
#undef SEP

#undef EXPAND
#undef RECURS
#undef RECURs
#undef RECURz

template <auto...A, typename...E>
constexpr auto dupl(TUPL_ID<E...>const& a) -> TUPL_ID<decltype(+get<A>(a))...>
    { return {get<A>(a)...}; };

template <typename...A, typename...E>
constexpr auto dupl(TUPL_ID<E...>const& a) -> TUPL_ID<A...>
    { return {get<A>(a)...}; };

#undef TUPL_DATA_ID

#include "namespace.hpp"

#undef TUPL_ID

#endif
