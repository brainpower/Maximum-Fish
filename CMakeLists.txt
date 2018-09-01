project(Maximum-Fish)

cmake_minimum_required(VERSION 2.8)

#######################################################
######################## Cmake Options ################
#######################################################

option( PROFILE "" OFF)

option(SUPPRESS_BOOST_HEADER_WARNINGS "only works with gcc, so it's got no effect when using another compiler" ON)
option(ENABLE_LTO "enable lto, if ya dont know what this is, ya dont need it!" OFF)

set( BUILD_SHARED_LIBS false CACHE BOOL "Build dynamic library." )

#set(VERSION_CHECK_DONE OFF)

if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE "Release")
endif()

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

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



find_package(Boost 1.55 COMPONENTS thread system filesystem REQUIRED)
find_package(SFML 2 COMPONENTS audio graphics window system REQUIRED)
find_package(SFGUI REQUIRED)
find_package(glm REQUIRED)
find_package(SBE REQUIRED)


#######################################################
####################### SBE ###########################
#######################################################

#set( SBE_DEBUG ${DEBUG} CACHE BOOL "" )

if(SBE_FOUND)
	include_directories(${SBE_INCLUDE_DIR})
else()
	message( FATAL_ERROR "!! SBE not found!")
endif()

if(SFGUI_FOUND)
	include_directories(${SFGUI_INCLUDE_DIR})
  # FIXME: make SBE export it's dependencies correctly or something
  set(SFGUI_LIBRARY sfgui)
else()
	message( FATAL_ERROR "!! SBE not found!")
endif()

# check for libraries

if(Boost_FOUND)
	if(CMAKE_COMPILER_IS_GNUCXX AND SUPPRESS_BOOST_HEADER_WARNINGS)
		# dont do this, GCC 6 chokes on that! (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=70129)
		#set(CMAKE_CXX_FLAGS "-isystem${Boost_INCLUDE_DIR}")
	endif()
	add_definitions( -DBOOST_THREAD_USE_LIB )
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIRS})
else()
	message( FATAL_ERROR "!! Boost not found!")
endif()

if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
  # FIXME: make SBE export its dependencies correctly or SFML set its libraries correctly or something
  set(SFML_LIBRARIES sfml-audio sfml-graphics sfml-window sfml-system)
else()
	message( FATAL_ERROR "!! SFML not found!\n   Please build SFML in '${CUSTOM_SFML_ROOT}/build' first!")
endif()

if(glm_FOUND)
  add_definitions( -DGLM_FORCE_RADIANS -DGLM_FORCE_CXX11 -DGLM_ENABLE_EXPERIMENTAL )
	include_directories(${GLM_INCLUDE_DIRS})
else()
	message( FATAL_ERROR "!! glm not found!" )
endif()

message(STATUS "-- INCLUDES --")
message(STATUS "sfml libs:      ${SFML_LIBRARIES}" )
message(STATUS "sfml includes:  ${SFML_INCLUDE_DIR}" )
message(STATUS "sfml deps:      ${SFML_DEPENDENCIES}" )
message(STATUS "sfgui lib:      ${SFGUI_LIBRARY}" )
message(STATUS "sfgui includes: ${SFGUI_INCLUDE_DIR}" )
message(STATUS "GLM includes    ${GLM_INCLUDE_DIRS}")
message(STATUS "SBE includes:   ${SBE_INCLUDE_DIR}")
message(STATUS "SBE libs:       ${SBE_LIBRARY}")
message(STATUS "boost includes: ${Boost_INCLUDE_DIR}\n\n" )


#######################################################
################### File inclusion ####################
#######################################################

set(SRCDIR ${PROJECT_SOURCE_DIR}/src )

file( GLOB_RECURSE SRCS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.cpp" )
file( GLOB_RECURSE HDRS RELATIVE "${PROJECT_SOURCE_DIR}" "${SRCDIR}/*.hpp" "${SRCDIR}/*.ipp" )


#######################################################
##################### Compiler Options ################
#######################################################

set(CMAKE_CXX_STANDARD 17)

include_directories( ${PROJECT_SOURCE_DIR}/src )

#needed on windows to find the libs sfml provides
if(CMAKE_SYSTEM_NAME MATCHES Windows)
  link_directories( ${CUSTOM_SFML_ROOT}/extlibs/libs-mingw/x86 )

  	# nogdi prevents a "#define ERROR 0" from wingdi.h which fucks up our code
  	# wingdi.h is sometimes included in gl.h ( some mingw versions of glew don't include it, most do )
  	add_definitions( -DNOGDI -DWIN32_LEAN_AND_MEAN )
endif()

if(NOT VERSION_CHECK_DONE)
	# set some definitions depending on gcc version
	add_definitions( -D__GCC_4_9__ )
	if(CMAKE_BUILD_TYPE MATCHES Debug)
		add_definitions( -Og )
	endif()
	set(VERSION_CHECK_DONE ON)
endif()

add_definitions( -pipe )

if(ENABLE_LTO)
	add_definitions( -flto )
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "System is ${CMAKE_SYSTEM_NAME}")
	#add_definitions( -fstack-protector --param=ssp-buffer-size=4 )
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
target_link_libraries( MaximumFish ${Boost_LIBRARIES} ${SBE_LIBRARY} ${SFML_LIBRARIES} ${SFGUI_LIBRARY}  ${SFML_DEPENDENCIES} Threads::Threads )

if( PROFILE )
	set_target_properties( MaximumFish PROPERTIES LINK_FLAGS "-O1 -pg" )
endif()
if( ENABLE_LTO )
	set_target_properties( MaximumFish PROPERTIES LINK_FLAGS "-flto" )
endif()
