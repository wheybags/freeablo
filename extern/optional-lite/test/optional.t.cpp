//
// Copyright 2014-2018 by Martin Moene
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// optional lite is inspired on std::optional by Fernando Cacciola and Andrzej Krzemienski
// and on expected lite by Martin Moene.

#include "optional-main.t.hpp"

using namespace nonstd;

#if optional_USES_STD_OPTIONAL && defined(__APPLE__)
# define opt_value( o ) *o
#else
# define opt_value( o )  o.value()
#endif

namespace {

struct nonpod { nonpod(){} };

struct Implicit { int x;          Implicit(int v) : x(v) {} };
struct Explicit { int x; explicit Explicit(int v) : x(v) {} };

bool operator==( Implicit a, Implicit b ) { return a.x == b.x; }
bool operator==( Explicit a, Explicit b ) { return a.x == b.x; }

std::ostream & operator<<( std::ostream & os, Implicit i ) { return os << "Implicit:" << i.x; }
std::ostream & operator<<( std::ostream & os, Explicit e ) { return os << "Explicit:" << e.x; }

// ensure comparison of pointers for lest:

// const void * lest_nullptr = 0;

// The following tracer code originates as Oracle from Optional by
// Andrzej Krzemienski, https://github.com/akrzemi1/Optional.

enum State
{
    /* 0 */ default_constructed,
    /* 1 */ value_copy_constructed,
    /* 2 */ value_move_constructed,
    /* 3 */ copy_constructed,
    /* 4 */ move_constructed,
    /* 5 */ move_assigned,
    /* 6 */ copy_assigned,
    /* 7 */ value_copy_assigned,
    /* 8 */ value_move_assigned,
    /* 9 */ moved_from,
    /*10 */ value_constructed
};

struct V
{
    State state;
    int   value;

    V(       ) : state( default_constructed ), value( deflt() ) {}
    V( int v ) : state( value_constructed   ), value( v       ) {}

    bool operator==( V const & rhs ) const { return state == rhs.state && value == rhs.value; }
    bool operator==( int       val ) const { return value == val; }

    static int deflt() { return 42; }
};

struct S
{
    State state;
    V     value;

    S(             ) : state( default_constructed    ) {}
    S( V const & v ) : state( value_copy_constructed ), value( v ) {}
    S( S const & s ) : state( copy_constructed       ), value( s.value        ) {}

    S & operator=( V const & v ) { state = value_copy_assigned; value = v; return *this; }
    S & operator=( const S & s ) { state = copy_assigned      ; value = s.value; return *this; }

#if optional_CPP11_OR_GREATER
    S(             V && v ) : state(  value_move_constructed ), value(  std::move( v       ) ) { v.state = moved_from; }
    S(             S && s ) : state(  move_constructed       ), value(  std::move( s.value ) ) { s.state = moved_from; }

    S & operator=( V && v ) { state = value_move_assigned     ; value = std::move( v       ); v.state = moved_from; return *this; }
    S & operator=( S && s ) { state = move_assigned           ; value = std::move( s.value ); s.state = moved_from; return *this; }
#endif

    bool operator==( S const & rhs ) const { return state == rhs.state && value == rhs.value; }
};

inline std::ostream & operator<<( std::ostream & os, V const & v )
{
    using lest::to_string;
    return os << "[V:" << to_string( v.value ) << "]";
}

inline std::ostream & operator<<( std::ostream & os, S const & s )
{
    using lest::to_string;
    return os << "[S:" << to_string( s.value ) << "]";
}

struct NoDefault
{
    NoDefault( NoDefault const & ) {}
    NoDefault & operator=( NoDefault const & ) { return *this; }

#if optional_CPP11_OR_GREATER
    NoDefault( NoDefault && ) = default;
    NoDefault & operator=( NoDefault && ) = default;
#endif

private:
    NoDefault();
};

struct CopyOnly
{
    CopyOnly( CopyOnly const & ) {}
    CopyOnly & operator=( CopyOnly const & ) { return *this; }

private:
    CopyOnly();
#if optional_CPP11_OR_GREATER
    CopyOnly( CopyOnly && ) = delete;
    CopyOnly & operator=( CopyOnly && ) = delete;
#endif
};

struct MoveOnly
{
#if optional_CPP11_OR_GREATER
    MoveOnly( MoveOnly && ) = default;
    MoveOnly & operator=( MoveOnly && ) = default;
#endif

private:
    MoveOnly();
    MoveOnly( MoveOnly const & );
    MoveOnly & operator=( MoveOnly const & );
};

struct NoDefaultCopyMove
{
    std::string text;
    NoDefaultCopyMove( std::string txt ) : text( txt ) {}

private:
    NoDefaultCopyMove();
    NoDefaultCopyMove( NoDefaultCopyMove const & );
    NoDefaultCopyMove & operator=( NoDefaultCopyMove const & );
#if optional_CPP11_OR_GREATER
    NoDefaultCopyMove( NoDefaultCopyMove && ) = delete;
    NoDefaultCopyMove & operator=( NoDefaultCopyMove && ) = delete;
#endif
};

#if optional_CPP11_OR_GREATER
struct InitList
{
    std::vector<int> vec;
    char c;
    S s;

