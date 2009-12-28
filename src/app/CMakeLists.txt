add_executable(cd3 main.cpp saxloader.cpp)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(cd3 cd3core)
else()
    target_link_libraries(cd3 cd3core pycd3 ${Boost_PROGRAM_OPTIONS_LIBRARY})
endif()
INSTALL(TARGETS cd3
  RUNTIME DESTINATION bin
)
