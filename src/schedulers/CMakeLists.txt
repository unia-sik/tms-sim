# $Id: CMakeLists.txt 1421 2016-06-22 07:46:32Z klugeflo $

set(schedulers_SOURCES
	ald.cpp
	beedf.cpp
	dbpedf.cpp
	dummy.cpp
	dvdedf.cpp
	edf.cpp
	fpp.cpp
	fppnat.cpp
	gdpa.cpp
	gdpas.cpp
	gmuamk.cpp
	hcedf.cpp
	mkuedf.cpp
	phcedf.cpp
	oedf.cpp
	oedfmax.cpp
	oedfmin.cpp
	)

#add_library(schedulers SHARED ${schedulers_SOURCES})
add_library(schedulers OBJECT ${schedulers_SOURCES})

#install(TARGETS schedulers DESTINATION ${LIBDIR})

#file(GLOB schedulers_HEADERS "*.h")
#install(FILES ${schedulers_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/schedulers)
