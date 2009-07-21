cmake_minimum_required (VERSION 2.6)
project(CD3)

INCLUDE_DIRECTORIES(src/cd3core src/cd3core/qs)
find_package(Qt4 REQUIRED)

if (NOT BOOST_ROOT AND MINGW)
	set(BOOST_ROOT 3rdparty/boost_1_39_0 CACHE PATH "")
	message(STATUS "using default boost in " ${BOOST_ROOT})
endif()

find_package(Boost 1.38.0 COMPONENTS signals program_options thread REQUIRED)

if (Boost_FOUND) 
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_MULTITHREADED ON)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS})

if (CMAKE_BUILD_TYPE STREQUAL Debug) 
	message(STATUS Debug)
	add_definitions(-DDEBUG)
else ()
	message(STATUS Release)
endif()

message(STATUS ${CMAKE_CXX_COMPILER_ID})
set(QT_USE_QTXML true)
set(QT_USE_QTSCRIPT true)
set(QT_NOTUSE_QTGUI true)
add_definitions(-DQT_NO_KEYWORDS)
INCLUDE(${QT_USE_FILE})
set(LIBRARY_OUTPUT_PATH ${CD3_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CD3_BINARY_DIR})


add_subdirectory(src/cd3core)
add_subdirectory(src/nodes)
add_subdirectory(src/app)
add_subdirectory(src/plugindoc)
