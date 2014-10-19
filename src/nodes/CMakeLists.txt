file(GLOB_RECURSE CPP_FILES ./ *.cpp *.h)
set(CPP_FILES ${CPP_FILES} rumba01dll.c)


add_subdirectory(muparser)
include_directories(muparser)
add_library(nodes SHARED ${CPP_FILES})

IF (MINGW)
	SET_TARGET_PROPERTIES(nodes PROPERTIES PREFIX "")
endif()
if (${CMAKE_HOST_WIN32})
	target_link_libraries(nodes cd3core)
else()
	target_link_libraries(nodes cd3core ${Boost_THREAD_LIBRARY})
endif()


INSTALL(TARGETS nodes
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
