#include <iostream>
#include <websocket-rails-client/msgwebsocket.hpp>


int main(int argc, const char* argv[]) {

    if (argc != 4) {
        std::cout << "Usage:" << std::endl;
        std::cout << " ./client <url> <sensor_uuid> <timestamp> <value>" << std::endl;
        return -1;
    }

		std::string url("wss://dev4-playground.mysmartgrid.de/websocket");
		websocket::msgwebsocket dispatcher(argv[1]);
		

    // main loop
		do {
			// create a value
			float value = random() % 500;
			
			// get timestamp
			time_t timestamp;
			time(&timestamp);
			
			std::ostringstream os;
			os << "{\"sensor_uuid\": \"" << argv[2] << "\", \"timestamp\": " << timestamp << ", \"value\": " << value << "}";

			try {
				dispatcher.post_measurement(argv[2], value);
				//dispatcher.post_measurement(os.str());

				usleep(1000000);
			} catch( std::exception &e) {
					std::cout<< "Got en exception: " << e.what()<< std::endl;
					
				}
				
		} while(1);
		
		//char c;
		//std::cin >> c;
		//dispatcher.disconnect();
		//std::cin >> c;
		std::cout<<"=========================== END ===============\n";
}
