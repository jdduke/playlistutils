file(GLOB SIMPLE_HEADERS *.h)
file(GLOB SIMPLE_SOURCES *.cpp)

set(EX_PROG "puSimple")
#if(MSVC10)
#  set(EX_PROG ${EX_PROG}_vc10)
#endif()

add_executable(${EX_PROG} ${SIMPLE_SOURCES} ${SIMPLE_HEADERS})
target_link_libraries(${EX_PROG} ${QT_QTCORE_LIBRARY} pu ${TICPP_LIB})
