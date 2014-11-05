/**
 * This file is part of libwebsocket.
 *
 * (c) Fraunhofer ITWM - Kai Krueger <kai.krueger@itwm.fhg.de>,    2014
 *                       Ely de Oliveira   <ely.oliveira@itwm.fhg.de>, 2014
 *
 * libklio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libklio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libklio. If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>


#include <websocket-rails-client/msgwebsocket.hpp>
#include "websocket-rails-client/websocket_rails.hpp"

/** valid events

client_connected
websocket_rails.subscribe
websocket_rails.unsubscribe
websocket_rails.subscribe_private
websocket_rails.channel_token
websocket_rails.ping
websocket_rails.pong
measurements.post
measurements.load
load
create

measurements.load_success
websocket_rails.channel_token

*/

websocket::msgwebsocket::msgwebsocket(
	const std::string &url
	, const int &interval
	, const bool debug) 
		: _signals(_io_service)//, SIGINT, SIGTERM)
		, _timer(_io_service)
		, _url(url)
		, _interval(interval)
		, _debug(debug)
		, _firstcall(true)
{
	_dispatcher = WebsocketRails::Ptr(new WebsocketRails(_url));
	
	_init();
}

websocket::msgwebsocket::~msgwebsocket() {
	_dispatcher->disconnect();
	_dispatcher.reset();
}

void websocket::msgwebsocket::disconnect() {
	_dispatcher->disconnect();
}


void websocket::msgwebsocket::subscribe(const std::string &sensor_uuid)
{
	// subscribe to a channel
	_debug && std::cout << "Subscribe to sensor: " << sensor_uuid << std::endl;

	std::ostringstream channel_id;
	channel_id << "sensor:" << sensor_uuid;

	Channel::Ptr channel = 
		_dispatcher->subscribe(channel_id.str()
													 , boost::bind(&websocket::msgwebsocket::_subscription_success, this, _1)
													 , boost::bind(&websocket::msgwebsocket::_subscription_failure, this, _1)
													 );
	channel->bind("load", boost::bind(&msgwebsocket::received_measurement, this, _1));
	channel->bind("create", boost::bind(&msgwebsocket::received_measurement, this, _1));
	channel->bind("update", boost::bind(&msgwebsocket::received_measurement, this, _1));
}

void websocket::msgwebsocket::bind_channel(const std::string &sensor_uuid, const std::string &event_name, cb_func callback)
{
	// subscribe to a channel
	_debug && std::cout << "Bind event "<< event_name << " on sensor: " << sensor_uuid << std::endl;

	std::ostringstream channel_id;
	channel_id << "sensor:" << sensor_uuid;

	Channel::Ptr channel = _dispatcher->subscribe(channel_id.str());
	channel->bind(event_name, callback);
}

void websocket::msgwebsocket::onPostSuccess(cb_func callback) {
  this->_onPostSuccess_callback = callback;
}

void websocket::msgwebsocket::onPostFailure(cb_func callback) {
  this->_onPostFailure_callback = callback;
}

void websocket::msgwebsocket::onLoadSuccess(cb_func callback) {
  this->_onLoadSuccess_callback = callback;
}

void websocket::msgwebsocket::onLoadFailure(cb_func callback) {
  this->_onLoadFailure_callback = callback;
}

void websocket::msgwebsocket::onReceive(cb_func callback) {
  this->_onReceive_callback = callback;
}

void websocket::msgwebsocket::_init() {
	_debug && std::cout << "Init called" << std::endl;
	_dispatcher->onOpen(boost::bind(&msgwebsocket::on_open, this, _1));
	_dispatcher->onClose(boost::bind(&msgwebsocket::on_close, this,_1));
	_dispatcher->onFail(boost::bind(&msgwebsocket::on_fail, this, _1));
	_dispatcher->connect();

	//boost::asio::signal_set signals(_io_service, SIGINT, SIGTERM);
  this->_survey_thread = boost::thread(&websocket::msgwebsocket::_run, this);
	//this->_survey(boost::system::error_code());
}

void websocket::msgwebsocket::_run() {
	this->_survey(boost::system::error_code());
	_signals.async_wait(
		boost::bind(&boost::asio::io_service::stop, &_io_service));
	_io_service.run();
}

