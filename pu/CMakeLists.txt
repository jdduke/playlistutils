project(PU)

file(GLOB PU_HEADERS include/*.h)
file(GLOB PU_PUBLIC  public/*.h)
file(GLOB PU_SOURCES src/*.h src/*.cpp)

source_group(Public FILES ${PU_PUBLIC})
#source_group(Include FILES ${PU_HEADERS})
#source_group(Source FILES ${PU_SOURCE})

if(BUILD_SHARED_LIBS)
  add_definitions(-DPU_EXPORTS)
endif()

add_library(pu ${PU_PUBLIC} ${PU_HEADERS} ${PU_SOURCES})
