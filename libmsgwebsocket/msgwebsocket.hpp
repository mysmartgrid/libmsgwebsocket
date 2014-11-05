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

#ifndef _msgwebsocket_hpp_
#define _msgwebsocket_hpp_
#include <string>

//#include <boost/function.hpp>
//#include <boost/bind.hpp>
	
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include "websocket-rails-client/websocket_rails.hpp"

#include <jsonxx/jsonxx.h>



class WebsocketRails;

namespace websocket {

	//typedef boost::function<void(jsonxx::Object)> cb_func;

	class msgwebsocket {
	
	public:
		msgwebsocket(const std::string &url, const int &interval=10, const bool debug=false);
		~msgwebsocket();
	
		void disconnect();
		
		void subscribe(const std::string &channel_name);
		void bind_channel(const std::string &sensor_uuid, const std::string &event_name, cb_func callback);

		void onPostSuccess(cb_func callback);
		void onPostFailure(cb_func callback);
		void onLoadSuccess(cb_func callback);
		void onLoadFailure(cb_func callback);
		void onReceive(cb_func callback);

		void post_measurement(const std::string &value);
		void post_measurement(const std::string &uuid, const double &value);
		void load_measurement(const std::string &uuid, const time_t &start, const time_t &end);
		void ping();

		void received_measurement(jsonxx::Object data);
		
	protected:
		virtual void on_open(jsonxx::Object data);
		virtual void on_close(jsonxx::Object data);
		virtual void on_fail(jsonxx::Object data);

	private:
		void _init();

		void _run();
		void _survey(const boost::system::error_code& error);
		
		void _subscription_success(jsonxx::Object data);
		void _subscription_failure(jsonxx::Object data);

		void _post_success(jsonxx::Object data);
		void _post_failure(jsonxx::Object data);

		void _load_success(jsonxx::Object data);
		void _load_failure(jsonxx::Object data);

		void _ping_success(jsonxx::Object data);
		void _ping_failure(jsonxx::Object data);


	
	
	private:
		boost::asio::io_service     _io_service;	
		boost::asio::signal_set     _signals;
		boost::asio::deadline_timer _timer;
		std::string _url;
		int _interval;
		bool _debug;
		
		bool _firstcall;
		boost::thread _survey_thread;
		WebsocketRails::Ptr _dispatcher;
		cb_func _onPostSuccess_callback;
		cb_func _onPostFailure_callback;
		cb_func _onLoadSuccess_callback;
		cb_func _onLoadFailure_callback;
		cb_func _onReceive_callback;
		
	};
} // namespace websocket
#endif /* _msgwebsocket_hpp_ */
