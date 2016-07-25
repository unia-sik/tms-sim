# $Id: CMakeLists.txt 1421 2016-06-22 07:46:32Z klugeflo $

set(utils_SOURCES
	bitmap.cpp
	bitstrings.cpp
	globalconfig.cpp
	kvfile.cpp
	logger.cpp
	nullstream.cpp
	random.cpp
	tlogger.cpp
	tmsexception.cpp
	tmsmath.cpp
	)

#add_library(utils SHARED ${utils_SOURCES})
add_library(utils OBJECT ${utils_SOURCES})

#install(TARGETS utils DESTINATION ${LIBDIR})

#file(GLOB utils_HEADERS "*.h")
#install(FILES ${utils_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/utils)

add_executable(genseeds genseeds.cpp)
target_link_libraries(genseeds
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS genseeds DESTINATION ${BIN_INSTALL_DIR})
