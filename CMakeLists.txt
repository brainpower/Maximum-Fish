project(Maximum-Fish)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML/cmake/Modules" ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

#######################################################
######################## Cmake Options ################
#######################################################

option( PROFILE "" OFF)
if(NOT USE_CUSTOM_BOOST)
	option(USE_CUSTOM_BOOST "" OFF)
endif()
option(USE_CUSTOM_SFML "" ON)
set(CUSTOM_BOOST_ROOT "${CUSTOM_BOOST_ROOT}" CACHE PATH "custom boost build root dir")
set(CUSTOM_SFML_ROOT "${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML" CACHE PATH "custom SFML build root dir")

option(SUPPRESS_BOOST_HEADER_WARNINGS "only works with gcc, so it's got no effect when using another compiler" ON)
option(ENABLE_LTO "enable lto, if ya dont know what this is, ya dont need it!" OFF)

set( SFGUI_BUILD_EXAMPLES false CACHE BOOL "Do or dont build examples)")
set( BUILD_SHARED_LIBS false CACHE BOOL "Build dynamic library." )
set( SFML_STATIC_LIBRARIES true CACHE BOOL "Was SFML built as a static library?" )

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(VERSION_CHECK_DONE OFF)

if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE "Release")
endif()

#######################################################
# compiler version detection                          #
#######################################################

if (CMAKE_COMPILER_IS_GNUCXX AND NOT VERSION_CHECK_DONE)
	# get gcc compiler version
	# if newer cmake, CMAKE_CXX_COMPILER_VERSION is set, so use it...
	# if not, call 'c++ -dumpversion' and get it from there
	if(CMAKE_CXX_COMPILER_VERSION)
		set(GCC_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
	else()
		execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	endif()

	# supress boost header warnings by telling gcc they're system headers

endif()

#######################################################
################## External Libraries #################
#######################################################

if(USE_CUSTOM_BOOST)
	message(STATUS "Using custom boost...")
	message(STATUS "setting BOOST_ROOT to ${CUSTOM_BOOST_ROOT}")
	set(Boost_NO_SYSTEM_PATHS ON)
	set(BOOST_ROOT "${CUSTOM_BOOST_ROOT}")
	set(BOOST_LIBRARYDIR "${CUSTOM_BOOST_ROOT}/lib")
endif()

if(USE_CUSTOM_SFML)
	message(STATUS "Using custom SFML...")
	set(SFML_ROOT "${CUSTOM_SFML_ROOT}/build")
	set(SFML_INCLUDE_DIR "${CUSTOM_SFML_ROOT}/include")
endif(USE_CUSTOM_SFML)

find_package(Boost 1.46 COMPONENTS thread system)# regex)
#only audio required, the others are pulled in by sfgui
find_package(SFML 2 COMPONENTS audio)

#######################################################
####################### SBE ###########################
#######################################################

#set( SBE_DEBUG ${DEBUG} CACHE BOOL "" )
add_subdirectory(libsbe)

# check for libraries

if(Boost_FOUND)
	if(CMAKE_COMPILER_IS_GNUCXX AND SUPPRESS_BOOST_HEADER_WARNINGS)
		set(CMAKE_CXX_FLAGS "-isystem${Boost_INCLUDE_DIR}")
	endif()
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message( FATAL_ERROR "!! Boost not found!")
endif()

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
else()
	message( FATAL_ERROR "!! SFML not found!\n   Please build SFML in '${CUSTOM_SFML_ROOT}/build' first!")
endif()



#######################################################
################### File inclusion ####################
#######################################################

set(SRCDIR ${PROJECT_SOURCE_DIR}/src )

file( GLOB_RECURSE SRCS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.cpp" )
file( GLOB_RECURSE HDRS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.hpp" "${SRCDIR}/*.ipp" )


#######################################################
##################### Compiler Options ################
#######################################################

include_directories( ${PROJECT_SOURCE_DIR}/src )

get_property(INCLUDE_DIRS GLOBAL PROPERTY SBE_INCLUDE_DIRS)

include_directories( ${INCLUDE_DIRS} )
#include_directories( ${PROJECT_SOURCE_DIR}/libsbe/include )
#include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/include )
#include_directories( ${PROJECT_SOURCE_DIR}/libsbe/SFGUI/extlibs/SFML/include )


#needed on windows to find the libs sfml provides
if(CMAKE_SYSTEM_NAME MATCHES Windows)
  link_directories( ${CUSTOM_SFML_ROOT}/extlibs/libs-mingw/ )
endif()

if(NOT VERSION_CHECK_DONE)
	# set some definitions depending on gcc version
	if (GCC_VERSION VERSION_EQUAL 4.7 OR GCC_VERSION VERSION_GREATER 4.7)
	   add_definitions ( -std=c++11 )

		if(GCC_VERSION VERSION_EQUAL 4.8 OR GCC_VERSION VERSION_GREATER 4.8)
			if(GCC_VERSION VERSION_EQUAL 4.9 OR GCC_VERSION VERSION_GREATER 4.9)
				add_definitions( -D__GCC_4_9__ )
			else()
				add_definitions ( -D__GCC_4_8__ )
			endif()
			if(CMAKE_BUILD_TYPE MATCHES Debug)
				add_definitions( -Og )
			endif()
		endif()
	else()
		if(GCC_VERSION VERSION_EQUAL 4.6 OR GCC_VERSION VERSION_GREATER 4.6)
	    add_definitions ( -std=c++0x -D__GCC_4_6__ )
		else()
			message("-- Unsupported compiler detected. Build might fail!")
			# changing this to -std=c++0x if you've got an old compiler, might work
			add_definitions( -std=c++11 )
		endif()

	endif()
	set(VERSION_CHECK_DONE ON)
endif()

add_definitions( -DBOOST_THREAD_USE_LIB -pipe )

if(NOT SHARED)
	add_definitions( -DSFML_STATIC -DSFGUI_STATIC)
endif()
if(ENABLE_LTO)
	add_definitions( -flto )
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "System is ${CMAKE_SYSTEM_NAME}")
	add_definitions( -fstack-protector --param=ssp-buffer-size=4 ) 
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	if( PROFILE )
		add_definitions( -O1 )
	else()
		add_definitions( -fomit-frame-pointer -fexpensive-optimizations -O3 )
	endif()
endif()

if( PROFILE )
	add_definitions( -pg )
endif()

add_executable( MaximumFish ${HDRS} ${SRCS}  )
target_link_libraries(MaximumFish sbe sfgui )

if( PROFILE )
	set_target_properties( MaximumFish PROPERTIES LINK_FLAGS "-O1 -pg" )
endif()
if( ENABLE_LTO )
	set_target_properties( MaximumFish PROPERTIES LINK_FLAGS "-flto" )
endif()
