# $Id: CMakeLists.txt 1428 2016-06-28 13:53:19Z klugeflo $

cmake_minimum_required(VERSION 2.8)
project(tms-sim)

set(CMAKE_BUILD_TYPE Debug)

# gcc-4.8 is needed only for address sanitizer
#set(CMAKE_CXX_COMPILER g++-4.8.3)
#set(CMAKE_CC_COMPILER gcc-4.8.3)

#add_definitions(-std=gnu++11 -static-libasan -fno-omit-frame-pointer -fsanitize=address)
# -fsanitize=address
# needs gcc-4.8
# for better output see http://tsdgeos.blogspot.de/2014/03/asan-and-gcc-how-to-get-line-numbers-in.html
# don't use add_definitions, this only goes into the actual CXXFLAGS,
# but we need at least some of these also in the LDFLAGS
# see: http://www.cmake.org/pipermail/cmake/2010-September/039843.html
set(CMAKE_CXX_FLAGS "-g3 -std=gnu++11 -fPIC -Wall -Wextra")
#set(CMAKE_CXX_FLAGS "-g3 -std=gnu++11 -fPIC -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address -static-libasan")

add_definitions(-DTLOGLEVEL=TLL_WARN)

# -fno-omit-frame-pointer -fsanitize=address

message(STATUS "Binary dir: ${CMAKE_BINARY_DIR}")
message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")

set(BIN_INSTALL_DIR "bin")
set(LIB_INSTALL_DIR "lib") # currently not used
set(INCLUDE_BASE_DIR "include")
set(INCLUDE_INSTALL_DIR "${INCLUDE_BASE_DIR}/${PROJECT_NAME}")
set(SHARE_INSTALL_DIR "share")
set(DATA_INSTALL_DIR "${SHARE_INSTALL_DIR}/${PROJECT_NAME}")
set(DOC_INSTALL_DIR "${SHARE_INSTALL_DIR}/doc/${PROJECT_NAME}")
set(XSD_INSTALL_DIR "${DATA_INSTALL_DIR}/xsd")
set(XML_INSTALL_DIR "${DATA_INSTALL_DIR}/xml")
set(CFG_INSTALL_DIR "${DATA_INSTALL_DIR}/cfg")

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
  message(STATUS "install prefix not defined, using ./")
  set(CMAKE_INSTALL_PREFIX ./)
else (NOT DEFINED CMAKE_INSTALL_PREFIX)
  message(STATUS "install prefix defined")
endif (NOT DEFINED CMAKE_INSTALL_PREFIX)
message(STATUS "Installing to " ${CMAKE_INSTALL_PREFIX})

add_definitions("-DXSD_INSTALL_DIR=\"${CMAKE_INSTALL_PREFIX}/${XSD_INSTALL_DIR}\"")



set(CMAKE_INCLUDE_CURRENT_DIR ON)
#include_directories(include)
include_directories ("${PROJECT_SOURCE_DIR}/src")

find_package(LibXml2 REQUIRED)

FIND_PACKAGE ( Threads REQUIRED )
find_package(Boost REQUIRED COMPONENTS program_options)

include_directories(${LIBXML2_INCLUDE_DIR})

add_subdirectory(cfg)
#add_subdirectory(include)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(tasksets)
#add_subdirectory(vis)

# visualisation
if (NOT DEFINED BUILD_GUI)
  message(STATUS "GUI will not be built (Specify -DBUILD_GUI=1 if you want to)")
else (NOT DEFINED BUILD_GUI)
  message(STATUS "BUILD_GUI is set (${BUILD_GUI}), building with Qt4 GUI")
find_package(Qt4)
if (Qt4_FOUND)
  message(STATUS "Qt4 found, will build visualisation")
  message(STATUS "Please keep in mind that the visualisation is still being developed and may possibly not work as expected!")
  add_subdirectory(vis)
else (Qt4_FOUND)
  message(STATUS "No Qt4 found, visualisation will not be built")
endif (Qt4_FOUND)
endif (NOT DEFINED BUILD_GUI)