    InitList( std::initializer_list<int> il, char k, S const & t )
    : vec( il ), c( k ), s( t ) {}

    InitList( std::initializer_list<int> il, char k, S && t )
    : vec( il ), c( k ), s( std::move( t ) ) {}
};
#endif

} // anonymous namespace

//
// test specification:
//

CASE( "union: A C++03 union can only contain POD types" )
{
    union U
    {
        char c;
#if optional_CPP11_OR_GREATER
        nonpod np;
#endif
    };
}

//
// optional member operations:
//

// construction:

CASE( "optional: Allows to default construct an empty optional (1a)" )
{
    optional<int> a;

    EXPECT( !a );
}

CASE( "optional: Allows to explicitly construct a disengaged, empty optional via nullopt (1b)" )
{
    optional<int> a( nullopt );

    EXPECT( !a );
}

CASE( "optional: Allows to default construct an empty optional with a non-default-constructible (1a)" )
{
//  FAILS: NoDefault nd;
//  FAILS: NoDefaultCopyMove ndcm;
    optional<NoDefault> ond;
    optional<CopyOnly> oco;
    optional<MoveOnly> omo;
    optional<NoDefaultCopyMove> ondcm;

    EXPECT( !ond );
    EXPECT( !oco );
    EXPECT( !omo );
    EXPECT( !ondcm );
}

CASE( "optional: Allows to copy-construct from empty optional (2)" )
{
    optional<int> a;

    optional<int> b( a );

    EXPECT( !b );
}

