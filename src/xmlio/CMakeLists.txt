# $Id: CMakeLists.txt 1422 2016-06-22 08:59:13Z klugeflo $

set(xmlio_SOURCES
#	dbptaskbuilder.cpp
	taskfactory.cpp
	tasksetreader.cpp
	tasksetwriter.cpp
	utilityaggregatorfactory.cpp
	utilitycalculatorfactory.cpp
	xmlutils.cpp
	mktaskbuilder.cpp
	periodictaskbuilder.cpp
	pconsumertaskbuilder.cpp
	pproducertaskbuilder.cpp
	sporadictaskbuilder.cpp
	sptaskbuilder.cpp
	uaexpbuilder.cpp
	uameanbuilder.cpp
	uawmeanbuilder.cpp
	uawmkbuilder.cpp
	ucfirmrtbuilder.cpp
	)

#add_library(xmlio SHARED ${xmlio_SOURCES})
add_library(xmlio OBJECT ${xmlio_SOURCES})

#install(TARGETS xmlio DESTINATION ${LIBDIR})

#file(GLOB xmlio_HEADERS "*.h")
#install(FILES ${xmlio_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/xmlio)
