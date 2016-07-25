# $Id: CMakeLists.txt 1355 2016-02-17 14:43:14Z klugeflo $

set(taskmodels_SOURCES
	dbptask.cpp
	mkptask.cpp
	mkmonitor.cpp
	mktask.cpp
	periodictask.cpp
	sporadictask.cpp
	sptask.cpp
	)

#add_library(taskmodels SHARED ${taskmodels_SOURCES})
add_library(taskmodels OBJECT ${taskmodels_SOURCES})

#install(TARGETS taskmodels DESTINATION ${LIBDIR})

#file(GLOB taskmodels_HEADERS "*.h")
#install(FILES ${taskmodels_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/taskmodels)
