include(${SWIG_USE_FILE})
execute_process(COMMAND ${SWIG_EXECUTABLE} -python -external-runtime swigruntime.h)

#execute_process(COMMAND ${SWIG_EXECUTABLE} -python -c++ -o pycd3WRAP.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pycd3.i
#	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


add_custom_target(pycd3gen ALL ${SWIG_EXECUTABLE} -python -c++ -o pycd3WRAP.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pycd3.i
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pycd3.i)
