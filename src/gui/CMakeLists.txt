file(GLOB GUI_CPP *.cpp *.h)
file(GLOB COMMAND_CPP  commands/*.cpp commands/*.h)
file(GLOB GUI_UI *.ui)

QT4_WRAP_UI(GUI_CPP ${GUI_UI})
set(MOC_HEADERS mainwindow.h nodeitem.h simulationscene.h portitem.h
				simulationsaver.h timestephandler.h newsimulationdialog.h
				stringparameteredit.h guilogsink.h findnodedialog.h
				connectionitem.h renamenodedialog.h saveasdialog.h)

QT4_WRAP_CPP(GUI_CPP ${MOC_HEADERS})
QT4_ADD_RESOURCES(GUI_CPP ../../data/resources/cd3-gui.qrc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

if (WITH_AUTOLAYOUT)
add_definitions(-DWITH_AUTOLAYOUT)
endif (WITH_AUTOLAYOUT)

add_executable(cd3-gui WIN32 ${GUI_CPP} ${COMMAND_CPP} ../app/saxloader.cpp)
target_link_libraries(cd3-gui ${BOOST_SYSTEM_LIBRARY} ${CD3_FRONTEND_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTSVG_LIBRARY} ${QT_QTMAIN_LIBRARY})

if (WITH_AUTOLAYOUT)
target_link_libraries(cd3-gui ${GRAPHVIZ_LIBRARIES})
endif (WITH_AUTOLAYOUT)


INSTALL(TARGETS cd3-gui RUNTIME DESTINATION bin)
