# $Id: CMakeLists.txt 1355 2016-02-17 14:43:14Z klugeflo $

set(core_SOURCES
	deadlinemonitor.cpp
	job.cpp
	scconfig.cpp
	scheduler.cpp
	simulation.cpp
	stat.cpp
	statistics.cpp
	task.cpp
	utilityaggregator.cpp
	utilitycalculator.cpp
	writeabletoxml.cpp
	)

#add_library(core SHARED ${core_SOURCES})
add_library(core OBJECT ${core_SOURCES})

#install(TARGETS core DESTINATION ${LIBDIR})


#file(GLOB core_HEADERS "*.h")
#install(FILES ${core_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/core)
