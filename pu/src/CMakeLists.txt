project(PU)

file(GLOB PU_HEADERS ../include/*.h)
file(GLOB PU_SOURCES *.h *.cpp)

add_definitions(-DPU_EXPORTS)
add_library(pu ${PU_HEADERS} ${PU_SOURCES})
target_link_libraries(pu ${QT_QTCORE_LIBRARY} ${TICPP_LIB})
