project(txml)

add_definitions("-DTIXML_USE_TICPP")
add_definitions("-DTIXML_USE_STL")

file(GLOB TXML_HEADERS *.h)
file(GLOB TXML_SOURCES *.cpp)

add_library(txml STATIC ${TXML_HEADERS} ${TXML_SOURCES})