void websocket::msgwebsocket::_survey(const boost::system::error_code& error)
{
	_timer.expires_from_now(boost::posix_time::seconds(_interval+(rand()%_interval)));
	_timer.async_wait(boost::bind(&websocket::msgwebsocket::_survey, this, _1));
	if(!this->_firstcall) {
		if(!_dispatcher->isConnected()) {
			_debug && std::cout<< "   trying to connect..."<< std::endl;
		
			try {
				_dispatcher->reconnect();
				if(!_dispatcher->isConnected()) {
					_debug && std::cout<<">>>> NOT CONNECTED...\n";
					return;
				}
			
			}
			catch(std::exception &e) {
				std::cout<<"Except: "<< e.what() << std::endl;
			}
		}
		//this->ping();
	}
	
	this->_firstcall = false;
}


void websocket::msgwebsocket::_subscription_success(jsonxx::Object data) {

	_debug && std::cout << "Function subscription_success called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	//if(this->_onPostSuccess_callback) {
//		websocket::cb_func callback = this->_onPostSuccess_callback;
	//	callback(data);
	//}
}

void websocket::msgwebsocket::_subscription_failure(jsonxx::Object data) {

	_debug && std::cout << "Function subscription_failure called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	//if(this->_onPostFailure_callback) {
	//	websocket::cb_func callback = this->_onPostFailure_callback;
	//	callback(data);
	//}
}

void websocket::msgwebsocket::_post_success(jsonxx::Object data) {

	_debug && std::cout << "Function post_success called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	if(this->_onPostSuccess_callback) {
		cb_func callback = this->_onPostSuccess_callback;
		callback(data);
	}
}

void websocket::msgwebsocket::_post_failure(jsonxx::Object data) {

	_debug && std::cout << "Function post_failure called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	if(this->_onPostFailure_callback) {
		cb_func callback = this->_onPostFailure_callback;
		callback(data);
	}
}

void websocket::msgwebsocket::post_measurement(const std::string &uuid, const double &value) {
	// get timestamp
	time_t timestamp;
	time(&timestamp);
			
	std::ostringstream os;
	os << "{\"sensor_uuid\": \"" << uuid << "\", \"timestamp\": " << timestamp << ", \"value\": " << value << "}";
	return this->post_measurement(os.str());
}

void websocket::msgwebsocket::post_measurement(const std::string &value) {

	// check connection
	_debug && std::cout << "send measurement" << std::endl;
#if 0
	if(!_dispatcher->isConnected()) {
		_debug && std::cout<< "   trying to connect..."<< std::endl;
		
		try {
			_dispatcher->reconnect();
			if(!_dispatcher->isConnected()) {
				_debug && std::cout<<">>>> NOT CONNECTED...\n";
				return;
			}
			
		}
		catch(std::exception &e) {
			std::cout<<"Except: "<< e.what() << std::endl;
		}
	}
#endif

	jsonxx::Object measurement;
	measurement.parse(value);
	Event post = _dispatcher->trigger("measurements.post", measurement, 
																	 boost::bind(&msgwebsocket::_post_success, this, _1),
																	 boost::bind(&msgwebsocket::_post_failure, this, _1));
}


void websocket::msgwebsocket::_load_success(jsonxx::Object data) {

	_debug && std::cout << "Function load_success called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	if(this->_onLoadSuccess_callback) {
		cb_func callback = this->_onLoadSuccess_callback;
		callback(data);
	}
}

void websocket::msgwebsocket::_load_failure(jsonxx::Object data) {

	_debug && std::cout << "Function load_failure called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
	if(this->_onLoadFailure_callback) {
		cb_func callback = this->_onLoadFailure_callback;
		callback(data);
	}
}

