# $Id: CMakeLists.txt 1432 2016-07-15 10:43:04Z klugeflo $

set(mkeval_lib_SOURCES
	abstractmktaskset.cpp
	concretemktaskset.cpp
	dummymkcts.cpp
	fixedtimesimulation.cpp
	gmperiodgenerator.cpp
	gstsimulation.cpp
	intervalperiodgenerator.cpp
        mkallocators.cpp
	mkeval.cpp
	mkgenerator.cpp
	mkglobals.cpp
	mkdsesimulationset.cpp
	mkpsimulation.cpp
	mksimulation.cpp
	periodgenerator.cpp
	utilisationstatistics.cpp
        )
add_library(mkeval_lib OBJECT ${mkeval_lib_SOURCES})


add_executable(mkeval eval.cpp)# ${mkeval_SOURCES})
target_link_libraries(mkeval
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkeval DESTINATION ${BIN_INSTALL_DIR})


set(mkrun_SOURCES
	mkrun.cpp
	)
add_executable(mkrun ${mkrun_SOURCES})
target_link_libraries(mkrun
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkrun DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkwrite mkwrite.cpp)
target_link_libraries(mkwrite
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkwrite DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkrun-xml mkrun-xml.cpp )
target_link_libraries(mkrun-xml
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkrun-xml DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkextwrite extwrite.cpp )
target_link_libraries(mkextwrite
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkextwrite DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkexcrun excrun.cpp )
target_link_libraries(mkexcrun
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkexcrun DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkbdexcsearch bdexcsearch.cpp )
target_link_libraries(mkbdexcsearch
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkbdexcsearch DESTINATION ${BIN_INSTALL_DIR})


add_executable(mktsprint tsprint.cpp )
target_link_libraries(mktsprint
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mktsprint DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkcs-check mkcs-check.cpp )
target_link_libraries(mkcs-check
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkcs-check DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkcexcrun cexcrun.cpp )
target_link_libraries(mkcexcrun
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkcexcrun DESTINATION ${BIN_INSTALL_DIR})


add_executable(mkdse mkdse.cpp )
target_link_libraries(mkdse
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS mkdse DESTINATION ${BIN_INSTALL_DIR})

add_executable(prseedsearch prseedsearch.cpp )
target_link_libraries(prseedsearch
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	${Boost_LIBRARIES}
	)
install(TARGETS prseedsearch DESTINATION ${BIN_INSTALL_DIR})
