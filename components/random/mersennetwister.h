#pragma once
#include <algorithm>
#include <climits>
#include <cstddef>
#include <limits>
#include <ostream>
#include <type_traits>

// This file is copied + modified from libstdc++, version 7.2.0
// Copied to make sure we use the same RNG code on all platforms.
// Here is the original license header:

// Copyright (C) 2009-2018 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

namespace Random
{
    namespace __detail
    {
        // Replacement/wrapper for __builtin_clz/__builtin_clzl/__builtin_clzll in gcc, which doesn't exist in msvc.
        // Partially generated with the following python snippet:
        //
        // bits = 32 # can set this to 64 too
        // for i in range(bits-1, -1, -1):
        //     print('((val & 0x{}) ? {} :'.format('{:x}'.format(2**i).zfill(bits/8*2), bits-1-i))

        inline constexpr int builtin_clz_replacement(unsigned int val)
        {
            return  ((val & 0x80000000) ? 0 :
                    (((val & 0x40000000) ? 1 :
                    (((val & 0x20000000) ? 2 :
                    (((val & 0x10000000) ? 3 :
                    (((val & 0x08000000) ? 4 :
                    (((val & 0x04000000) ? 5 :
                    (((val & 0x02000000) ? 6 :
                    (((val & 0x01000000) ? 7 :
                    (((val & 0x00800000) ? 8 :
                    (((val & 0x00400000) ? 9 :
                    (((val & 0x00200000) ? 10 :
                    (((val & 0x00100000) ? 11 :
                    (((val & 0x00080000) ? 12 :
                    (((val & 0x00040000) ? 13 :
                    (((val & 0x00020000) ? 14 :
                    (((val & 0x00010000) ? 15 :
                    (((val & 0x00008000) ? 16 :
                    (((val & 0x00004000) ? 17 :
                    (((val & 0x00002000) ? 18 :
                    (((val & 0x00001000) ? 19 :
                    (((val & 0x00000800) ? 20 :
                    (((val & 0x00000400) ? 21 :
                    (((val & 0x00000200) ? 22 :
                    (((val & 0x00000100) ? 23 :
                    (((val & 0x00000080) ? 24 :
                    (((val & 0x00000040) ? 25 :
                    (((val & 0x00000020) ? 26 :
                    (((val & 0x00000010) ? 27 :
                    (((val & 0x00000008) ? 28 :
                    (((val & 0x00000004) ? 29 :
                    (((val & 0x00000002) ? 30 :
                    (((val & 0x00000001) ? 31 : 32)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
        }
        inline constexpr int builtin_clz_replacement(int val) { return builtin_clz_replacement(static_cast<unsigned int>(val)); }

        static_assert(sizeof(long long) == 8, "long long is 64 bit");
        inline constexpr int builtin_clz_replacement(unsigned long long val)
        {
            return  ((val & 0x8000000000000000) ? 0 :
                    ((val & 0x4000000000000000) ? 1 :
                    ((val & 0x2000000000000000) ? 2 :
                    ((val & 0x1000000000000000) ? 3 :
                    ((val & 0x0800000000000000) ? 4 :
                    ((val & 0x0400000000000000) ? 5 :
                    ((val & 0x0200000000000000) ? 6 :
                    ((val & 0x0100000000000000) ? 7 :
                    ((val & 0x0080000000000000) ? 8 :
                    ((val & 0x0040000000000000) ? 9 :
                    ((val & 0x0020000000000000) ? 10 :
                    ((val & 0x0010000000000000) ? 11 :
                    ((val & 0x0008000000000000) ? 12 :
                    ((val & 0x0004000000000000) ? 13 :
                    ((val & 0x0002000000000000) ? 14 :
                    ((val & 0x0001000000000000) ? 15 :
                    ((val & 0x0000800000000000) ? 16 :
                    ((val & 0x0000400000000000) ? 17 :
                    ((val & 0x0000200000000000) ? 18 :
                    ((val & 0x0000100000000000) ? 19 :
                    ((val & 0x0000080000000000) ? 20 :
                    ((val & 0x0000040000000000) ? 21 :
                    ((val & 0x0000020000000000) ? 22 :
                    ((val & 0x0000010000000000) ? 23 :
                    ((val & 0x0000008000000000) ? 24 :
                    ((val & 0x0000004000000000) ? 25 :
                    ((val & 0x0000002000000000) ? 26 :
                    ((val & 0x0000001000000000) ? 27 :
                    ((val & 0x0000000800000000) ? 28 :
                    ((val & 0x0000000400000000) ? 29 :
                    ((val & 0x0000000200000000) ? 30 :
                    ((val & 0x0000000100000000) ? 31 :
                    ((val & 0x0000000080000000) ? 32 :
                    ((val & 0x0000000040000000) ? 33 :
                    ((val & 0x0000000020000000) ? 34 :
                    ((val & 0x0000000010000000) ? 35 :
                    ((val & 0x0000000008000000) ? 36 :
                    ((val & 0x0000000004000000) ? 37 :
                    ((val & 0x0000000002000000) ? 38 :
                    ((val & 0x0000000001000000) ? 39 :
                    ((val & 0x0000000000800000) ? 40 :
                    ((val & 0x0000000000400000) ? 41 :
                    ((val & 0x0000000000200000) ? 42 :
                    ((val & 0x0000000000100000) ? 43 :
                    ((val & 0x0000000000080000) ? 44 :
                    ((val & 0x0000000000040000) ? 45 :
                    ((val & 0x0000000000020000) ? 46 :
                    ((val & 0x0000000000010000) ? 47 :
                    ((val & 0x0000000000008000) ? 48 :
                    ((val & 0x0000000000004000) ? 49 :
                    ((val & 0x0000000000002000) ? 50 :
                    ((val & 0x0000000000001000) ? 51 :
                    ((val & 0x0000000000000800) ? 52 :
                    ((val & 0x0000000000000400) ? 53 :
                    ((val & 0x0000000000000200) ? 54 :
                    ((val & 0x0000000000000100) ? 55 :
                    ((val & 0x0000000000000080) ? 56 :
                    ((val & 0x0000000000000040) ? 57 :
                    ((val & 0x0000000000000020) ? 58 :
                    ((val & 0x0000000000000010) ? 59 :
                    ((val & 0x0000000000000008) ? 60 :
                    ((val & 0x0000000000000004) ? 61 :
                    ((val & 0x0000000000000002) ? 62 :
                    ((val & 0x0000000000000001) ? 63 : 64))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
        }
        inline constexpr int builtin_clz_replacement(long long val) { return builtin_clz_replacement(static_cast<unsigned long long>(val)); }

        static_assert(sizeof(long) == 4 || sizeof(long) == 8, "long is sensible");
        inline constexpr int builtin_clz_replacement(unsigned long val)
        {
            return (sizeof(long) == 4) ? builtin_clz_replacement(static_cast<unsigned int>(val))
                                       : builtin_clz_replacement(static_cast<unsigned long long>(val));
        }
        inline constexpr int builtin_clz_replacement(long val) { return builtin_clz_replacement(static_cast<unsigned long>(val)); }

        //// This is a helper function for the sort routines and for random.tcc.
        //  Precondition: __n > 0.
        inline constexpr int __lg(int __n) { return int(sizeof(int) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        inline constexpr unsigned __lg(unsigned __n) { return int(sizeof(int) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        inline constexpr long __lg(long __n) { return int(sizeof(long) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        inline constexpr unsigned long __lg(unsigned long __n) { return int(sizeof(long) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        inline constexpr long long __lg(long long __n) { return int(sizeof(long long) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        inline constexpr unsigned long long __lg(unsigned long long __n) { return int(sizeof(long long) * CHAR_BIT) - 1 - builtin_clz_replacement(__n); }

        // template <typename _UIntType, size_t __w, bool = __w<static_cast<size_t>(std::numeric_limits<_UIntType>::digits)> struct _Shift
        //{
        //  static const _UIntType __value = 0;
        //};

        // template <typename _UIntType, size_t __w> struct _Shift<_UIntType, __w, true>
        //{
        //    static const _UIntType __value = _UIntType(1) << __w;
        //};

        template <typename _UIntType> inline constexpr _UIntType shift(size_t __w)
        {
            return (__w == 0) ? 0 : ((__w >= std::numeric_limits<_UIntType>::digits) ? 0 : _UIntType(1) << __w);
        }

        template <typename _UIntType, size_t __w> struct _Shift
        {
            static const _UIntType __value = shift<_UIntType>(__w);
        };

        // template <int __s,
        //          int __which = ((__s <= CHAR_BIT * sizeof(int)) + (__s <= CHAR_BIT * sizeof(long)) +
        //                         (__s <= CHAR_BIT * sizeof(long long))
        //                         /* assume long long no bigger than __int128 */
        //                         + (__s <= 128))>
        // struct _Select_uint_least_t
        //{
        //    static_assert(__which < 0, /* needs to be dependent */
        //                  "sorry, would be too much trouble for a slow result");
        //};

        // template <int __s> struct _Select_uint_least_t<__s, 4>
        //{
        //  typedef unsigned int type;
        //};

        // template <int __s> struct _Select_uint_least_t<__s, 3>
        //{
        //  typedef unsigned long type;
        //};

        // template <int __s> struct _Select_uint_least_t<__s, 2>
        //{
        //  typedef unsigned long long type;
        //};

        // Assume a != 0, a < m, c < m, x < m.
        template <typename _Tp, _Tp __m, _Tp __a, _Tp __c /*,
                  bool __big_enough = (!(__m & (__m - 1)) || (_Tp(-1) - __c) / __a >= __m - 1),
                  bool __schrage_ok = __m % __a<__m / __a*/>
        struct _Mod
        {
            // typedef typename _Select_uint_least_t<__lg(__a) + __lg(__m) + 2>::type _Tp2;
            typedef uint64_t _Tp2;
            static _Tp __calc(_Tp __x) { return static_cast<_Tp>((_Tp2(__a) * __x + __c) % __m); }
        };

        template <typename _Tp, _Tp __m, _Tp __a = 1, _Tp __c = 0> inline _Tp __mod(_Tp __x) { return _Mod<_Tp, __m, __a, __c>::__calc(__x); }
    }

    /**
     * A generalized feedback shift register discrete random number generator.
     *
     * This algorithm avoids multiplication and division and is designed to be
     * friendly to a pipelined architecture.  If the parameters are chosen
     * correctly, this generator will produce numbers with a very long period and
     * fairly good apparent entropy, although still not cryptographically strong.
     *
     * The best way to use this generator is with the predefined mt19937 class.
     *
     * This algorithm was originally invented by Makoto Matsumoto and
     * Takuji Nishimura.
     *
     * @tparam __w  Word size, the number of bits in each element of
     *              the state vector.
     * @tparam __n  The degree of recursion.
     * @tparam __m  The period parameter.
     * @tparam __r  The separation point bit index.
     * @tparam __a  The last row of the twist matrix.
     * @tparam __u  The first right-shift tempering matrix parameter.
     * @tparam __d  The first right-shift tempering matrix mask.
     * @tparam __s  The first left-shift tempering matrix parameter.
     * @tparam __b  The first left-shift tempering matrix mask.
     * @tparam __t  The second left-shift tempering matrix parameter.
     * @tparam __c  The second left-shift tempering matrix mask.
     * @tparam __l  The second right-shift tempering matrix parameter.
     * @tparam __f  Initialization multiplier.
     */
    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    class mersenne_twister_engine
    {
        static_assert(std::is_unsigned<_UIntType>::value, "result_type must be an unsigned integral type");
        static_assert(1u <= __m && __m <= __n, "template argument substituting __m out of bounds");
        static_assert(__r <= __w,
                      "template argument substituting "
                      "__r out of bound");
        static_assert(__u <= __w,
                      "template argument substituting "
                      "__u out of bound");
        static_assert(__s <= __w,
                      "template argument substituting "
                      "__s out of bound");
        static_assert(__t <= __w,
                      "template argument substituting "
                      "__t out of bound");
        static_assert(__l <= __w,
                      "template argument substituting "
                      "__l out of bound");
        static_assert(__w <= std::numeric_limits<_UIntType>::digits, "template argument substituting __w out of bound");
        static_assert(__a <= (__detail::shift<_UIntType>(__w) - 1), "template argument substituting __a out of bound");
        static_assert(__b <= (__detail::shift<_UIntType>(__w) - 1), "template argument substituting __b out of bound");
        static_assert(__c <= (__detail::shift<_UIntType>(__w) - 1), "template argument substituting __c out of bound");
        static_assert(__d <= (__detail::shift<_UIntType>(__w) - 1), "template argument substituting __d out of bound");
        static_assert(__f <= (__detail::shift<_UIntType>(__w) - 1), "template argument substituting __f out of bound");

    public:
        /** The type of the generated random value. */
        typedef _UIntType result_type;

        // parameter values
        static constexpr size_t word_size = __w;
        static constexpr size_t state_size = __n;
        static constexpr size_t shift_size = __m;
        static constexpr size_t mask_bits = __r;
        static constexpr result_type xor_mask = __a;
        static constexpr size_t tempering_u = __u;
        static constexpr result_type tempering_d = __d;
        static constexpr size_t tempering_s = __s;
        static constexpr result_type tempering_b = __b;
        static constexpr size_t tempering_t = __t;
        static constexpr result_type tempering_c = __c;
        static constexpr size_t tempering_l = __l;
        static constexpr result_type initialization_multiplier = __f;
        static constexpr result_type default_seed = 5489u;

        // constructors and member function
        explicit mersenne_twister_engine(result_type __sd = default_seed) { seed(__sd); }

        /**
         * @brief Constructs a %mersenne_twister_engine random number generator
         *        engine seeded from the seed sequence @p __q.
         *
         * @param __q the seed sequence.
         */
        template <typename _Sseq, typename = typename std::enable_if<!std::is_same<_Sseq, mersenne_twister_engine>::value>::type>
        explicit mersenne_twister_engine(_Sseq& __q)
        {
            seed(__q);
        }

        void seed(result_type __sd = default_seed);

        template <typename _Sseq> typename std::enable_if<std::is_class<_Sseq>::value>::type seed(_Sseq& __q);

        /**
         * @brief Gets the smallest possible value in the output range.
         */
        static constexpr result_type min() { return 0; }

        /**
         * @brief Gets the largest possible value in the output range.
         */
        static constexpr result_type max() { return __detail::_Shift<_UIntType, __w>::__value - 1; }

        /**
         * @brief Discard a sequence of random numbers.
         */
        void discard(unsigned long long __z);

        result_type operator()();

        /**
         * @brief Compares two % mersenne_twister_engine random number generator
         *        objects of the same type for equality.
         *
         * @param __lhs A % mersenne_twister_engine random number generator
         *              object.
         * @param __rhs Another % mersenne_twister_engine random number
         *              generator object.
         *
         * @returns true if the infinite sequences of generated values
         *          would be equal, false otherwise.
         */
        friend bool operator==(const mersenne_twister_engine& __lhs, const mersenne_twister_engine& __rhs)
        {
            return (std::equal(__lhs._M_x, __lhs._M_x + state_size, __rhs._M_x) && __lhs._M_p == __rhs._M_p);
        }

        /**
         * @brief Inserts the current state of a % mersenne_twister_engine
         *        random number generator engine @p __x into the output stream
         *        @p __os.
         *
         * @param __os An output stream.
         * @param __x  A % mersenne_twister_engine random number generator
         *             engine.
         *
         * @returns The output stream with the state of @p __x inserted or in
         * an error state.
         */
        template <typename _UIntType1,
                  size_t __w1,
                  size_t __n1,
                  size_t __m1,
                  size_t __r1,
                  _UIntType1 __a1,
                  size_t __u1,
                  _UIntType1 __d1,
                  size_t __s1,
                  _UIntType1 __b1,
                  size_t __t1,
                  _UIntType1 __c1,
                  size_t __l1,
                  _UIntType1 __f1,
                  typename _CharT,
                  typename _Traits>
        friend std::basic_ostream<_CharT, _Traits>&
        operator<<(std::basic_ostream<_CharT, _Traits>& __os,
                   const mersenne_twister_engine<_UIntType1, __w1, __n1, __m1, __r1, __a1, __u1, __d1, __s1, __b1, __t1, __c1, __l1, __f1>& __x);

        /**
         * @brief Extracts the current state of a % mersenne_twister_engine
         *        random number generator engine @p __x from the input stream
         *        @p __is.
         *
         * @param __is An input stream.
         * @param __x  A % mersenne_twister_engine random number generator
         *             engine.
         *
         * @returns The input stream with the state of @p __x extracted or in
         * an error state.
         */
        template <typename _UIntType1,
                  size_t __w1,
                  size_t __n1,
                  size_t __m1,
                  size_t __r1,
                  _UIntType1 __a1,
                  size_t __u1,
                  _UIntType1 __d1,
                  size_t __s1,
                  _UIntType1 __b1,
                  size_t __t1,
                  _UIntType1 __c1,
                  size_t __l1,
                  _UIntType1 __f1,
                  typename _CharT,
                  typename _Traits>
        friend std::basic_istream<_CharT, _Traits>&
        operator>>(std::basic_istream<_CharT, _Traits>& __is,
                   mersenne_twister_engine<_UIntType1, __w1, __n1, __m1, __r1, __a1, __u1, __d1, __s1, __b1, __t1, __c1, __l1, __f1>& __x);

    private:
        void _M_gen_rand();

        _UIntType _M_x[state_size];
        size_t _M_p;
    };

    /**
     * @brief Compares two % mersenne_twister_engine random number generator
     *        objects of the same type for inequality.
     *
     * @param __lhs A % mersenne_twister_engine random number generator
     *              object.
     * @param __rhs Another % mersenne_twister_engine random number
     *              generator object.
     *
     * @returns true if the infinite sequences of generated values
     *          would be different, false otherwise.
     */
    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    inline bool operator!=(const mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>& __lhs,
                           const mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>& __rhs)
    {
        return !(__lhs == __rhs);
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::word_size;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::state_size;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::shift_size;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::mask_bits;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::xor_mask;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_u;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_d;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_s;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_b;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_t;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_c;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr size_t mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::tempering_l;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::initialization_multiplier;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    constexpr _UIntType mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::default_seed;

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    void mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::seed(result_type __sd)
    {
        _M_x[0] = __detail::__mod<_UIntType, __detail::_Shift<_UIntType, __w>::__value>(__sd);

        for (size_t __i = 1; __i < state_size; ++__i)
        {
            _UIntType __x = _M_x[__i - 1];
            __x ^= __x >> (__w - 2);
            __x *= __f;
            __x += __detail::__mod<_UIntType, __n>(__i);
            _M_x[__i] = __detail::__mod<_UIntType, __detail::_Shift<_UIntType, __w>::__value>(__x);
        }
        _M_p = state_size;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    template <typename _Sseq>
    typename std::enable_if<std::is_class<_Sseq>::value>::type
    mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::seed(_Sseq& __q)
    {
        const _UIntType __upper_mask = (~_UIntType()) << __r;
        const size_t __k = (__w + 31) / 32;
        uint_least32_t __arr[__n * __k];
        __q.generate(__arr + 0, __arr + __n * __k);

        bool __zero = true;
        for (size_t __i = 0; __i < state_size; ++__i)
        {
            _UIntType __factor = 1u;
            _UIntType __sum = 0u;
            for (size_t __j = 0; __j < __k; ++__j)
            {
                __sum += __arr[__k * __i + __j] * __factor;
                __factor *= __detail::_Shift<_UIntType, 32>::__value;
            }
            _M_x[__i] = __detail::__mod<_UIntType, __detail::_Shift<_UIntType, __w>::__value>(__sum);

            if (__zero)
            {
                if (__i == 0)
                {
                    if ((_M_x[0] & __upper_mask) != 0u)
                        __zero = false;
                }
                else if (_M_x[__i] != 0u)
                    __zero = false;
            }
        }
        if (__zero)
            _M_x[0] = __detail::_Shift<_UIntType, __w - 1>::__value;
        _M_p = state_size;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    void mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::_M_gen_rand(void)
    {
        const _UIntType __upper_mask = (~_UIntType()) << __r;
        const _UIntType __lower_mask = ~__upper_mask;

        for (size_t __k = 0; __k < (__n - __m); ++__k)
        {
            _UIntType __y = ((_M_x[__k] & __upper_mask) | (_M_x[__k + 1] & __lower_mask));
            _M_x[__k] = (_M_x[__k + __m] ^ (__y >> 1) ^ ((__y & 0x01) ? __a : 0));
        }

        for (size_t __k = (__n - __m); __k < (__n - 1); ++__k)
        {
            _UIntType __y = ((_M_x[__k] & __upper_mask) | (_M_x[__k + 1] & __lower_mask));
            _M_x[__k] = (_M_x[__k + (__m - __n)] ^ (__y >> 1) ^ ((__y & 0x01) ? __a : 0));
        }

        _UIntType __y = ((_M_x[__n - 1] & __upper_mask) | (_M_x[0] & __lower_mask));
        _M_x[__n - 1] = (_M_x[__m - 1] ^ (__y >> 1) ^ ((__y & 0x01) ? __a : 0));
        _M_p = 0;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    void mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::discard(unsigned long long __z)
    {
        while (__z > state_size - _M_p)
        {
            __z -= state_size - _M_p;
            _M_gen_rand();
        }
        _M_p += __z;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f>
    typename mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::result_type
    mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>::operator()()
    {
        // Reload the vector - cost is O(n) amortized over n calls.
        if (_M_p >= state_size)
            _M_gen_rand();

        // Calculate o(x(i)).
        result_type __z = _M_x[_M_p++];
        __z ^= (__z >> __u) & __d;
        __z ^= (__z << __s) & __b;
        __z ^= (__z << __t) & __c;
        __z ^= (__z >> __l);

        return __z;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f,
              typename _CharT,
              typename _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os,
               const mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>& __x)
    {
        typedef std::basic_ostream<_CharT, _Traits> __ostream_type;
        typedef typename __ostream_type::ios_base __ios_base;

        const typename __ios_base::fmtflags __flags = __os.flags();
        const _CharT __fill = __os.fill();
        const _CharT __space = __os.widen(' ');
        __os.flags(__ios_base::dec | __ios_base::fixed | __ios_base::left);
        __os.fill(__space);

        for (size_t __i = 0; __i < __n; ++__i)
            __os << __x._M_x[__i] << __space;
        __os << __x._M_p;

        __os.flags(__flags);
        __os.fill(__fill);
        return __os;
    }

    template <typename _UIntType,
              size_t __w,
              size_t __n,
              size_t __m,
              size_t __r,
              _UIntType __a,
              size_t __u,
              _UIntType __d,
              size_t __s,
              _UIntType __b,
              size_t __t,
              _UIntType __c,
              size_t __l,
              _UIntType __f,
              typename _CharT,
              typename _Traits>
    std::basic_istream<_CharT, _Traits>& operator>>(std::basic_istream<_CharT, _Traits>& __is,
                                                    mersenne_twister_engine<_UIntType, __w, __n, __m, __r, __a, __u, __d, __s, __b, __t, __c, __l, __f>& __x)
    {
        typedef std::basic_istream<_CharT, _Traits> __istream_type;
        typedef typename __istream_type::ios_base __ios_base;

        const typename __ios_base::fmtflags __flags = __is.flags();
        __is.flags(__ios_base::dec | __ios_base::skipws);

        for (size_t __i = 0; __i < __n; ++__i)
            __is >> __x._M_x[__i];
        __is >> __x._M_p;

        __is.flags(__flags);
        return __is;
    }

    /**
     * The classic Mersenne Twister.
     *
     * Reference:
     * M. Matsumoto and T. Nishimura, Mersenne Twister: A 623-Dimensionally
     * Equidistributed Uniform Pseudo-Random Number Generator, ACM Transactions
     * on Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.
     */

    // Note: original source uses uint_fast32_t here, which on linux it seems is actually 64 bit.
    // This actually seems to be buggy if uint_fast32_t is actually a uint32_t, as we end up shifting
    // by 32 bits, which results in zeros in places we don't want them. So, I just changed it here.
    typedef mersenne_twister_engine<uint64_t, 32, 624, 397, 31, 0x9908b0dfUL, 11, 0xffffffffUL, 7, 0x9d2c5680UL, 15, 0xefc60000UL, 18, 1812433253UL> mt19937;
}