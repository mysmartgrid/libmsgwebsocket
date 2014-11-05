# libmsgwebsocket


MsgWebsocket (MySmartGrid Websocket) is a C++ library that implements
the WebsocketRails protocol for the MySmartGrid Websocket interface
It Extends the websocket-rails-client-cpp [1] library.

## Requirements

### allready copied to that repository
* [1] websocket-rails-client-cpp, https://github.com/phlegx/websocket-rails-client-cpp
* WebSocket++ library, https://github.com/zaphoyd/websocketpp (>= 0.3.0 | see own license)
* Jsonxx library, https://github.com/hjiang/jsonxx (latest | see own license)


### As external requirements
* Boost library up and running, http://www.boost.org/ (>= 1.48 | see own license)
* Websocket-Rails server, https://github.com/websocket-rails/websocket-rails (v0.7.0)


# test

./test/client wss://dev4-playground.mysmartgrid.de/websocket d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1 1


./test/demo_load wss://dev4-playground.mysmartgrid.de/websocket d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1



