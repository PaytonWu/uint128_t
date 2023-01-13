#if !defined(UINT128_T_CONFIG_H)
#define UINT128_T_CONFIG_H
#pragma once

#if !defined(__cplusplus) || __cplusplus < 202002L
#   error "C++20 or above is required"
#endif

#if defined(_MSC_VER)
#   if defined(UINT128_T_DLL)
#       if defined(UINT128_T_EXPORT)
#           define UINT128_T_API __declspec(dllexport)
#       else
#           define UINT128_T_API __declspec(dllimport)
#       endif
#   else
#       define UINT128_T_API
#   endif
#else
#   define UINT128_T_API __attribute__((visibility("default")))
#endif

#define UINT128_T_EXTERN_C extern

#endif