/*
I [2014-10-23 13:25:21.381] [Dispatcher] Started Event: measurements.load
I [2014-10-23 13:25:21.381] [Dispatcher] Name: measurements.load
I [2014-10-23 13:25:21.382] [Dispatcher] Data: {"from"=>1414056508, "sensor_uuid"=>"d1d1d1d1d1d1d1d1s2s2s2s2s2s2s2s2", "to"=>1414052908}
I [2014-10-23 13:25:21.382] [Dispatcher] Connection: #<Connection::18f686bb7d4f23f0fd43>
I [2014-10-23 13:25:21.383] [Channel] [sensor:d1d1d1d1d1d1d1d1s2s2s2s2s2s2s2s2] {}
I [2014-10-23 13:25:21.384] [Dispatcher] Event measurements.load Finished in 0.001885603 seconds

I [2014-10-20 09:43:01.547] Dispatcher] Started Event: measurements.load
I [2014-10-20 09:43:01.547] Dispatcher] Name: measurements.load
I [2014-10-20 09:43:01.547] Dispatcher] Data: {"sensor_uuid"=>"d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1", "from"=>1413787381, "to"=>1413790981}
I [2014-10-20 09:43:01.547] Dispatcher] Connection: #<Connection::e06ba1b61e5c9e375f73>
I [2014-10-20 09:43:01.757] [Channel] [sensor:d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1] {1413787381=>285.0, 1413787382=>157.0,
...
I [2014-10-20 09:43:01.793] Dispatcher Event measurements.load Finished in 0.245425182 seconds
I [2014-10-20 09:43:01.795] ConnectionManager Connection opened: #<Connection::3eb9a278a4399cba6e3c>

 */

/*
	///* Subscribe to Channel

    std::ostringstream channel_id;
    channel_id << "sensor:" << argv[1];
    std::cout << std::endl << "Subscribing channel: " << channel_id.str() << std::endl;

    Channel channel = dispatcher.subscribe(channel_id.str(), boost::bind(subscription_success, _1), boost::bind(subscription_failure, _1));
    channel.bind("load", boost::bind(received_measurement, _1));
    channel.bind("create", boost::bind(received_measurement, _1));
    channel.bind("update", boost::bind(received_measurement, _1));

    std::cout << "Listening to channel: " << channel_id.str() << std::endl;

*/
void websocket::msgwebsocket::received_measurement(jsonxx::Object data) {

    std::cout << ">>>>>>Function received_measurement called" << std::endl;
    std::cout << data.json() << std::endl;
//	if(this->_onReceive_callback) {
//		cb_func callback = this->_onLoadFailure_callback;
//		callback(data);
//	}
}

void websocket::msgwebsocket::load_measurement(
	const std::string &uuid,
	const time_t &start, 
	const time_t &end
	) {
	_debug && std::cout << "load measurement" << std::endl;
//{"sensor_uuid"=>"d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1", "from"=>1413787381, "to"=>1413790981}


	std::ostringstream os;
	os << "{\"sensor_uuid\": \"" << uuid 
		 << "\", \"from\": " << start 
		 << ", \"to\": " << end << "}";
	std::cout<<"Load: "<< os.str() << std::endl;
	
	if(!_dispatcher->isConnected()) {
		_debug && std::cout<< "   trying to connect..."<< std::endl;
		
		try {
			_dispatcher->reconnect();
			if(!_dispatcher->isConnected()) {
				_debug && std::cout<<">>>> NOT CONNECTED...\n";
				return;
			}
			
		}
		catch(std::exception &e) {
			std::cout<<"Except: "<< e.what() << std::endl;
		}
	}

	jsonxx::Object measurement;
	measurement.parse(os.str());
	Event load = _dispatcher->trigger("measurements.load", measurement, 
																	 boost::bind(&msgwebsocket::_load_success, this, _1),
																	 boost::bind(&msgwebsocket::_load_failure, this, _1));

}

void websocket::msgwebsocket::_ping_success(jsonxx::Object data) {

	_debug && std::cout << "Function ping_success called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
}

void websocket::msgwebsocket::_ping_failure(jsonxx::Object data) {

	_debug && std::cout << "Function ping_failure called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
}

void websocket::msgwebsocket::ping() {
	_debug && std::cout<<"trigger ping...\n";
	jsonxx::Object ping;
	ping.parse("{}");
	Event evPing =  _dispatcher->trigger("websocket_rails.ping", ping,
																	 boost::bind(&msgwebsocket::_ping_success, this, _1),
																	 boost::bind(&msgwebsocket::_ping_failure, this, _1));
}

void websocket::msgwebsocket::on_open(jsonxx::Object data) {
	 _debug && std::cout<<"on_open\n";
	 _debug && std::cout << "Function on_open called" << std::endl;
	 _debug && std::cout << data.json() << std::endl;

}

void websocket::msgwebsocket::on_close(jsonxx::Object data) {

	_debug && std::cout << "Function on_close called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
}

void websocket::msgwebsocket::on_fail(jsonxx::Object data) {

	_debug && std::cout << "Function on_fail called" << std::endl;
	_debug && std::cout << data.json() << std::endl;
}

