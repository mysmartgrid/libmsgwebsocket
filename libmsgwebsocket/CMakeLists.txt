# -*- mode: cmake; -*-

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
include_directories(${WEBSOCKETPP_INCLUDE_DIR})

message(">> websocke: ${websocket_srcs} <<")

set(libmsgwebsocket_srcs
	${websocket_srcs}
	${CMAKE_SOURCE_DIR}/jsonxx/jsonxx.cc
	msgwebsocket.cpp
)

set_source_files_properties(${libmsgwebsocket_srcs} COMPILE_FLAGS "-std=c++0x -D_WEBSOCKETPP_CPP11_STL_")

add_library(libmsgwebsocket ${libmsgwebsocket_srcs})

