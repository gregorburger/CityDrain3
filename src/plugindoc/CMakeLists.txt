ADD_EXECUTABLE(plugindoc pdocmain)

if (Boost_FOUND)
	target_link_libraries(plugindoc ${QT_LIBRARIES} ${Boost_LIBRARIES} cd3core)
else()
	TARGET_LINK_LIBRARIES(plugindoc ${QT_LIBRARIES} cd3core)
endif()
