cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    dependencygraph.cpp
    main.cpp)

#if(UNIX)
#    add_library(stdc++fs UNKNOWN IMPORTED)
#    set_property(TARGET stdc++fs PROPERTY IMPORTED_LOCATION "/usr/lib/gcc/x86_64-linux-gnu/7/libstdc++fs.a")
#endif()

FIND_PACKAGE(Boost COMPONENTS filesystem graph REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_executable(BSTPart3 ${SOURCES})

target_link_libraries(BSTPart3 ${Boost_LIBRARIES})
