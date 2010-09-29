include(${SWIG_USE_FILE})
execute_process(COMMAND ${SWIG_EXECUTABLE} -python -external-runtime swigruntime.h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_INCLUDE_DIRS})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(pycd3.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(pycd3.i PROPERTIES SWIG_FLAGS "")

SWIG_ADD_MODULE(pycd3 python pycd3.i)
SWIG_LINK_LIBRARIES(pycd3 cd3core ${PYTHON_LIBRARIES})

#file(GLOB SRCS *.cpp)

#add_executable(proto main.cpp)
#target_link_libraries(proto core ${PYTHON_LIBRARIES})

#add_library(core SHARED cd3node.cpp noderegistry.cpp)
