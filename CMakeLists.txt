project(Maximum-Fish)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML/cmake/Modules" ${CMAKE_MODULE_PATH})

add_definitions(-DCOUNT_EVENTS)

option(USE_CUSTOM_BOOST "" OFF)
option(USE_CUSTOM_SFML "" ON)
set(CUSTOM_BOOST_ROOT "" CACHE PATH "custom boost build root dir")
set(CUSTOM_SFML_ROOT "${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML" CACHE PATH "custom SFML build root dir")

set(SFGUI_BUILD_EXAMPLES false CACHE BOOL "Do or dont build examples)")
set( BUILD_SHARED_LIBS false CACHE BOOL "Build dynamic library." )
set( SFML_STATIC_LIBRARIES true CACHE BOOL "Was SFML built as a static library?" )

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

if(USE_CUSTOM_BOOST)
	message("-- Using custom boost...")
	set(BOOST_ROOT "${CUSTOM_BOOST_ROOT}")
endif()

if(USE_CUSTOM_SFML)
	message("-- Using custom SFML...")
	set(SFML_ROOT "${CUSTOM_SFML_ROOT}/build")
	set(SFML_INCLUDE_DIR "${CUSTOM_SFML_ROOT}/include")
endif(USE_CUSTOM_SFML)

find_package(Boost 1.46 COMPONENTS thread system)# regex)
#only audio required, the others are pulled in by sfgui
find_package(SFML 2 COMPONENTS audio)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
else(Boost_FOUND)
  message( FATAL_ERROR "!! Boost not found!")
endif()
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else(SFML_FOUND)
  message( FATAL_ERROR "!! SFML not found!\n   Please build SFML in '${CUSTOM_SFML_ROOT}/build' first!")
endif()

include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${PROJECT_SOURCE_DIR}/libsbe/include )
include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/include )
#include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML/include )

add_subdirectory(libsbe)

#needed on windows to find the libs sfml provides
if(CMAKE_SYSTEM_NAME MATCHES Windows)
  link_directories( ${CUSTOM_SFML_ROOT}/extlibs/libs-mingw/ )
endif()
message("-- System is ${CMAKE_SYSTEM_NAME}")

set(SRCDIR ${PROJECT_SOURCE_DIR}/src )

file( GLOB_RECURSE SRCS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.cpp" )
file( GLOB_RECURSE HDRS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.h" )

message ( "-- Sfml: ${CUSTOM_SFML_ROOT}" )

get_property(LIBS GLOBAL PROPERTY SBE_LINK_LIBS )
message ( "-- Linking: ${LIBS}")

add_definitions( -DSFML_STATIC -DSFGUI_STATIC)

if (GCC_VERSION VERSION_GREATER 4.6)
    add_definitions ( -std=c++11 )
else()
    add_definitions ( -std=c++0x )
endif()


add_executable( MaximumFish ${HDRS} ${SRCS}  )
#target_link_libraries(MaximumFish sbe ${LIBS} )
target_link_libraries(MaximumFish sbe sfgui )


