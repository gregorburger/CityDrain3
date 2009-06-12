set(qs_HDRS qs/qswcalcunit.h qs/qswflow.h controller.h)
QT4_WRAP_CPP(moc_qs_SRCS ${qs_HDRS})

add_library(cd3core SHARED
calculationunit.cpp
flow.cpp
flowbuffer.cpp
flowfuns.cpp
mapbasedmodel.cpp
modelserializer.cpp
node.cpp
nodeconnection.cpp
noderegistry.cpp
saxloader.cpp
simulation.cpp
simulationregistry.cpp
stateserializer.cpp
controller.cpp
qs/qswflow.cpp 
qs/qswcalcunit.cpp
${moc_qs_SRCS})

add_definitions(-DQT_NO_KEYWORDS)
include_directories (${CD3_SOURCE_DIR}/cd3core)
include_directories (${CD3_SOURCE_DIR}/cd3core/qs)
include_directories (${CD3_SOURCE_DIR}/3rdparty/boost_1_38_0)
set(QT_USE_QTXML true)
set(QT_USE_QTSCRIPT true)
set(QT_NOTUSE_QTGUI true)
INCLUDE(${QT_USE_FILE})
TARGET_LINK_LIBRARIES(cd3core ${QT_LIBRARIES})
set(LIBRARY_OUTPUT_PATH ${CD3_SOURCE_DIR})
