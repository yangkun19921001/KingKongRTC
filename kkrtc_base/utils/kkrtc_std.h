//
// Created by devyk on 2023/9/19.
//

#ifndef KKRTC_KKRTC_STD_H
#define KKRTC_KKRTC_STD_H

#ifndef __cplusplus
#  error cvstd.hpp header must be compiled as C++
#endif
#include <cstddef>
#include <cstring>
#include <cctype>

#include <string>

// import useful primitives from stl
#  include <algorithm>
#  include <utility>
#  include <cstdlib> //for abs(int)
#  include <cmath>

#if !defined _MSC_VER && !defined __BORLANDC__
#  if defined __cplusplus && __cplusplus >= 201103L && !defined __APPLE__
#    include <cstdint>
#    ifdef __NEWLIB__
        typedef unsigned int uint;
#    else
        typedef std::uint32_t uint;
#    endif
#  else
#    include <stdint.h>
     typedef uint32_t uint;
#  endif
#else
typedef unsigned uint;
#endif

typedef signed char schar;

#ifndef __IPL_H__
typedef unsigned char uchar;
typedef unsigned short ushort;
#endif


#if defined _MSC_VER || defined __BORLANDC__
typedef __int64 int64;
typedef unsigned __int64 uint64;
#  define KK_BIG_INT(n)   n##I64
#  define KK_BIG_UINT(n)  n##UI64
#else
typedef int64_t int64;
   typedef uint64_t uint64;
#  define KK_BIG_INT(n)   n##LL
#  define KK_BIG_UINT(n)  n##ULL
#endif

namespace kkrtc {
    static inline uchar abs(uchar a) { return a; }
    static inline ushort abs(ushort a) { return a; }
    static inline unsigned abs(unsigned a) { return a; }
    static inline uint64 abs(uint64 a) { return a; }

    using std::min;
    using std::max;
    using std::abs;
    using std::swap;
    using std::sqrt;
    using std::exp;
    using std::pow;
    using std::log;
}

#include "kkrtc_std_wrapper.h"

namespace kkrtc {

//! @addtogroup core_utils
//! @{

//////////////////////////// memory management functions ////////////////////////////

/** @brief Allocates an aligned memory buffer.

The function allocates the buffer of the specified size and returns it. When the buffer size is 16
bytes or more, the returned buffer is aligned to 16 bytes.
@param bufSize Allocated buffer size.
 */
     void* fastMalloc(size_t bufSize);

/** @brief Deallocates a memory buffer.

The function deallocates the buffer allocated with fastMalloc . If NULL pointer is passed, the
function does nothing. C version of the function clears the pointer *pptr* to avoid problems with
double memory deallocation.
@param ptr Pointer to the allocated buffer.
 */
     void fastFree(void* ptr);

/*!
  The STL-compliant memory Allocator based on cv::fastMalloc() and cv::fastFree()
*/
    template<typename _Tp> class Allocator
    {
    public:
        typedef _Tp value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        template<typename U> class rebind { typedef Allocator<U> other; };

        explicit Allocator() {}
        ~Allocator() {}
        explicit Allocator(Allocator const&) {}
        template<typename U>
        explicit Allocator(Allocator<U> const&) {}

        // address
        pointer address(reference r) { return &r; }
        const_pointer address(const_reference r) { return &r; }

        pointer allocate(size_type count, const void* =0) { return reinterpret_cast<pointer>(fastMalloc(count * sizeof (_Tp))); }
        void deallocate(pointer p, size_type) { fastFree(p); }

        void construct(pointer p, const _Tp& v) { new(static_cast<void*>(p)) _Tp(v); }
        void destroy(pointer p) { p->~_Tp(); }

        size_type max_size() const { return max(static_cast<_Tp>(-1)/sizeof(_Tp), 1); }
    };

//! @} core_utils

//! @endcond

//! @addtogroup core_basic
//! @{

//////////////////////////////// string class ////////////////////////////////

    class  FileNode; //for string constructor from FileNode

    typedef std::string String;

#ifndef OPENCV_DISABLE_STRING_LOWER_UPPER_CONVERSIONS

//! @cond IGNORED
    namespace details {
// std::tolower is int->int
        static inline char char_tolower(char ch)
        {
            return (char)std::tolower((int)ch);
        }
// std::toupper is int->int
        static inline char char_toupper(char ch)
        {
            return (char)std::toupper((int)ch);
        }
    } // namespace details
//! @endcond

    static inline std::string toLowerCase(const std::string& str)
    {
        std::string result(str);
        std::transform(result.begin(), result.end(), result.begin(), details::char_tolower);
        return result;
    }

    static inline std::string toUpperCase(const std::string& str)
    {
        std::string result(str);
        std::transform(result.begin(), result.end(), result.begin(), details::char_toupper);
        return result;
    }

#endif // OPENCV_DISABLE_STRING_LOWER_UPPER_CONVERSIONS

//! @} core_basic
} // cv

#endif //KKRTC_KKRTC_STD_H
