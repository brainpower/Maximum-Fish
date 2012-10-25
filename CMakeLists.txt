project(Maximum-Fish)

cmake_minimum_required(VERSION 2.8)


LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/libsbe/build/ )

#target_link_libraries( libsbe )
add_definitions(-Wall -DCOUNT_EVENTS -fPIC)

include_directories(${PROJECT_SOURCE_DIR}/src )
include_directories(${PROJECT_SOURCE_DIR}/libsbe/include )


add_subdirectory(libsbe)
#add_subdirectory(src)

if(CMAKE_SYSTEM_NAME MATCHES Windows)
    add_definitions(-std=gnu++0x)
else()
    add_definitions(-std=c++0x)
endif()


set(SRCDIR ${PROJECT_SOURCE_DIR}/src )

set( blah "asf" )
#execute_process( COMMAND find ${SRCDIR} -name *.cpp ERROR_FILE out_err.txt OUTPUT_FILE out.txt OUTPUT_VARIABLE blah RESULT_VARIABLE rv )

file( GLOB_RECURSE SRCS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.cpp" )
file( GLOB_RECURSE HDRS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.h" )

message ( "SRCS: ${SRCS} \n HDRS: ${HDRS}" )

# headers and sources are defined in src/CMakeLists.txt
add_executable( MaximumFish ${HDRS} ${SRCS}  )


