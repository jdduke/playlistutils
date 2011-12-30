cmake_minimum_required (VERSION 2.6)
project(PU)

set(CMAKE_DEBUG_POSTFIX "d")

#find_package(Boost COMPONENTS filesystem)
#if(NOT BOOST_FOUND)
#  set(BOOST_ROOT "$ENV{BOOST_DIR}/boost_1_48_0")
#  find_package(Boost COMPONENTS filesystem REQUIRED)
#endif()
#include_directories(${BOOST_INCLUDE_DIR})
#add_definitions( "-DHAS_BOOST" )

# Treat warnings as errors cmake option
SET(TREAT_WARNINGS_AS_ERRORS TRUE CACHE BOOL "Treat warnings as errors")

if(MSVC10)
  set(PU_SUFFIX _vc10)
elseif(MSVC11)
  set(PU_SUFFIX _vc11)
else()
  set(PU_SUFFIX)
endif()

if(CMAKE_SIZEOF_VOID_P MATCHES 4)
  set(PU_SIZE_TYPE x86)
else()
  set(PU_SIZE_TYPE x64)
endif()

set(BUILD_SHARED_LIBS TRUE CACHE BOOL "TRUE to build PU as shared libraries, FALSE to build it as static libraries")
if(BUILD_SHARED_LIBS)
  set(PU_LIBRARY_TYPE SHARED)
  set(PU_LIBRARY_PATH ${CMAKE_HOME_DIRECTORY}/lib/${PU_SIZE_TYPE}${PU_SUFFIX}_shared)
else()
  add_definitions("-DPU_STATIC")
  set(PU_LIBRARY_TYPE STATIC)
  set(PU_LIBRARY_PATH  ${CMAKE_HOME_DIRECTORY}/lib/${PU_SIZE_TYPE}${PU_SUFFIX}_static)
  set(CMAKE_EXECUTABLE_SUFFIX s${CMAKE_EXECUTABLE_SUFFIX})
endif()

set(PU_BINARY_PATH ${CMAKE_HOME_DIRECTORY}/bin/${PU_SIZE_TYPE}${PU_SUFFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PU_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Executables.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PU_BINARY_PATH}
    CACHE PATH
    "Single Directory for all Libraries")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PU_LIBRARY_PATH}
    CACHE PATH
    "Single Directory for all static libraries.")

if(UNIX)
  if(TREAT_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
  endif()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x -Wno-deprecated")
    add_definitions( "-DDEBUG" )
  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "-g -O3 -Wall -std=c++0x -fno-omit-frame-pointer")
  elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithProfile")
    set(CMAKE_CXX_FLAGS "-pg -g -O3 -Wall -std=c++0x -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS "-pg -fPIC")
    set(CMAKE_MODULE_LINKER_FLAGS "-pg")
  else()
    set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++0x -Wno-unused-variable -Wno-write-strings -fno-strict-aliasing  -Wno-parentheses -Wno-deprecated -fno-omit-frame-pointer")
  endif()
endif()

if(WIN32)
  SET(LINK_STATIC_RUNTIME OFF CACHE BOOL "Link statically against C++ runtime")
  if(TREAT_WARNINGS_AS_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX /MP /W3")
  if(MSVC10 OR MSVC11)
    add_definitions("-D_ITERATOR_DEBUG_LEVEL=0")
  endif()
  # Extra speed optimisation options
  set(MSVC_EXTRA_OPTIMIZE ON CACHE BOOL "Use extra optimization flags in release builds")
  if(MSVC_EXTRA_OPTIMIZE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi /Ot /Oy /GL /GS- /fp:fast")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /Ot /Oy /GL /GS- /fp:fast")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG")
  endif()
  # Even more speed optimisations
  set(MSVC_OPTIMIZE_SSE2 OFF CACHE BOOL "Use SSE2 instruction set in release builds")
  if(MSVC_OPTIMIZE_SSE2)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /arch:SSE2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /arch:SSE2")
  endif()
  #ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  #ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  if(LINK_STATIC_RUNTIME AND NOT BUILD_SHARED_LIBS)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:msvcrt /NODEFAULTLIB:msvcrtd")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt /NODEFAULTLIB:msvcrtd")
  endif()
endif()


SET(ENABLE_QT FALSE CACHE BOOL "Enable Qt usage")
if(ENABLE_QT)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_DEBUG")
  endif()
  add_definitions("-DHAS_QT")
  if(BUILD_SHARED_LIBS)
    add_definitions("-DQT_DLL -DQWT_DLL")
  else()
    add_definitions("-DPU_STATIC -DQT_NODLL")
  endif()
endif()

set(TICPP_LIB ticpp)

include_directories(${PU_SOURCE_DIR}/pu/include)
include_directories(${PU_SOURCE_DIR}/pu/public)
include_directories(${PU_SOURCE_DIR}/common)

add_subdirectory(pu)

SET(BUILD_SAMPLES TRUE CACHE BOOL "Build samples")
if(BUILD_SAMPLES)
  add_subdirectory(external/ticpp)
  add_subdirectory(samples)
endif()
