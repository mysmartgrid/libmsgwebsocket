#include <unistd.h>
#include <stdlib.h>

#include <iostream>

#include <websocket-rails-client/websocketFactory.hpp>
#include <websocket-rails-client/msgwebsocket.hpp>


void demo_measurements(jsonxx::Object data) 
{
	std::cout << "Function demo_measurement called" << std::endl;
	std::cout << data.json() << std::endl;
}

int main(int argc, const char* argv[]) {

	if (argc < 3) {
		std::cout << "Usage:" << std::endl;
		std::cout << " "<< argv[0] << " <url> <sensor_uuid> [<sensor_uuid> [...]]" << std::endl;
		return -1;
	}

	websocket::msgwebsocket dispatcher(argv[1], 10, true);

	std::cout<<"========================= Start mainloop\n";
	for( int i=2; i<argc; i++) {
		std::cout << "subscribe to "<< argv[i] << std::endl;
		dispatcher.subscribe(argv[i]);
	}
	dispatcher.bind_channel(argv[2], "create", boost::bind(&demo_measurements, _1));
	
	time_t start = 1414052908;
	time_t end   = 1414056508;
	time(&end);
	start = end-60;
	
	// main loop
	try {
		dispatcher.load_measurement(argv[2], start, end);
	} catch( std::exception &e) {
		std::cout<< "Got en exception: " << e.what()<< std::endl;
		
	}
	
	char c;
	std::cin >> c;
	dispatcher.disconnect();
	std::cin >> c;
	std::cout<<"=========================== END ===============\n";
}
