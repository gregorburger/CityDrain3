add_executable(cd3 main.cpp saxloader.cpp)
target_link_libraries(cd3 pycd3)
INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
