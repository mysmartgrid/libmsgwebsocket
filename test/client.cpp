#include <iostream>
#include <libmsgwebsocket/msgwebsocket.hpp>


int main(int argc, const char* argv[]) {

	printf("Argc: %d\n", argc);
	
	if (argc != 3 && argc !=5) {
		std::cout << "Usage:" << std::endl;
		std::cout << " ./client <url> <sensor_uuid> [<timestamp> <value>]" << std::endl;
		return -1;
	}

	std::string url("wss://dev4-playground.mysmartgrid.de/websocket");
	websocket::msgwebsocket dispatcher(argv[1]);
		

	if(argc==3) {
		// main loop
		do {
			// create a value
			float value = random() % 500;
			
			try {
				dispatcher.post_measurement(argv[2], value);
			
				usleep(1000000);
			} catch( std::exception &e) {
				std::cout<< "Got en exception: " << e.what()<< std::endl;
					
			}
				
		} while(1);
	}
	else {
		try {
			std::cout<<"Post a single value."<<std::endl;
			float value = atof(argv[4]);
			dispatcher.post_measurement(argv[2], value);
			
		} catch( std::exception &e) {
			std::cout<< "Got en exception: " << e.what()<< std::endl;
					
		}
	
		std::cout<<"Press q<return> to terminate."<<std::endl;
		char c;
		std::cin >> c;
		dispatcher.disconnect();
		std::cout<<"Called disconnect. Press q<return> to terminate."<<std::endl;
		std::cin >> c;
	}
	std::cout<<"=========================== END ===============\n";
}
