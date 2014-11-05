# -*- mode: cmake; -*-
project(libmsgwebsocket)
cmake_minimum_required(VERSION 2.6.2 FATAL_ERROR)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

set(LIBMSGWEBSOCKET_MAJOR_VERSION 0)
set(LIBMSGWEBSOCKET_MINOR_VERSION 1)
set(LIBMSGWEBSOCKET_SUB_VERSION   0)
set(VERSION_SEP "-")
set(LIBMSGWEBSOCKET_SHORT_VERSION "${LIBMSGWEBSOCKET_MAJOR_VERSION}${VERSION_SEP}${LIBMSGWEBSOCKET_MINOR_VERSION}")
set(LIBMSGWEBSOCKET_VERSION "${LIBMSGWEBSOCKET_SHORT_VERSION}${VERSION_SEP}${LIBMSGWEBSOCKET_SUB_VERSION}")
set(LIBMSGWEBSOCKET_RPM_VERSION "${LIBMSGWEBSOCKET_MAJOR_VERSION}.${LIBMSGWEBSOCKET_MINOR_VERSION}.${LIBMSGWEBSOCKET_SUB_VERSION}")
set(VERSION_DATE "2014-10-01")

###########################################################
# Where are the additional libraries installed? Note: provide includes
# path here, subsequent checks will resolve everything else
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")

OPTION(USE_DEBUG
  "Build the project in debug-mode"
  NO)

#include ( CompilerFlags )
find_package(OpenSSL)
#find_package(GnuTLS)

configure_file(libmsgwebsocket/config.h.in ${CMAKE_BINARY_DIR}/libmsgwebsocket/config.h)

set (WEBSOCKETPP_PLATFORM_LIBS pthread rt)
set (WEBSOCKETPP_PLATFORM_TSL_LIBS ssl crypto)
set (WEBSOCKETPP_BOOST_LIBS system thread)

set(JSONXX_HOME "${CMAKE_SOURCE_DIR}/jsonxx")
set(WEBSOCKETPP_HOME "${CMAKE_SOURCE_DIR}/websocketpp")
set(WEBSOCKETPP_INCLUDE_DIR ${WEBSOCKETPP_HOME})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/jsonxx)
configure_file(${JSONXX_HOME}/jsonxx.h ${CMAKE_BINARY_DIR}/jsonxx/jsonxx.h COPY_ONLY)
configure_file(${JSONXX_HOME}/jsonxx.cc ${CMAKE_BINARY_DIR}/jsonxx/jsonxx.cc COPY_ONLY)

# We depend on boost >= 1.46.1 because of the updated filesystem
# library. The boost version detection is a mess, see
# $ cmake --help-module FindBoost
# , section " The mess that is Boost_ADDITIONAL_VERSIONS (sorry?)"
SET(Boost_ADDITIONAL_VERSIONS "1.46.1" "1.47" "1.47.1" "1.48.0" "1.48"
  "1.49.0" "1.49" "1.50.0" "1.50")
set (Boost_USE_STATIC_LIBS ON)
SET(Boost_DETAILED_FAILURE_MSG true)
FIND_PACKAGE(Boost 1.46.1
  COMPONENTS program_options filesystem system thread)

#set(JSONXX_INCLUDE_DIR ${JSONXX_HOME})

add_subdirectory(websocket-rails-client-cpp/websocket-rails-client)
message(">> websocke: ${websocket_srcs} <<")
add_subdirectory(libmsgwebsocket)
add_subdirectory(test)
#add_subdirectory(src)

# enable unit testing
include(CTest)
enable_testing()

### print some output for the user
message("")
message("        ***** Configuration parameters *****")
message("             prefix: ${CMAKE_INSTALL_PREFIX}")

# add some files to the installation target
INSTALL(FILES README INSTALL COPYRIGHT DESTINATION
  share/doc/libmsgwebsocket-${V_MAJOR}-${V_MINOR}
)

# CPACK packaging
INCLUDE(InstallRequiredSystemLibraries)
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gridlicense Demo Tools")
set(CPACK_PACKAGE_VENDOR              "Fraunhofer ITWM")
set(CPACK_PACKAGE_CONTACT             "Kai Krueger <krueger@itwm.fhg.de>")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "Kai Krueger <krueger@itwm.fhg.de>")
set(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYRIGHT")
set(CPACK_PACKAGE_VERSION_MAJOR ${V_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${V_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${V_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}-${V_MAJOR}.${V_MINOR}")

set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/.svn/;.swp$;.#;/#;.*~;cscope.*;build.*")

IF(WIN32 AND NOT UNIX)
  set(CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_PACKAGE_NAME "Gridlicense Demo Tools")
  # set(CPACK_PACKAGE_EXECUTABLES "gui4" "Grid License Demo Frontend")
  set(CPACK_PACKAGE_DEFAULT_LOCATION ${INSTALL_DIR})

  # sure there is at least one set of four (4) backlasshes.
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\gui4.exe")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Gridlicense Demo Tools")
  set(CPACK_NSIS_HELP_LINK    "http://www.itwm.fraunhofer.de/")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.itwm.fraunhofer.de/abteilungen/hpc.html")
  set(CPACK_NSIS_CONTACT "Kai Krueger <krueger@itwm.fhg.de>")
  set(CPACK_NSIS_MODIFY_PATH ON)
  # set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CMAKE_PROJECT_NAME}-${EWS_VERSION}")
  # set(CPACK_NSIS_MENU_LINKS "${EWS_WEBSITE}" "Homepage for ${EWS_APP_NAME}")
  set(CPACK_NSIS_DEFINES "  !define MUI_WELCOMEPAGE_TITLE_3LINES 1
    !define MUI_FINISHPAGE_TITLE_3LINES 1")
else(WIN32 AND NOT UNIX)
  #set(CPACK_SET_DESTDIR On)
  set(CPACK_SOURCE_STRIP_FILES "")
  set(CPACK_GENERATOR "TGZ;TBZ2;TZ;DEB;RPM")
  set(CPACK_RPM_PACKAGE_RELOCATABLE 1)
endif(WIN32 AND NOT UNIX)
#include(CPack)

