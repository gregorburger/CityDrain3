add_executable(cd3 main.cpp saxloader.cpp)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(cd3 pycd3)
else()
    target_link_libraries(cd3 pycd3 ${Boost_PROGRAM_OPTIONS_LIBRARY})
endif()
INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
