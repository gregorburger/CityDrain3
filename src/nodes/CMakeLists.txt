file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(nodes SHARED ${CPP_FILES})

IF (MINGW)
	SET_TARGET_PROPERTIES(nodes PROPERTIES PREFIX "")
ENDIF()
#message(STATUS ${Boost_LIBRARIES})
target_link_libraries(nodes cd3core)

#INSTALL(TARGETS nodes
#  LIBRARY DESTINATION lib
#)
