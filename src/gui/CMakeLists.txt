file(GLOB GUI_CPP *.cpp)
file(GLOB GUI_UI *.ui)

QT4_WRAP_UI(GUI_CPP ${GUI_UI})
set(MOC_HEADERS mainwindow.h nodeitem.h simulationscene.h portitem.h simulationsaver.h timestephandler.h newsimulationdialog.h)
QT4_WRAP_CPP(GUI_CPP ${MOC_HEADERS})
QT4_ADD_RESOURCES(GUI_CPP ../../data/resources/cd3-gui.qrc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(cd3-gui ${GUI_CPP} ../app/saxloader.cpp)
target_link_libraries(cd3-gui cd3core pycd3 ${QT_QTGUI_LIBRARY})
