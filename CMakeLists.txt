cmake_minimum_required(VERSION 3.5)

project("restapi")

if (WIN32)
    set(BOOST_ROOT "external/boost")
endif (WIN32)

if (UNIX)
    set(BOOST_ROOT "/usr")
endif (UNIX)

set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(BOOST_ALL_DYN_LINK OFF)

find_package(Boost COMPONENTS)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
add_executable("restapi" src/main.cpp src/restapi/restapi.h src/restapi/restapi.cpp)
target_link_libraries("restapi" ${Boost_LIBRARIES})

