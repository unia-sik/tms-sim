# $Id: CMakeLists.txt 1407 2016-05-31 07:30:51Z klugeflo $

add_subdirectory(core)
add_subdirectory(schedulers)
add_subdirectory(taskmodels)
add_subdirectory(utility)
add_subdirectory(utils)
add_subdirectory(xmlio)

add_subdirectory(tms-sim)
#add_subdirectory(generator)
add_subdirectory(mkeval)
add_subdirectory(tseval)

add_subdirectory(tests)

set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-whole-archive")

# build static and shared libraries with same name:
# http://datainfer.wordpress.com/2013/10/24/make-both-static-and-shared-libraries-in-one-build-with-cmake/

add_library(tms STATIC $<TARGET_OBJECTS:core>
		       $<TARGET_OBJECTS:schedulers>
		       $<TARGET_OBJECTS:taskmodels>
		       $<TARGET_OBJECTS:utility>
		       $<TARGET_OBJECTS:utils>
		       $<TARGET_OBJECTS:xmlio>
		       #$<TARGET_OBJECTS:mkgenerator>
		       $<TARGET_OBJECTS:mkeval_lib>
                       $<TARGET_OBJECTS:tseval_lib>
		       )

# add_library(tms SHARED $<TARGET_OBJECTS:core>
# 		$<TARGET_OBJECTS:schedulers>
# 		$<TARGET_OBJECTS:taskmodels>
# 		$<TARGET_OBJECTS:utility>
# 		$<TARGET_OBJECTS:utils>
# 		$<TARGET_OBJECTS:xmlio>
# 		$<TARGET_OBJECTS:mkgenerator>
# 		)

#install(TARGETS tms LIBRARY DESTINATION ${LIB_INSTALL_DIR})
#install(TARGETS tms ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
