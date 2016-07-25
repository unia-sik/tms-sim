# $Id: CMakeLists.txt 891 2015-04-14 14:32:00Z klugeflo $
# Build Qt visualisation

find_package(Qt4 REQUIRED)
find_package(Boost REQUIRED COMPONENTS regex)

#SET(tv_SRC vis.cpp displayarea.cpp displaydata.cpp
#displaydimension.cpp displaylegend.cpp displaymodel.cpp
#displaywidget.cpp logfilereader.cpp jobinfo.cpp taskstatedecoder.cpp
#viswindow.cpp)
SET(tv_HDR displayarea.h displaylegend.h displaywidget.h viswindow.h)
 
INCLUDE(${QT_USE_FILE})
 
#QT4_WRAP_UI(UISrcs MyWidget.ui)
QT4_WRAP_CPP(tv_HDR_MOC ${tv_HDR})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CMAKE_CURRENT_BINARY_DIR}
		    ${Boost_INCLUDE_DIRS})


# message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
# if (${Boost_regex_FOUND})
#   message(STATUS "Regex found")
# else (${Boost_regex_FOUND})
#   message(STATUS "Regex not found")
# endif (${Boost_regex_FOUND})
# message(STATUS "Boost regex found: ${Boost_regex_FOUND}")
# message(STATUS "Boost regex library: ${Boost_regex_LIBRARY}")


add_subdirectory(model)
add_subdirectory(qtgui)

add_library(qtguilib STATIC $<TARGET_OBJECTS:model>
		     $<TARGET_OBJECTS:qtgui>
		     )


#ADD_LIBRARY(tv ${TV_SRC})
#ADD_EXECUTABLE(tms-vis ${tv_SRC} ${tv_HDR_MOC})
ADD_EXECUTABLE(tms-vis qtgui/vis.cpp)
#TARGET_LINK_LIBRARIES(tms-vis ${QT_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(tms-vis qtguilib ${QT_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS tms-vis DESTINATION ${BIN_INSTALL_DIR})
