file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(nodes SHARED ${CPP_FILES})

IF (MINGW)
	SET_TARGET_PROPERTIES(nodes PROPERTIES PREFIX "")
endif()
if (${CMAKE_CXX_COMPILER} STREQUAL MSVC)
    target_link_libraries(nodes cd3core)
else()
    target_link_libraries(nodes cd3core ${Boost_THREAD_LIBRARY})
endif()


#INSTALL(TARGETS nodes
#  LIBRARY DESTINATION lib
#)
