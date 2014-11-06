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


# demo programs

./test/client wss://dev4-playground.mysmartgrid.de/websocket d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1 1


./test/demo_load wss://dev4-playground.mysmartgrid.de/websocket d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1


# CPP

## Post values

``` // create the connection
``` websocket::msgwebsocket dispatcher(url);
``` // post a value for a sensor
``` dispatcher.post_measurement(sensor_uuid, value);
``` dispatcher.disconnect();

## get values

``` // function to call when the server sends a new value
``` void demo_measurements(jsonxx::Object data) 
``` {
``` 	std::cout << "Function demo_measurement called" << std::endl;
``` 	std::cout << data.json() << std::endl;
``` }

``` // create the connection
``` websocket::msgwebsocket dispatcher(url);
``` // subscribe a sensor
```	dispatcher.subscribe(sensor_uuid);
``` // bind the function to call when the server sends new values
```	dispatcher.bind_channel(sensor_uuid, "create", boost::bind(&demo_measurements, _1));
``` // query the server for values within an timeinterval
```	dispatcher.load_measurement(sensor_uuid, start_time, end_time);
``` dispatcher.disconnect();