# $Id: CMakeLists.txt 1355 2016-02-17 14:43:14Z klugeflo $

file(GLOB XSD "*.xsd")
file(GLOB XML "*.xml")

install(FILES ${XSD} DESTINATION ${XSD_INSTALL_DIR})
# temporarily disable installation of example task sets
#install(FILES ${XML} DESTINATION ${XML_INSTALL_DIR})


#  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION ${DOCDIR})
