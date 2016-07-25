# $Id: CMakeLists.txt 1058 2015-10-29 15:19:40Z klugeflo $

set(tms-sim_SOURCES
        tms-sim.cpp
        )

add_executable(tms-sim ${tms-sim_SOURCES})

target_link_libraries(tms-sim
	#schedulers xmlio taskmodels utility core utils
	tms
	${LIBXML2_LIBRARIES}
	${Boost_LIBRARIES}
	)

install(TARGETS tms-sim DESTINATION ${BIN_INSTALL_DIR})
