project(ticpp)

add_definitions("-DTIXML_USE_TICPP")
add_definitions("-DTIXML_USE_STL")

file(GLOB TICPP_HEADERS *.h)
file(GLOB TICPP_SOURCES *.cpp)

add_library(ticpp STATIC ${TICPP_HEADERS} ${TICPP_SOURCES})
