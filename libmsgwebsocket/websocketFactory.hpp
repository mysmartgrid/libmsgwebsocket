#ifndef _websocketFactory_hpp_
#define _websocketFactory_hpp_

#include <boost/shared_ptr.hpp>
//#include <websocket-rails-client/msgwebsocket.hpp>

namespace websocket {
	class msgwebsocket;
}


namespace mysmartgrid {

	typedef boost::shared_ptr<websocket::msgwebsocket> websocket_Ptr;

	class websocketFactory {
		typedef boost::shared_ptr<websocketFactory> Ptr;

	public:
		websocketFactory(){}
		~websocketFactory(){}
		
		websocket_Ptr createWebsocket(const std::string &url, const bool debug=false){};
		
	private:
}; // class websocketFactory


} // namespace mysmartgrid
#endif /* _websocketFactory_hpp_ */
