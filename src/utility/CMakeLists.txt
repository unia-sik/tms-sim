# $Id: CMakeLists.txt 1407 2016-05-31 07:30:51Z klugeflo $

set(utility_SOURCES
	uaexp.cpp
	uamean.cpp
	uanone.cpp
	uawindow.cpp
	uawmean.cpp
	uawmk.cpp
	ucfirm.cpp
	ucnone.cpp
	)

#add_library(utility SHARED ${utility_SOURCES})
add_library(utility OBJECT ${utility_SOURCES})

#install(TARGETS utility DESTINATION ${LIBDIR})

#file(GLOB utility_HEADERS "*.h")
#install(FILES ${utility_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/utility)
