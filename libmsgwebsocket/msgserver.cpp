#define USE_DEBUG 1

#include <websocket-rails-client/config.h>

#if defined(USE_DEBUG) && USE_DEBUG
#  include <websocketpp/config/debug_asio.hpp>
#else
#  include <websocketpp/config/asio.hpp>
#endif

#include <websocketpp/server.hpp>
#include <websocket-rails-client/websocket.hpp>
#include <websocket-rails-client/event.hpp>

#include <iostream>

#if defined(USE_DEBUG) && USE_DEBUG
typedef websocketpp::server<websocketpp::config::debug_asio_tls> server;
#else
typedef websocketpp::server<websocketpp::config::asio_tls> server;
#endif

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::ref;

// pull out the type of messages sent by our config
//typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef server::message_ptr message_ptr;

typedef websocketpp::lib::shared_ptr<boost::asio::ssl::context> context_ptr;

/* commands and protocol
client2server
[ "measurements.post",
   {
                "data": {
                        "sensor_uuid": "d1d1d1d1d1d1d1d1s1s1s1s1s1s1s1s1",
                        "timestamp": 1413899490,
                        "value": 276
                },
                "id": "b98c1795-14a0-40f1-a371-02d4fda53df7"
        }
]

[
        "websocket_rails.pong",
        {
        }
]

server2client
[["client_connected",
     {"id":null,"channel":null,"user_id":null,
      "data":{"connection_id":"8a8c44b3457d9d934d8a"},
      "success":null,"result":null,"token":null,
      "server_token":null}]]

[["measurements.post_success",
     {"id":null,"channel":null,"user_id":null,
      "data":"post_success","success":null,"result":null,
      "token":null,"server_token":null}]]

[["websocket_rails.ping",
     {"id":null,"channel":null,"user_id":null,
      "data":{},"success":null,"result":null,
      "token":null,"server_token":null}]]

 */


typedef websocketpp::message_buffer::message<websocketpp::message_buffer::alloc::con_msg_manager> message_type;
typedef websocketpp::message_buffer::alloc::con_msg_manager<message_type> con_msg_man_type;

void on_open(server* s, connection_hdl hdl) {
	std::cout<<">>>>>open\n";
//	connection_data data;
	con_msg_man_type::ptr manager(new con_msg_man_type());
	size_t foo = 1024;

	std::ostringstream os;
	os << "{ ";
	os << "\"connection_id\": \"99a9f9506edf53c44f24\", ";
	os << " }";
	jsonxx::Object ConnectionID;
	ConnectionID.parse(os.str());

//messageHandler:::[["client_connected",{"id":null,"channel":null,"user_id":null,"data":{"connection_id":"64c64994b55286e936bb"},"success":null,"result":null,"token":null,"server_token":null}]]

	jsonxx::Array data;
  data << "client_connected" <<  ConnectionID;//{ \"conn\":\"99a9f9506edf53c44f24\"}";
  Event event(data);
	//event.setConnectionId("99a9f9506edf53c44f24");

	std::ostringstream obj;
	obj << "[" << event.serialize() << "]";
	std::cout<< "Json: " << obj.str()<< std::endl;
	
	message_type::ptr msg = manager->get_message(websocketpp::frame::opcode::TEXT,foo);
	msg->set_payload(obj.str());

	s->send(hdl,msg);
}

void dispatch_event(server* s, websocketpp::connection_hdl hdl, message_ptr msg, jsonxx::Array &data) {
  for(int _i = 0, _len = data.size(); _i < _len; _i++) {
		std::cout<<"Dispatch item " << _i << std::endl;
    jsonxx::Array socket_message = data.get<jsonxx::Array>(_i);
    Event event(socket_message);
		std::ostringstream os;
		os << "{ ";
		os << " }";
		jsonxx::Object ConnectionID;
		ConnectionID.parse(os.str());
		jsonxx::Array data;
		bool do_answer=true;
		
    if(event.isResult()) {
      std::cout<<"Event is: RESULT"<< std::endl;
    } else if(event.isChannel()) {
      std::cout<<"Event is: CHANNEL"<< std::endl;
    } else if(event.getName() == "measurements.post") {
      std::cout<<"Event is: MEASUREMENTS.POST"<< std::endl;
			data << "measurements.post_success" <<  ConnectionID;
    } else if(event.getName() == "measurements.load") {
      std::cout<<"Event is: MEASUREMENTS.LOAD"<< std::endl;
    } else if(event.getName() == "websocket_rails.ping") {
      std::cout<<"Event is: PING"<< std::endl;
    } else if(event.getName() == "websocket_rails.pong") {
      std::cout<<"Event is: PONG"<< std::endl;
			do_answer=false;
    } else {
      std::cout<<"Event is: "<< event.getName() << std::endl;
      //this->dispatch(event);
    }

		if(do_answer) {
			std::cout<<"Prepare response\n";
		Event eventRes(data);
		jsonxx::Array dataPing;
		dataPing << "websocket_rails.ping";
		Event eventPing(dataPing);
		
		std::ostringstream obj;
		obj << "[" << eventRes.serialize() << "," << eventPing.serialize() << "]";
		msg->set_payload(obj.str());

		s->send(hdl, msg->get_payload(), msg->get_opcode());
		}
  }

}


