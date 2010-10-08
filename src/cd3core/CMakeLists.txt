file(GLOB_RECURSE CPP_FILES ./ *.cpp)

message(STATUS ${CMAKE_BINARY_DIR})
include_directories(${PYTHON_INCLUDE_DIR} ${CMAKE_BINARY_DIR})
set_source_files_properties(${CMAKE_BINARY_DIR}/src/python-swig/pycd3WRAP.cpp PROPERTIES GENERATED 1)
if (WITH_PYTHON)
	set(CPP_FILES ${CPP_FILES} ${CMAKE_BINARY_DIR}/src/python-swig/pycd3WRAP.cpp)
endif (WITH_PYTHON)
add_library(cd3core SHARED ${CPP_FILES})

TARGET_LINK_LIBRARIES(cd3core ${QT_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_SIGNALS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

add_dependencies(cd3core pycd3gen)

if(WITH_PYTHON)
	target_link_libraries(cd3core ${PYTHON_LIBRARIES})
endif(WITH_PYTHON)

INSTALL(TARGETS cd3core
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib)
