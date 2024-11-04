#pragma once

// Macro for constexpr, to support in mixed 03/0x mode.
#ifndef _UPPXX17_CONSTEXPR
# if __cplusplus >= 201703L
#  define _UPPXX17_CONSTEXPR constexpr
# else
#  define _UPPXX17_CONSTEXPR
# endif
#endif

#if __cplusplus <= 201703L
#define CAP_VALUE_AND_THIS =
#else
#define CAP_VALUE_AND_THIS =,this
#endif

#ifndef _UPPXX20_CONSTEXPR
# if __cplusplus >= 202002L
#  define _UPPXX20_CONSTEXPR constexpr
# else
#  define _UPPXX20_CONSTEXPR
# endif
#endif

#ifndef _UPPXX23_CONSTEXPR
# if __cplusplus >= 202100L
#  define _UPPXX23_CONSTEXPR constexpr
# else
#  define _UPPXX23_CONSTEXPR
# endif
#endif
