include_directories(../nodes)
file(GLOB_RECURSE CPP_FILES ./ *.cpp *.h)

set(CPP_FILES ${CPP_FILES})

add_library(cd3dance4water SHARED ${CPP_FILES})

IF (MINGW)
        SET_TARGET_PROPERTIES(cd3dance4water PROPERTIES PREFIX "")
endif()
#if (${CMAKE_HOST_WIN32})
#    target_link_libraries(nodes cd3core ${Boost_THREAD_LIBRARY})
#else()
    target_link_libraries(cd3dance4water cd3core nodes)
#endif()


INSTALL(TARGETS cd3dance4water
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
