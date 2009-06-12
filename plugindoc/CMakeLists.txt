ADD_EXECUTABLE(plugindoc pdocmain)
include_directories (${CD3_SOURCE_DIR}/cd3core)
link_directories($(CD3_BINARY_DIR)/cd3core/ ${CD3_SOURCE_DIR}/3rdparty/boost_1_38_0/stage/lib)
TARGET_LINK_LIBRARIES(plugindoc ${QT_LIBRARIES} cd3core boost_signals)
