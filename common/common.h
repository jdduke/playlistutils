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
  #define WIN32_LEAN_AND_MEAN
  #define PU_WINDOWS
  #if _WIN64
    #define PU_64
  #else
    #define PU_32
  #endif
  #if defined _DEBUG
    #define PU_DEBUG
  #else
    #define PU_RELEASE
  #endif
#else
  #if defined NDEBUG
    #define PU_RELEASE
  #else
    #define PU_DEBUG
  #endif
  #if __x86_64__ || __ppc64__
    #define PU_64
  #else
    #define PU_32
  #endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Define portable import / export macros
/////////////////////////////////////////////////////////////////////////////
#if defined _WIN32 || defined __CYGWIN__
  #define PU_HELPER_DLL_IMPORT __declspec(dllimport)
  #define PU_HELPER_DLL_EXPORT __declspec(dllexport)
  #define PU_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define PU_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define PU_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define PU_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define PU_HELPER_DLL_IMPORT
    #define PU_HELPER_DLL_EXPORT
    #define PU_HELPER_DLL_LOCAL
  #endif
#endif

#if defined PU_STATIC
  #define PU_API
  #define PU_LOCAL
#else
  #ifdef PU_EXPORTS
    #define PU_API PU_HELPER_DLL_EXPORT
  #else
    #define PU_API PU_HELPER_DLL_IMPORT
  #endif
  #define PU_LOCAL PU_HELPER_DLL_LOCAL
#endif

#if defined PU_EXPORTS || defined PU_STATIC
#  define PU_EXTERN
#else
#  define PU_EXTERN extern
#endif

#define PU_TEMPLATE_API_HELPER(ext, api, templ) ext template class api templ
#define PU_TEMPLATE_API(templ) PU_TEMPLATE_API_HELPER(PU_EXTERN, PU_API, templ)

/////////////////////////////////////////////////////////////////////////////
// Define some useful class macros
/////////////////////////////////////////////////////////////////////////////
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace pu {

class PU_API Releaser {
public:
  template< typename T>
  void operator()(T* ptr) {
    if( ptr ) { ptr->release(); }
  }
};

}

#endif
