ADD_EXECUTABLE(plugindoc pdocmain)

target_link_libraries(plugindoc ${QT_LIBRARIES} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} cd3core)

