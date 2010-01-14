file(GLOB GUI_CPP *.cpp)
file(GLOB GUI_UI *.ui)

QT4_WRAP_UI(GUI_CPP ${GUI_UI})
message(STATUS ${GUI_CPP})
QT4_WRAP_CPP(GUI_CPP mainwindow.h nodeitem.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_executable(cd3-gui ${GUI_CPP})
target_link_libraries(cd3-gui cd3core pycd3 ${QT_QTGUI_LIBRARY})
