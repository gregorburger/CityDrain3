add_executable(cd3 main.cpp)
target_link_libraries(cd3 cd3core)
if (Boost_FOUND)
	target_link_libraries(cd3 ${QT_LIBRARIES} ${BOOST_LIBRARIES})
else()
	target_link_libraries(cd3 ${QT_LIBRARIES} boost_signals boost_thread-mt boost_program_options)
endif()

INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
