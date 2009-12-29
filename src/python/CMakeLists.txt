file(GLOB_RECURSE CPP_FILES ./ *.cpp)
add_library(pycd3 SHARED ${CPP_FILES})
include_directories(${PYTHON_INCLUDE_PATH})
#message(STATUS  ${Python_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(pycd3 ${PYTHON_LIBRARIES} cd3core ${Boost_PYTHON_LIBRARY})

if(WIN32)
	set_target_properties(pycd3}
	PROPERTIES
	OUTPUT_NAME "pycd3"
	PREFIX ""
	SUFFIX .pyd
	IMPORT_SUFFIX .pyd
)
else()
	set_target_properties(pycd3
	PROPERTIES
	OUTPUT_NAME "pycd3"
	PREFIX ""
)
endif()

#INSTALL(TARGETS pycd3
#  LIBRARY DESTINATION lib
#)
