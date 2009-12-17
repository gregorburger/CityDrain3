file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(nodes MODULE ${CPP_FILES})

IF (MINGW)
	SET_TARGET_PROPERTIES(nodes PROPERTIES PREFIX "")
ENDIF()

target_link_libraries(nodes ${QT_LIBRARIES} ${Boost_LIBRARIES} cd3core)

INSTALL(TARGETS nodes
  LIBRARY DESTINATION lib
)
