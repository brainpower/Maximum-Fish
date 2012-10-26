project(Maximum-Fish)

cmake_minimum_required(VERSION 2.8)


add_definitions(-DCOUNT_EVENTS)

include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${PROJECT_SOURCE_DIR}/libsbe/include )
include_directories( ${PROJECT_SOURCE_DIR}/../boost_1_51_0  )
include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/include )
include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML/include )

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

add_definitions( -DSFML_STATIC -DSFGUI_STATIC -std=c++11 )
add_executable( MaximumFish ${HDRS} ${SRCS}  )
#target_link_libraries(MaximumFish sbe ${LIBS} )
target_link_libraries(MaximumFish sbe sfgui )


