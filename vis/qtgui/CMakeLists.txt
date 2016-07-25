# $Id: CMakeLists.txt 897 2015-04-16 13:02:30Z klugeflo $

set(qtgui_SOURCES
	defaulttaskstatewidget.cpp
	displayarea.cpp
	displaydata.cpp
	displaydimension.cpp
	displaylegend.cpp
	displaymodel.cpp
	displaywidget.cpp
	mktaskstatewidget.cpp
	taskstatewidget.cpp
	taskstatewidgetfactory.cpp
	viswindow.cpp
)

set(qtgui_HEADER displayarea.h displaylegend.h displaywidget.h viswindow.h)
QT4_WRAP_CPP(qtgui_HEADER_MOC ${qtgui_HEADER})

add_library(qtgui OBJECT ${qtgui_SOURCES} ${qtgui_HEADER_MOC})
