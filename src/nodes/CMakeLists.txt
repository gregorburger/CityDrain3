set(qs_HDRS qs/qswnode.h)
QT4_WRAP_CPP(moc_qs_SRCS ${qs_HDRS})

file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(nodes MODULE 
${CPP_FILES}
${moc_qs_SRCS}
)

IF (MINGW)
	SET_TARGET_PROPERTIES(nodes PROPERTIES PREFIX "")
ENDIF()

target_link_libraries(nodes ${QT_LIBRARIES} ${Boost_LIBRARIES} cd3core)

INSTALL(TARGETS nodes
  LIBRARY DESTINATION lib
)
