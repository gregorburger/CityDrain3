cmake_minimum_required (VERSION 2.6)
project(cd3)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	message(STATUS "Using Visual C compilers")
	set(CMAKE_CXX_FLAGS "/EHsc")
endif()
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	message(STATUS "Using GNU compilers")
	set(CMAKE_CXX_FLAGS "-g -Wall")
endif()


#check for Qt4 which is a definitive requirement for building cd3
find_package(Qt4 REQUIRED)
set(QT_USE_QTXML true)
set(QT_USE_QTGUI true)
set(QT_USE_QTSVG true)
add_definitions(-DQT_NO_KEYWORDS)
INCLUDE(${QT_USE_FILE})
set(LIBRARY_OUTPUT_PATH ${cd3_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${cd3_BINARY_DIR})

set(CD3_FRONTEND_LIBS cd3core)
set(BOOST_COMPONENTS system signals program_options thread date_time filesystem)


include_directories(src/cd3core)

#include python scripting capabilites
option(WITH_PYTHON "Include Python scripting capabilites" ON)
if (WITH_PYTHON)
	find_package(PythonLibs REQUIRED)
	find_package(SWIG REQUIRED)
endif (WITH_PYTHON)
if (WITH_PYTHON AND PYTHONLIBS_FOUND AND SWIG_FOUND)
	message(STATUS "Python support enabled")
	include_directories(${PYTHON_INCLUDE_DIRS})
else()
	add_definitions(-DPYTHON_DISABLED)
	message(STATUS "Python support disabled")
endif()

#check for boost which is a definitive requirement for building cd3
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.38.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
set(Boost_USE_MULTITHREADED ON)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#check for openmp. also disables the code using qt thread
option(WITH_PARALLEL "Enable parallel code. Needs a OpenMP compiler" ON)
find_package(OpenMP QUIET)
if (WITH_PARALLEL AND OPENMP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	message(STATUS "OpenMP is enabled")
else()
	add_definitions(-DOPENMP_DISABLED)
	message(STATUS "OpenMP is disabled. Consider using a compiler that supports OpenMP")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
	message(STATUS Debug)
	add_definitions(-DDEBUG)
else ()
	message(STATUS Release)
endif()


##'NOF CHECKING... BUILDING##
add_subdirectory(src/cd3core)
add_subdirectory(src/nodes)
add_subdirectory(src/dance4water)

if (WITH_PYTHON)
		add_subdirectory(src/python-swig)
endif()

add_subdirectory(src/app)
add_subdirectory(src/gui)

option(WITH_PLUGINDOC "Build the plugindoc app" OFF)
if (WITH_PLUGINDOC)
	add_subdirectory(src/plugindoc)
endif()

option(WITH_TESTS "Enable unit testing" OFF)

if (WITH_TESTS)
	find_package(GTest REQUIRED)
	message(STATUS "Tests enabled")
	enable_testing(true)
	add_subdirectory(src/tests)
endif()

INCLUDE(Cd3CPack.txt)
