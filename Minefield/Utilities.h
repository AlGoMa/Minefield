#pragma once
#include <math.h>
/*
    __linux__       Defined on Linux
    __unix__        Defined on Unit OS
    __sun           Defined on Solaris
    __FreeBSD__     Defined on FreeBSD
    __NetBSD__      Defined on NetBSD
    __OpenBSD__     Defined on OpenBSD
    __APPLE__       Defined on Mac OS X
    __hpux          Defined on HP-UX
    __osf__         Defined on Tru64 UNIX (formerly DEC OSF1)
    __sgi           Defined on Irix
    _AIX            Defined on AIX
    _WIN32          Defined on Windows
*/

template<typename t>
t safediv(t in_tI, t in_tJ, t in_tDefault)
{
    const unsigned uCondition = (unsigned)(in_tJ != 0.0f);

    const unsigned mask = 0U - uCondition;

    unsigned res = Reinterprete_Cast<unsigned, t>(in_tI / in_tJ);

    return Reinterprete_Cast<t, unsigned>((res & mask) | (Reinterprete_Cast<unsigned, t>(in_tDefault) & ~mask));
}

#if defined(SQRT_ACCURATE)
constexpr static float rsqrt(float number) noexcept
{
    STATIC_ASSERT(std::numeric_limits<float>::is_iec559); // (enable only on IEEE 754)

    float const fVal = std::bit_cast<float>(0x5f3759df - (std::bit_cast<std::uint32_t>(number) >> 1));
    return fVal * (1.5f - (number * 0.5f * fVal * fVal));
}
#else
#define rsqrt sqrt
#endif

#define GLUE_(a,b) a ## b
#define GLUE(a,b) GLUE_(a,b)

#if ASSERTIONS_ENABLED
#define debugBreak() asm { int 3 }
#define ASSERT(ex) \
        if (ex) {} \
        else \
        { \
            printf(""); \
            debugBreak(); \
        }

#ifdef __cplusplus
#if __cplusplus >= 201103L
#define STATIC_ASSERT(expr) \
            static_assert(expr, \
            "static assert failed: " \
            #expr)
#else
template<bool> class TStaticAssert;
template<> class TStaticAssert<true> {};
#define STATIC_ASSERT(expr) \
        enum \
        { \
           GLUE(g_assert_fail, GLUE(GLUE(_line_, __LINE__), GLUE(_indicents_, __COUNTER__))) = sizeof(TStaticAssert<!!(expr)>) \
        };
#endif
#else
#define STATIC_ASSERT(expr) \
    enum \
    { \
       GLUE(g_assert_fail_, __LINE__) = 1 / (int)(!!(expr)) \
    };
#endif
#else
#define ASSERT(expr)
#define STATIC_ASSERT(expr)
#endif

#define SAFE_RELEASE(p) if(nullptr != p) { delete p; p = nullptr;}

#define ThrowException(h) if(FAILED(h)){ throw std::exception();}