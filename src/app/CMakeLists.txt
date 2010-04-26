add_executable(cd3 main.cpp saxloader.cpp)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	target_link_libraries(cd3 ${CD3_FRONTEND_LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY})
else()
	target_link_libraries(cd3 ${CD3_FRONTEND_LIBS})
endif()

INSTALL(TARGETS cd3 RUNTIME DESTINATION bin)
