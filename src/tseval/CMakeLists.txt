# $Id: CMakeLists.txt 904 2015-04-22 13:50:04Z klugeflo $

set(tseval_lib_SOURCES
	dependencymatrix.cpp
	pconsumertask.cpp
	periodgenerator.cpp
	pproducertask.cpp
	tmppconsumertask.cpp
	tstaskset.cpp
	)

add_library(tseval_lib OBJECT ${tseval_lib_SOURCES})

set(tseval_SOURCES
	tseval.cpp
	)

add_executable(tseval ${tseval_SOURCES})
target_link_libraries(tseval
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	)

set(tswrite_SOURCES
	tswrite.cpp
	)
add_executable(tswrite ${tswrite_SOURCES})
target_link_libraries(tswrite
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	)

set(tsrun_SOURCES
	tsrun.cpp
	)
add_executable(tsrun ${tsrun_SOURCES})
target_link_libraries(tsrun
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	)


install(TARGETS tseval tswrite tsrun DESTINATION ${BIN_INSTALL_DIR})


# set(runts_SOURCES
# 	mkeval.cpp
# 	runts.cpp
# 	)

# add_executable(runts ${runts_SOURCES})

# target_link_libraries(runts
# 	#mkgenerator schedulers xmlio taskmodels utility core utils
# 	tms
# 	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
# 	)

# install(TARGETS runts DESTINATION ${BIN_INSTALL_DIR})
