set(qs_HDRS qs/qswcalcunit.h qs/qswflow.h controller.h)
QT4_WRAP_CPP(moc_qs_SRCS ${qs_HDRS})

file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(cd3core SHARED
${CPP_FILES}
${moc_qs_SRCS})

TARGET_LINK_LIBRARIES(cd3core ${QT_LIBRARIES} ${Boost_LIBRARIES})
set(LIBRARY_OUTPUT_PATH ${CD3_SOURCE_DIR})
