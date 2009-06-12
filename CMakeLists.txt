cmake_minimum_required (VERSION 2.6)
project(CD3)

INCLUDE_DIRECTORIES(cd3core cd3core/qs)
find_package(Qt4 REQUIRED)
 
add_subdirectory(cd3core)
add_subdirectory(nodes)
add_subdirectory(app)
ADD_SUBDIRECTORY(plugindoc)
