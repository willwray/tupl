#include "tupl.hpp"
#include <cassert>

using namespace ltl;

template <typename T>
constexpr std::add_const_t<T>& as_const(T& t) noexcept { return t; }

#if defined (_MSC_VER)
#define MSVC(...)__VA_ARGS__
#else
#define MSVC(...)
#endif

static_assert( tupl{1,2L,'3'} > tupl{1,2L,'2'} );

bool test_refs()
{
    int i;
    long j;
    char c;
    tupl<int&,long&,char&> ijc{i,j,c}; // tie
    ijc = {1,2L,'d'};  // assign-through (std tuple & tie don't have this)
    auto& [x,y,z] = ijc;
  //ijc = { x, y, z }; // FAIL: selects deleted default copy-assignment
    as_const(ijc) = { x, y, z }; // Reference types are top-level const

    assert(x == 1 && y == 2 && z == 'd');
    assert(x == get<0>(ijc) && y == get<1>(ijc) && z == get<2>(ijc));
    assert(&x == &get<0>(ijc) && &y == &get<1>(ijc) && &z == &get<2>(ijc));
    assert((ijc == tupl<int&, long&, char&>{x, y, z}));
    return true;
};

struct c16 { char data[16]; };
struct BIG { char data[20]; };

using tupl_scalar_refs = tupl<int&, long&, char&>;

static_assert( std::is_aggregate<tupl_scalar_refs>() );
static_assert(std::is_trivially_copyable<tupl_scalar_refs>());
static_assert(!std::is_trivially_default_constructible<tupl_scalar_refs>());
static_assert( MSVC(!) std::is_trivial_v<tupl_scalar_refs>);


static_assert( std::same_as<tupl_assign_fwd_t<tupl<int&,long&,char&>>,
                                              tupl<int, long, char>> );

static_assert( std::same_as<tupl_assign_fwd_t<tupl<int[20]>>,
                                              tupl<int const(&)[20]>> );

static_assert( std::same_as<tupl_assign_fwd_t<tupl<c16>>,
                                              tupl<c16>> );

static_assert( std::same_as<tupl_assign_fwd_t<tupl<BIG>>,
                                              tupl<BIG&&>> );

void big() {
    tupl c {c16{}};
    c = {};
    c16 cc{};
    c = {cc};

    tupl b {BIG{}};
    b = {};
    BIG bb{};
    b = {bb}; // default

    //tupl<BIG&> br{bb};
    //br = {};
    //br = {{}};
    //tupl<BIG&> const cbr{bb};
    //cbr = {};
    //cbr = {{}};
    //BIG bc{};
    //br = {bc}; // ambiguous tupl<BIG&> deleted, tupl<BIG&
    //cbr = {bc};  // not ambiguous
    //cbr = {"loo"};
    //br = {(BIG const&)bc}; // disambiguated - exact match
//    assert( )
}

int main()
{
    test_refs();

    tupl mut {1,2L,'3'};
    tupl mvt {1,2L,'2'};
    mut = {};
    //mut = {42}; // missing initializer warning
    //mut = {2,3,4}; // missing initializer warning
    return mut <=> mut == 0
        && mut <=> mvt > 0;
}