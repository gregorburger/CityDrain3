include_directories(${GTEST_INCLUDE_DIRS} ../cd3core/)

add_executable(typeconverter typeconverter.cpp)
target_link_libraries(typeconverter ${GTEST_BOTH_LIBRARIES} cd3core)

add_test(AllTestsInTypeconverter typeconverter)

GTEST_ADD_TESTS(typeconverter ARGN typeconverter.cpp)
