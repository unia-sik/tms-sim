# $Id: CMakeLists.txt 803 2014-12-11 17:05:37Z klugeflo $

# improved building of documentation following
# http://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

# to build the documentation, call cmake with -DBUILD_DOCUMENTATION=on

option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)
if(BUILD_DOCUMENTATION)

  # add a target to generate API documentation with Doxygen
  find_package(Doxygen)

  if(NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
	    "Doxygen is needed to build the documentation.")
  endif(NOT DOXYGEN_FOUND)
#endif(DOXYGEN_FOUND)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		 ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
  		    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		    COMMENT "Generating API documentation with Doxygen" VERBATIM
		    )
  # removed doc from all -- then however we also cannot install
  #install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION share/doc/tms-sim)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION ${DOC_INSTALL_DIR})

endif(BUILD_DOCUMENTATION)
