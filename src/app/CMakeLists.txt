add_executable(cd3 main.cpp)
message(STATUS ${PYTHON_LIBRARIES})
target_link_libraries(cd3 ${QT_LIBRARIES} ${BOOST_LIBRARIES} ${PYTHON_LIBRARIES} cd3core)
INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
