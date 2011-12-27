project(PU)

file(GLOB PU_HEADERS ../include/*.h)
file(GLOB PU_SOURCES *.h *.cpp)

add_library(pu ${PU_HEADERS} ${PU_SOURCES})
