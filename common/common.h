/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef _COMMON_H_
#define _COMMON_H

/////////////////////////////////////////////////////////////////////////////
// Define platform-specific macros
/////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
#  define WIN32_LEAN_AND_MEAN
#  define PU_WINDOWS
#  if _WIN64
#    define PU_64
#  else
#    define PU_32
#  endif
#  if defined _DEBUG
#    define PU_DEBUG
#  else
#    define PU_RELEASE
#  endif
#else
#  if defined NDEBUG
#     define PU_RELEASE
#  else
#    define PU_DEBUG
#  endif
#  if __x86_64__ || __ppc64__
#    define PU_64
#  else
#    define PU_32
#  endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Define portable import / export macros
/////////////////////////////////////////////////////////////////////////////
#if defined(PU_WINDOWS) && !defined(PU_STATIC)
#  ifdef PU_EXPORTS
#    define PU_API __declspec(dllexport)
#  else
#    define PU_API __declspec(dllimport)
#  endif
#else
// Other platforms and static build don't need these export macros
#  define PU_API
#endif

/////////////////////////////////////////////////////////////////////////////
// Define some useful class macros
/////////////////////////////////////////////////////////////////////////////
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#endif
