#include <jsonxx/jsonxx.h>
#include <websocket-rails-client/msgwebsocket.hpp>

namespace websocket {

	//class msgwebsocket;
	
	class msgwebsocketIF : public websocket::msgwebsocket {
	public:
		msgwebsocketIF(const std::string &url, const bool &debug=false) :
				msgwebsocket(url, debug) {}
		~msgwebsocketIF(){};

		void post_measurement(const std::string &value)
			{ ::websocket::msgwebsocket::post_measurement(value); };
		void ping()
			{ ::websocket::msgwebsocket::ping(); };
		void load_measurement(const std::string &value)
			{ ::websocket::msgwebsocket::load_measurement(); };
		
		void on_open(jsonxx::Object data)
			{ ::websocket::msgwebsocket::on_open(data); }
		void on_close(jsonxx::Object data)
			{ ::websocket::msgwebsocket::on_close(data); }
		void on_fail(jsonxx::Object data)
			{ ::websocket::msgwebsocket::on_fail(data); }

		void post_success(jsonxx::Object data)
			{ ::websocket::msgwebsocket::post_success(data); }
		void post_failure(jsonxx::Object data)
			{ ::websocket::msgwebsocket::post_failure(data); }
		
	private:

	}; // class msgwebsocketIF

} // namespace websocket
