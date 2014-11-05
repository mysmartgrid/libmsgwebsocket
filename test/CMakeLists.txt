# -*- mode: cmake; -*-

set(demo_load_srcs demo_load.cpp)
set_source_files_properties(${demo_load_SRCS} COMPILE_FLAGS "-std=c++0x -D_WEBSOCKETPP_CPP11_STL_")

add_executable(demo_load ${demo_load_srcs})    
target_link_libraries(demo_load websocket)
target_link_libraries(demo_load ${Boost_LIBRARIES} pthread)
target_link_libraries(demo_load ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})

#
set(client_srcs client.cpp)
set_source_files_properties(${client_SRCS} COMPILE_FLAGS "-std=c++0x -D_WEBSOCKETPP_CPP11_STL_")

add_executable(client ${client_srcs})    
target_link_libraries(client websocket)
target_link_libraries(client ${Boost_LIBRARIES} pthread)
target_link_libraries(client ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
