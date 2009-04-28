set(qs_HDRS qs/qswcalcunit.h qs/qswflow.h qs/qswnode.h)
QT4_WRAP_CPP(moc_qs_SRCS ${qs_HDRS})

add_library(nodes MODULE 
catchmentcss.cpp
constsource.cpp
cso.cpp
defaultsimulation.cpp
fileout.cpp
flowtypefactory.cpp
mixer.cpp
nodes.cpp
null.cpp
orderedpipesimulation.cpp
parallelsimulation.cpp
pipelinedsimulation.cpp
rainread.cpp
sewer.cpp
splitter.cpp
testnode.cpp
vardtsimulation.cpp
qs/qswflow.cpp 
qs/qswcalcunit.cpp 
qs/qswnode.cpp 
qs/statemigrator.cpp
${moc_qs_SRCS}
)

add_definitions(-DQT_NO_KEYWORDS)
include_directories (${CD3_SOURCE_DIR}/cd3core)
include_directories (${CD3_SOURCE_DIR}/3rdparty/boost_1_38_0)
set(QT_USE_QTSCRIPT true)
set(QT_USE_QTXML true)
set(QT_NOTUSE_QTGUI true)
INCLUDE(${QT_USE_FILE})
link_directories($(CD3_BINARY_DIR)/cd3core/)
TARGET_LINK_LIBRARIES(nodes ${QT_LIBRARIES} cd3core)
set(LIBRARY_OUTPUT_PATH ${CD3_SOURCE_DIR})
