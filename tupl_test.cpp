#include "tupl.hpp"
#include <cassert>

using namespace ltl;

static_assert( tupl{1,2L,'3'} > tupl{1,2L,'2'} );

int test_refs()
{
    int i;
    long j;
    char c;
    tupl<int&,long&,char&> ijc{i,j,c};
    ijc = {1,2L,'d'};
    auto [x,y,z] = ijc;
    return x;
};

struct c16 { char data[16]; };
struct BIG { char data[20]; };

static_assert( std::is_aggregate_v<tupl<int&,long&,char&>> );
//static_assert( std::is_trivial_v<tupl<int&,long&,char&>> );

static_assert( std::same_as<tupl_ass_t<tupl<int&,long&,char&>>,
                                       tupl<int, long, char>> );

static_assert( std::same_as<tupl_ass_t<tupl<int[20]>>,
                                       tupl<int const(&)[20]>> );

static_assert( std::same_as<tupl_ass_t<tupl<c16>>,
                                       tupl<c16>> );

static_assert( std::same_as<tupl_ass_t<tupl<BIG>>,
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
    tupl mut {1,2L,'3'};
    tupl mvt {1,2L,'2'};
    mut = {};
    //mut = {42}; // missing initializer warning
    //mut = {2,3,4}; // missing initializer warning
    return mut <=> mut == 0
        && mut <=> mvt > 0;
}