CASE( "optional: Allows to move-construct from empty optional (C++11, 3)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a;

    optional<int> b( std::move( a ) );

    EXPECT( !b );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-construct from empty optional, explicit converting (C++11, 4a)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a;

    optional<Explicit> b{ a };

    EXPECT( !b );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-construct from empty optional, non-explicit converting (4b)" )
{
    optional<int> a;

    optional<Implicit> b( a );

    EXPECT( !b );
}

CASE( "optional: Allows to move-construct from empty optional, explicit converting (C++11, 5a)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a;

    optional<Explicit> b{ std::move( a ) };

    EXPECT( !b );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-construct from empty optional, non-explicit converting (C++11, 5a)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a;

    optional<Implicit> b( std::move( a ) );

    EXPECT( !b );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-construct from non-empty optional (2)" )
{
    optional<int> a( 7 );

    optional<int> b( a );

    EXPECT(  b      );
    EXPECT( *b == 7 );
}

CASE( "optional: Allows to copy-construct from non-empty optional, explicit converting (C++11, 4a)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a( 7 );

    optional<Explicit> b{ a };

    EXPECT(  b                );
    EXPECT( *b == Explicit{7} );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-construct from non-empty optional, non-explicit converting (4b)" )
{
    optional<int> a( 7 );

    optional<Implicit> b( a );

    EXPECT(  b                );
    EXPECT( *b == Implicit(7) );
}

CASE( "optional: Allows to move-construct from non-empty optional (C++11, 3)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a( 7 );

    optional<int> b( std::move( a ) );

    EXPECT(  b      );
    EXPECT( *b == 7 );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-construct from non-empty optional, explicit converting (C++11, 5a)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a( 7 );

    optional<Explicit> b{ std::move( a ) };

    EXPECT(  b                );
    EXPECT( *b == Explicit{7} );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-construct from non-empty optional, non-explicit converting (C++11, 5b)" )
{
#if optional_CPP11_OR_GREATER
    optional<int> a( 7 );

    optional<Implicit> b( std::move( a ) );

    EXPECT(  b                );
    EXPECT( *b == Implicit(7) );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

namespace {

#if optional_CPP11_OR_GREATER
    void use_optional( nonstd::optional<Implicit> ) {}
#else
    template< typename T >
    void use_optional( T ) {}
#endif

}

CASE( "optional: Allows to copy-construct from literal value (8)" )
{
    use_optional( 7 );
    optional<int> a = 7;

    EXPECT(  a      );
    EXPECT( *a == 7 );
}

CASE( "optional: Allows to copy-construct from literal value, converting (8)" )
{
    use_optional( '7' );
    optional<int> a = '7';

    EXPECT(  a        );
    EXPECT( *a == '7' );
}

CASE( "optional: Allows to copy-construct from value (8)" )
{
    const int i = 7;

    use_optional( i );
    optional<int> a( i );

    EXPECT(  a      );
    EXPECT( *a == 7 );
}

CASE( "optional: Allows to copy-construct from value, converting (8)" )
{
    const char c = '7';

    use_optional( c );
    optional<int> a( c );

    EXPECT(  a        );
    EXPECT( *a == '7' );
}

CASE( "optional: Allows to move-construct from value (C++11, 8b)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );

    optional<S> a( std::move( s ) );

    EXPECT( a->value == 7                );
    EXPECT( a->state == move_constructed );
    EXPECT(  s.state == moved_from       );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-construct from value, explicit converting (C++11, 8a)" )
{
#if optional_CPP11_OR_GREATER
    int seven = 7;

    optional<Explicit> a{ std::move( seven ) };

    EXPECT(  a                );
    EXPECT( *a == Explicit{7} );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-construct from value, non-explicit converting (C++11, 8b)" )
{
#if optional_CPP11_OR_GREATER
    int seven = 7;
    optional<Implicit> a( std::move( seven ) );

    EXPECT(  a                );
    EXPECT( *a == Implicit(7) );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to in-place construct from literal value (C++11, 6)" )
{
#if optional_CPP11_OR_GREATER
    using pair_t = std::pair<char, int>;

    optional<pair_t> a( in_place, 'a', 7 );

    EXPECT( a->first  == 'a' );
    EXPECT( a->second ==  7  );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to in-place copy-construct from value (C++11, 6)" )
{
#if optional_CPP11_OR_GREATER
    char c = 'a'; S s( 7 );
    using pair_t = std::pair<char, S>;

    optional<pair_t> a( in_place, c, s );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
#if optional_USES_STD_OPTIONAL
    EXPECT( a->second.state == copy_constructed );
#else
    EXPECT( a->second.state == move_constructed );
#endif
    EXPECT(         s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to in-place move-construct from value (C++11, 6)" )
{
#if optional_CPP11_OR_GREATER
    char c = 'a'; S s( 7 );
    using pair_t = std::pair<char, S>;

    optional<pair_t> a( in_place, c, std::move( s ) );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
    EXPECT( a->second.state == move_constructed );
    EXPECT(         s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to in-place copy-construct from initializer-list (C++11, 7)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    optional<InitList> a( in_place, { 7, 8, 9, }, 'a', s );

    EXPECT( a->vec[0]  ==  7 );
    EXPECT( a->vec[1]  ==  8 );
    EXPECT( a->vec[2]  ==  9 );
    EXPECT( a->c       == 'a');
    EXPECT( a->s.value ==  7 );
#if optional_USES_STD_OPTIONAL
    EXPECT( a->s.state == copy_constructed );
#else
    EXPECT( a->s.state == move_constructed );
#endif
    EXPECT(    s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to in-place move-construct from initializer-list (C++11, 7)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    optional<InitList> a( in_place, { 7, 8, 9, }, 'a', std::move( s ) );

    EXPECT( a->vec[0]  ==  7  );
    EXPECT( a->vec[1]  ==  8  );
    EXPECT( a->vec[2]  ==  9  );
    EXPECT( a->c       == 'a' );
    EXPECT( a->s.value ==  7  );
    EXPECT( a->s.state == move_constructed );
    EXPECT(    s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

// assignment:

CASE( "optional: Allows to assign nullopt to disengage (1)" )
{
    optional<int>  a( 7 );

    a = nullopt;

    EXPECT( !a );
}

CASE( "optional: Allows to copy-assign from/to engaged and disengaged optionals (2)" )
{
    SETUP( "" ) {
        optional<int> d1;
        optional<int> d2;
        optional<int> e1( 123 );
        optional<int> e2( 987 );

    SECTION( "a disengaged optional assigned nullopt remains empty" ) {
        d1 = nullopt;
        EXPECT( !d1 );
    }
    SECTION( "a disengaged optional assigned an engaged optional obtains its value" ) {
        d1 = e1;
        EXPECT(  d1 );
        EXPECT( *d1 == 123 );
    }
    SECTION( "an engaged optional assigned an engaged optional obtains its value" ) {
        e1 = e2;
        EXPECT(  e1 );
        EXPECT( *e1 == 987 );
    }
    SECTION( "an engaged optional assigned nullopt becomes empty" ) {
        e1 = nullopt;
        EXPECT( !e1 );
    }
    SECTION( "a disengaged optional assigned a disengaged optional remains empty" ) {
        d1 = d2;
        EXPECT( !d1 );
    }}
}

CASE( "optional: Allows to move-assign from/to engaged and disengaged optionals (C++11, 3)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<int> d1;
        optional<int> d2;
        optional<int> e1( 123 );
        optional<int> e2( 987 );

    SECTION( "a disengaged optional assigned nullopt remains empty" ) {
        d1 = std::move( nullopt );
        EXPECT( !d1 );
    }
    SECTION( "a disengaged optional assigned an engaged optional obtains its value" ) {
        d1 = std::move( e1);
        EXPECT(  d1 );
        EXPECT( *d1 == 123 );
    }
    SECTION( "an engaged optional assigned an engaged optional obtains its value" ) {
        e1 = std::move( e2 );
        EXPECT(  e1 );
        EXPECT( *e1 == 987 );
    }
    SECTION( "an engaged optional assigned nullopt becomes empty" ) {
        e1 = std::move( nullopt );
        EXPECT( !e1 );
    }
    SECTION( "a disengaged optional assigned a disengaged optional remains empty" ) {
        d1 = std::move( d2);
        EXPECT( !d1 );
    }}
#else
    EXPECT( !!"optional: move-assignment is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-assign from/to engaged and disengaged optionals, converting, (5)" )
{
    SETUP( "" ) {
        optional<int>  d1;
        optional<char> d2;
        optional<int>  e1( 123 );
        optional<char> e2( '7' );

    SECTION( "a disengaged optional assigned an engaged optional obtains its value, converting" ) {
        d1 = e2;
        EXPECT(  d1 );
        EXPECT( *d1 == '7' );
    }
    SECTION( "an engaged optional assigned an engaged optional obtains its value, converting" ) {
        e1 = e2;
        EXPECT(  e1 );
        EXPECT( *e1 == '7' );
    }
    SECTION( "an engaged optional assigned a disengaged optional becomes empty, converting" ) {
        e1 = d2;
        EXPECT(  !e1 );
    }
    SECTION( "a disengaged optional assigned a disengaged optional remains empty, converting" ) {
        d1 = d2;
        EXPECT( !d1 );
    }}
}

CASE( "optional: Allows to move-assign from/to engaged and disengaged optionals, converting (C++11, 6)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<int>  d1;
        optional<char> d2;
        optional<int>  e1( 123 );
        optional<char> e2( '7' );

    SECTION( "a disengaged optional assigned an engaged optional obtains its value, converting" ) {
        d1 = std::move( e2 );
        EXPECT(  d1 );
        EXPECT( *d1 == '7' );
    }
    SECTION( "an engaged optional assigned an engaged optional obtains its value, converting" ) {
        e1 = std::move( e2 );
        EXPECT(  e1 );
        EXPECT( *e1 == '7' );
    }
    SECTION( "an engaged optional assigned a disengaged optional becomes empty, converting" ) {
        e1 = std::move( d2 );
        EXPECT(  !e1 );
    }
    SECTION( "a disengaged optional assigned a disengaged optional remains empty, converting" ) {
        d1 = std::move( d2 );
        EXPECT( !d1 );
    }}
#else
    EXPECT( !!"optional: move-assignment is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-assign from literal value (4)" )
{
    optional<int> a;

    a = 7;

    EXPECT( *a == 7 );
}

CASE( "optional: Allows to copy-assign from value (4)" )
{
    const int i = 7;
    optional<int> a;

    a = i;

    EXPECT( *a == i );
}

CASE( "optional: Allows to move-assign from value (C++11, 4)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    optional<S> a;

    a = std::move( s );

    EXPECT( a->value == 7 );
    EXPECT( a->state == move_constructed );
    EXPECT(  s.state == moved_from       );
#else
    EXPECT( !!"optional: move-assign is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-emplace content from arguments (C++11, 7)" )
{
#if optional_CPP11_OR_GREATER
    using pair_t = std::pair<char, S>;
    S s( 7 );
    optional<pair_t> a;

    a.emplace( 'a', s );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
    EXPECT( a->second.state == copy_constructed );
    EXPECT(         s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-emplace content from arguments (C++11, 7)" )
{
#if optional_CPP11_OR_GREATER
    using pair_t = std::pair<char, S>;
    S s( 7 );
    optional<pair_t> a;

    a.emplace( 'a', std::move( s ) );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
    EXPECT( a->second.state == move_constructed );
    EXPECT(         s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to copy-emplace content from intializer-list and arguments (C++11, 8)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    optional<InitList> a;

    a.emplace( { 7, 8, 9, }, 'a', s );

    EXPECT( a->vec[0]  ==  7  );
    EXPECT( a->vec[1]  ==  8  );
    EXPECT( a->vec[2]  ==  9  );
    EXPECT( a->c       == 'a' );
    EXPECT( a->s.value ==  7  );
    EXPECT( a->s.state == copy_constructed );
    EXPECT(    s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "optional: Allows to move-emplace content from intializer-list and arguments (C++11, 8)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    optional<InitList> a;

    a.emplace( { 7, 8, 9, }, 'a', std::move( s ) );

    EXPECT( a->vec[0]  ==  7  );
    EXPECT( a->vec[1]  ==  8  );
    EXPECT( a->vec[2]  ==  9  );
    EXPECT( a->c       == 'a' );
    EXPECT( a->s.value ==  7               );
    EXPECT( a->s.state == move_constructed );
    EXPECT(    s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

// swap:

CASE( "optional: Allows to swap with other optional (member)" )
{
    SETUP( "" ) {
        optional<int> d1;
        optional<int> d2;
        optional<int> e1( 42 );
        optional<int> e2( 7 );

    SECTION( "swap disengaged with disengaged optional" ) {
        d1.swap( d2 );
        EXPECT( !d1 );
    }
    SECTION( "swap engaged with engaged optional" ) {
        e1.swap( e2 );
        EXPECT(  e1  );
        EXPECT(  e2 );
        EXPECT( *e1 == 7 );
        EXPECT( *e2 == 42 );
    }
    SECTION( "swap disengaged with engaged optional" ) {
        d1.swap( e1 );
        EXPECT(  d1 );
        EXPECT( !e1 );
        EXPECT( *d1 == 42 );
    }
    SECTION( "swap engaged with disengaged optional" ) {
        e1.swap( d1 );
        EXPECT(  d1 );
        EXPECT( !e1 );
        EXPECT( *d1 == 42 );
    }}
}

// observers:

CASE( "optional: Allows to obtain value via operator->()" )
{
    SETUP( "" ) {
        optional<Implicit>        e( Implicit( 42 ) );
        optional<Implicit> const ce( Implicit( 42 ) );

    SECTION( "operator->() yields pointer to value (const)" ) {
        EXPECT(  ce->x == 42 );
    }
    SECTION( "operator->() yields pointer to value (non-const)" ) {
        e->x = 7;
        EXPECT(  e->x == 7 );
    }}
}

CASE( "optional: Allows to obtain moved-value via operator->() (C++11)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<Implicit>        e( Implicit( 42 ) );
        optional<Implicit> const ce( Implicit( 42 ) );

    SECTION( "operator->() yields pointer to value (const)" ) {
        EXPECT(  std::move( ce )->x == 42 );
    }
    SECTION( "operator->() yields pointer to value (non-const)" ) {
        e->x = 7;
        EXPECT(  std::move( e )->x == 7 );
    }}
#else
    EXPECT( !!"optional: move-semantics are not available (no C++11)" );
#endif
}

CASE( "optional: Allows to obtain value via operator*()" )
{
    SETUP( "" ) {
        optional<int>        e( 42 );
        optional<int> const ce( 42 );

    SECTION( "operator*() yields value (const)" ) {
        EXPECT( *ce == 42 );
    }
    SECTION( "operator*() yields value (non-const)" ) {
        *e = 7;
        EXPECT( *e == 7 );
    }}
}

CASE( "optional: Allows to obtain moved-value via operator*() (C++11)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<int>        e( 42 );
        optional<int> const ce( 42 );

    SECTION( "operator*() yields value (const)" ) {
        EXPECT( *(std::move( ce )) == 42 );
    }
    SECTION( "operator*() yields value (non-const)" ) {
        *e = 7;
        EXPECT( *(std::move( e )) == 7 );
    }}
#else
    EXPECT( !!"optional: move-semantics are not available (no C++11)" );
#endif
}

CASE( "optional: Allows to obtain has_value() via operator bool()" )
{
    optional<int> a;
    optional<int> b( 7 );

    EXPECT_NOT( a );
    EXPECT(     b );
}

CASE( "optional: Allows to obtain value via value()" )
{
    SETUP( "" ) {
        optional<int> e( 42 );
        optional<int> const ce( 42 );

    SECTION( "value() yields value (const)" ) {
        EXPECT( opt_value( ce ) == 42 );
    }
    SECTION( "value() yields value (non-const)" ) {
        EXPECT( opt_value( e ) == 42 );
    }}
}

CASE( "optional: Allows to obtain moved-value via value() (C++11)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<int> e( 42 );
        optional<int> const ce( 42 );

    SECTION( "value() yields value (const)" ) {
        EXPECT( opt_value( std::move( ce ) ) == 42 );
    }
    SECTION( "value() yields value (non-const)" ) {
        EXPECT( opt_value( std::move( e ) ) == 42 );
    }}
#else
    EXPECT( !!"optional: move-semantics are not available (no C++11)" );
#endif
}

CASE( "optional: Allows to obtain value or default via value_or()" )
{
    SETUP( "" ) {
        optional<int> d;
        optional<int> e( 42 );

    SECTION( "value_or( 7 ) yields value for non-empty optional" ) {
        EXPECT( e.value_or( 7 ) == 42 );
    }
    SECTION( "value_or( 7 ) yields default for empty optional" ) {
        EXPECT( d.value_or( 7 ) == 7 );
    }}
}

CASE( "optional: Allows to obtain moved-value or moved-default via value_or() (C++11)" )
{
#if optional_CPP11_OR_GREATER
    SETUP( "" ) {
        optional<int> d;
        optional<int> e( 42 );

    SECTION("for l-values") {
        EXPECT( d.value_or( 7 ) ==  7 );
        EXPECT( e.value_or( 7 ) == 42 );
    }
    SECTION("for r-values") {
        EXPECT( std::move( d ).value_or( 7 ) ==  7 );
        EXPECT( std::move( e ).value_or( 7 ) == 42 );
    }}
#else
    EXPECT( !!"optional: move-semantics are not available (no C++11)" );
#endif
}

CASE( "optional: Throws bad_optional_access at disengaged access" )
{
#if optional_USES_STD_OPTIONAL && defined(__APPLE__)
    EXPECT( true );
#else
    SETUP( "" ) {
        optional<int> d;
        optional<int> const cd;

    SECTION("for l-values") {
        EXPECT_THROWS_AS(  d.value(), bad_optional_access );
        EXPECT_THROWS_AS( cd.value(), bad_optional_access );
    }
# if optional_CPP11_OR_GREATER
    SECTION("for r-values") {
        EXPECT_THROWS_AS( std::move(  d ).value(), bad_optional_access );
        EXPECT_THROWS_AS( std::move( cd ).value(), bad_optional_access );
    }
# endif
    }
#endif
}

CASE( "optional: Throws bad_optional_access with non-empty what()" )
{
    try
    {
        optional<int> d;
        (void) d.value();
    }
    catch( bad_optional_access const & e )
    {
        EXPECT( ! std::string( e.what() ).empty() );
    }
}

// modifiers:

CASE( "optional: Allows to reset content" )
{
    optional<int> a = 7;

    a.reset();

    EXPECT_NOT( a.has_value() );
}

//
// optional non-member functions:
//

CASE( "optional: Allows to swaps engage state and values (non-member)" )
{
    SETUP( "" ) {
        optional<int> d1;
        optional<int> d2;
        optional<int> e1( 42 );
        optional<int> e2( 7 );

    SECTION( "swap disengaged with disengaged optional" ) {
        swap( d1, d2 );
        EXPECT( !d1 );
    }
    SECTION( "swap engaged with engaged optional" ) {
        swap( e1, e2 );
        EXPECT(  e1  );
        EXPECT(  e2 );
        EXPECT( *e1 == 7 );
        EXPECT( *e2 == 42 );
    }
    SECTION( "swap disengaged with engaged optional" ) {
        swap( d1, e1 );
        EXPECT(  d1 );
        EXPECT( !e1 );
        EXPECT( *d1 == 42 );
    }
    SECTION( "swap engaged with disengaged optional" ) {
        swap( e1, d1 );
        EXPECT(  d1 );
        EXPECT( !e1 );
        EXPECT( *d1 == 42 );
    }}
}

template< typename R, typename S, typename T >
void relop( lest::env & lest_env )
{
    SETUP( "" ) {
        optional<R> d;
        optional<S> e1( 6 );
        optional<T> e2( 7 );

    SECTION( "engaged    == engaged"    ) { EXPECT(   e1 == e1  ); }
    SECTION( "engaged    == disengaged" ) { EXPECT( !(e1 == d ) ); }
    SECTION( "disengaged == engaged"    ) { EXPECT( !(d  == e1) ); }

    SECTION( "engaged    != engaged"    ) { EXPECT(   e1 != e2  ); }
    SECTION( "engaged    != disengaged" ) { EXPECT(   e1 != d   ); }
    SECTION( "disengaged != engaged"    ) { EXPECT(   d  != e2  ); }

    SECTION( "engaged    <  engaged"    ) { EXPECT(   e1 <  e2  ); }
    SECTION( "engaged    <  disengaged" ) { EXPECT( !(e1 <  d ) ); }
    SECTION( "disengaged <  engaged"    ) { EXPECT(   d  <  e2  ); }

    SECTION( "engaged    <= engaged"    ) { EXPECT(   e1 <= e1  ); }
    SECTION( "engaged    <= engaged"    ) { EXPECT(   e1 <= e2  ); }
    SECTION( "engaged    <= disengaged" ) { EXPECT( !(e1 <= d ) ); }
    SECTION( "disengaged <= engaged"    ) { EXPECT(   d  <= e2  ); }

    SECTION( "engaged    >  engaged"    ) { EXPECT(   e2 >  e1  ); }
    SECTION( "engaged    >  disengaged" ) { EXPECT(   e2 >  d   ); }
    SECTION( "disengaged >  engaged"    ) { EXPECT( !(d  >  e1) ); }

    SECTION( "engaged    >= engaged"    ) { EXPECT(   e1 >= e1  ); }
    SECTION( "engaged    >= engaged"    ) { EXPECT(   e2 >= e1  ); }
    SECTION( "engaged    >= disengaged" ) { EXPECT(   e2 >= d   ); }
    SECTION( "disengaged >= engaged"    ) { EXPECT( !(d  >= e1) ); }

    SECTION( "disengaged == nullopt"    ) { EXPECT(  (d       == nullopt) ); }
    SECTION( "nullopt    == disengaged" ) { EXPECT(  (nullopt == d      ) ); }
    SECTION( "engaged    == nullopt"    ) { EXPECT(  (e1      != nullopt) ); }
    SECTION( "nullopt    == engaged"    ) { EXPECT(  (nullopt != e1     ) ); }
    SECTION( "disengaged == nullopt"    ) { EXPECT( !(d       <  nullopt) ); }
    SECTION( "nullopt    == disengaged" ) { EXPECT( !(nullopt <  d      ) ); }
    SECTION( "disengaged == nullopt"    ) { EXPECT(  (d       <= nullopt) ); }
    SECTION( "nullopt    == disengaged" ) { EXPECT(  (nullopt <= d      ) ); }
    SECTION( "disengaged == nullopt"    ) { EXPECT( !(d       >  nullopt) ); }
    SECTION( "nullopt    == disengaged" ) { EXPECT( !(nullopt >  d      ) ); }
    SECTION( "disengaged == nullopt"    ) { EXPECT(  (d       >= nullopt) ); }
    SECTION( "nullopt    == disengaged" ) { EXPECT(  (nullopt >= d      ) ); }

    SECTION( "engaged    == value"      ) { EXPECT( e1 == 6  ); }
    SECTION( "value     == engaged"     ) { EXPECT(  6 == e1 ); }
    SECTION( "engaged   != value"       ) { EXPECT( e1 != 7  ); }
    SECTION( "value     != engaged"     ) { EXPECT(  6 != e2 ); }
    SECTION( "engaged   <  value"       ) { EXPECT( e1 <  7  ); }
    SECTION( "value     <  engaged"     ) { EXPECT(  6 <  e2 ); }
    SECTION( "engaged   <= value"       ) { EXPECT( e1 <= 7  ); }
    SECTION( "value     <= engaged"     ) { EXPECT(  6 <= e2 ); }
    SECTION( "engaged   >  value"       ) { EXPECT( e2 >  6  ); }
    SECTION( "value     >  engaged"     ) { EXPECT(  7 >  e1 ); }
    SECTION( "engaged   >= value"       ) { EXPECT( e2 >= 6  ); }
    SECTION( "value     >= engaged"     ) { EXPECT(  7 >= e1 ); }
    }
}

CASE( "optional: Provides relational operators" )
{
    relop<int, int, int>( lest_env );
}

CASE( "optional: Provides mixed-type relational operators" )
{
    relop<char, int, long>( lest_env );
}

CASE( "make_optional: Allows to copy-construct optional" )
{
    S s( 7 );

    EXPECT( make_optional( s )->value == 7          );
    EXPECT(                   s.state != moved_from );
}

CASE( "make_optional: Allows to move-construct optional (C++11)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );

    EXPECT( make_optional( std::move( s ) )->value == 7          );
    EXPECT(                                s.state == moved_from );
#else
    EXPECT( !!"optional: move-construction is not available (no C++11)" );
#endif
}

CASE( "make_optional: Allows to in-place copy-construct optional from arguments (C++11)" )
{
#if optional_CPP11_OR_GREATER
    using pair_t = std::pair<char, S>;

    S s( 7);
    auto a = make_optional<pair_t>( 'a', s );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
#if optional_USES_STD_OPTIONAL
    EXPECT( a->second.state == copy_constructed );
#else
    EXPECT( a->second.state == move_constructed );
#endif
    EXPECT(         s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "make_optional: Allows to in-place move-construct optional from arguments (C++11)" )
{
#if optional_CPP11_OR_GREATER
    using pair_t = std::pair<char, S>;

    S s( 7 );
    auto a = make_optional<pair_t>( 'a', std::move( s ) );

    EXPECT( a->first        == 'a' );
    EXPECT( a->second.value ==  7  );
    EXPECT( a->second.state == move_constructed );
    EXPECT(         s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "make_optional: Allows to in-place copy-construct optional from initializer-list and arguments (C++11)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    auto a = make_optional<InitList>( { 7, 8, 9, }, 'a', s );

    EXPECT( a->vec[0]  ==  7  );
    EXPECT( a->vec[1]  ==  8  );
    EXPECT( a->vec[2]  ==  9  );
    EXPECT( a->c       == 'a' );
    EXPECT( a->s.value ==  7  );
#if optional_USES_STD_OPTIONAL
    EXPECT( a->s.state == copy_constructed );
#else
    EXPECT( a->s.state == move_constructed );
#endif
    EXPECT(    s.state != moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "make_optional: Allows to in-place move-construct optional from initializer-list and arguments (C++11)" )
{
#if optional_CPP11_OR_GREATER
    S s( 7 );
    auto a = make_optional<InitList>( { 7, 8, 9, }, 'a', std::move( s ) );

    EXPECT( a->vec[0]  ==  7  );
    EXPECT( a->vec[1]  ==  8  );
    EXPECT( a->vec[2]  ==  9  );
    EXPECT( a->c       == 'a' );
    EXPECT( a->s.value ==  7  );
    EXPECT( a->s.state == move_constructed );
    EXPECT(    s.state == moved_from       );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

CASE( "std::hash<>: Allows to obtain hash (C++11)" )
{
#if optional_CPP11_OR_GREATER
    const auto a = optional<int>( 7 );
    const auto b = optional<int>( 7 );

    EXPECT( std::hash<optional<int>>{}( a ) == std::hash<optional<int>>{}( b ) );
#else
    EXPECT( !!"std::hash<>: std::hash<> is not available (no C++11)" );
#endif
}


//
// Negative tests:
//

//
// Tests that print information:
//

struct Struct{ Struct(){} };

#if !defined(optional_FEATURE_MAX_ALIGN_HACK) || !optional_FEATURE_MAX_ALIGN_HACK

#define optional_OUTPUT_ALIGNMENT_OF( type ) \
    "alignment_of<" #type ">: " <<  \
     alignment_of<type>::value  << "\n" <<

CASE("alignment_of: Show alignment of various types"
     "[.]" )
{
#if optional_CPP11_OR_GREATER
    using std::alignment_of;
#else
    using ::nonstd::optional_lite::detail::alignment_of;
#endif

    std::cout <<
        optional_OUTPUT_ALIGNMENT_OF( char )
        optional_OUTPUT_ALIGNMENT_OF( short )
        optional_OUTPUT_ALIGNMENT_OF( int )
        optional_OUTPUT_ALIGNMENT_OF( long )
        optional_OUTPUT_ALIGNMENT_OF( float )
        optional_OUTPUT_ALIGNMENT_OF( double )
        optional_OUTPUT_ALIGNMENT_OF( long double )
        optional_OUTPUT_ALIGNMENT_OF( Struct )
         "";
}
#undef optional_OUTPUT_ALIGNMENT_OF
#endif

#define optional_OUTPUT_SIZEOF( type ) \
    "sizeof( optional<" #type "> ): " << \
     sizeof( optional<   type>   )    << " (" << sizeof(type) << ")\n" <<

CASE("storage_t: Show sizeof various optionals"
     "[.]" )
{
    std::cout <<
#if !optional_USES_STD_OPTIONAL
        "sizeof( nonstd::optional_lite::detail::storage_t<char> ): " <<
         sizeof( nonstd::optional_lite::detail::storage_t<char> )    << "\n" <<
#endif
         optional_OUTPUT_SIZEOF( char )
         optional_OUTPUT_SIZEOF( short )
         optional_OUTPUT_SIZEOF( int )
         optional_OUTPUT_SIZEOF( long )
         optional_OUTPUT_SIZEOF( float )
         optional_OUTPUT_SIZEOF( double )
         optional_OUTPUT_SIZEOF( long double )
         optional_OUTPUT_SIZEOF( Struct )
         "";
}
#undef optional_OUTPUT_SIZEOF

//
// Issues:
//

CASE( "optional: isocpp-lib: CH 3, p0032r2 -- let's not have too clever tags" "[.issue-1]" )
{
    EXPECT( false );
#if 0
    optional< optional< optional<int> > > a (
        in_place,
#if 0
        in_place,
#else
//        nonstd_lite_in_place_type_t(int),
        static_cast< nonstd::in_place_t >( in_place ),
#endif
        nullopt
    );

    EXPECT(       a );
    EXPECT(      *a );
    EXPECT_NOT( **a );
#endif
}

namespace issue18 {

struct S
{
    static int & dtor_count() { static int i = 0; return i; }
    S( char /*c*/, int /*i*/ ) {}
    ~S() { ++dtor_count(); }
};
} // issue18

CASE( "optional: emplace does not construct in-place (destructor called while 'emplacing')" "[.issue-18]" )
{
#if optional_CPP11_OR_GREATER
    using issue18::S;
    {
        nonstd::optional<S> os;

        EXPECT( S::dtor_count() == 0 );

        os.emplace( 'c', 42 );

        EXPECT( S::dtor_count() == 0 );
    }
    EXPECT( S::dtor_count() == 1 );
#else
    EXPECT( !!"optional: in-place construction is not available (no C++11)" );
#endif
}

// end of file