void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
	std::cout<< ">>>>>>>message_handler\n";
//	std::cout << "on_message called with hdl: " << hdl.lock().get()
//						<< " and message: " << msg->get_payload()
//						<< std::endl;
	try {
		std::string event_names;
		std::string tmp = "[" + msg->get_payload() + "]";
		jsonxx::Array event_data;
		event_data.parse(tmp);
#if defined(USE_DEBUG) && USE_DEBUG
		std::cout<<"messageHandler:: str=" << msg->get_payload() << std::endl;
		std::cout<<"messageHandler:: size=" << event_data.size()<< std::endl;
#endif

		// decode
		for(int i = 0, len = event_data.size()-1; i < len; i++) {
			std::cout<<"Get item " << i << std::endl;
			event_names += event_data.get<jsonxx::Array>(i).get<jsonxx::String>(0) + " ";
			std::cout<<"Item finished" << std::endl;
		}
		dispatch_event(s, hdl, msg, event_data);


	} catch (const websocketpp::lib::error_code& e) {
		std::cout << "Echo failed because: " << e
							<< "(" << e.message() << ")" << std::endl;
	}
}

bool validate(server & s, connection_hdl hdl) {
	server::connection_ptr con = s.get_con_from_hdl(hdl);

	std::cout << ">>>>>>Cache-Control: " << con->get_request_header("Cache-Control") << std::endl;

	const std::vector<std::string> & subp_requests = con->get_requested_subprotocols();
	std::vector<std::string>::const_iterator it;

	for (it = subp_requests.begin(); it != subp_requests.end(); ++it) {
		std::cout << "Requested: " << *it << std::endl;
	}

	if (subp_requests.size() > 0) {
		con->select_subprotocol(subp_requests[0]);
	}

	return true;
}

void http_handler(server & s, connection_hdl hdl) {
	std::cout<< ">>>>>>>http_handler\n";
}


std::string get_password() {
	return "test";
}

context_ptr on_tls_init(websocketpp::connection_hdl hdl) {
	std::cout << "on_tls_init called with hdl: " << hdl.lock().get() << std::endl;
	context_ptr ctx(new boost::asio::ssl::context(boost::asio::ssl::context::tlsv1));

	try {
		ctx->set_options(boost::asio::ssl::context::default_workarounds |
										 boost::asio::ssl::context::no_sslv2 |
										 boost::asio::ssl::context::single_dh_use);
		//ctx->set_password_callback(bind(&get_password));
		ctx->use_certificate_chain_file("STAR_mysmartgrid_de.crt");
		ctx->use_private_key_file("mysmartgrid.de.key", boost::asio::ssl::context::pem);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	return ctx;
}

int main() {
	// Create a server endpoint
	//server::handler::ptr h(new handler());
	server echo_server;

	try {
		// Set logging settings
		echo_server.set_access_channels(websocketpp::log::alevel::all);
		echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

		// Initialize ASIO
		echo_server.init_asio();
		echo_server.set_reuse_addr(true);

		// Register our message handler
		echo_server.set_message_handler(bind(&on_message,&echo_server,::_1,::_2));
		echo_server.set_validate_handler(bind(&validate,ref(echo_server),::_1));
		echo_server.set_http_handler(bind(&http_handler,ref(echo_server),::_1));
		echo_server.set_open_handler(bind(&on_open,&echo_server,::_1));

		//echo_server.set_validate_handler(bind(&validate,&echo_server,::_1));
		echo_server.set_tls_init_handler(bind(&on_tls_init,::_1));


		// Listen on port 9002
		echo_server.listen(9002);

		// Start the server accept loop
		echo_server.start_accept();

		// Start the ASIO io_service run loop
		echo_server.run();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	} catch (websocketpp::lib::error_code e) {
		std::cout << e.message() << std::endl;
	} catch (...) {
		std::cout << "other exception" << std::endl;
	}

}
