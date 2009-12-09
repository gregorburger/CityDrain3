add_executable(cd3 main.cpp saxloader.cpp)
message(STATUS ${PYTHON_LIBRARIES})
target_link_libraries(cd3 ${QT_LIBRARIES} ${BOOST_LIBRARIES} ${PYTHON_LIBRARIES} cd3core pycd3)
INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
