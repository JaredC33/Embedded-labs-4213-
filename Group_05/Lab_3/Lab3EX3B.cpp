//Use g++ joystick.cc -std=c++11 -o Lab3EX3B Lab3EX3B.cpp

#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include "joystick.hh"
#include <cstdlib>

#define  PORT 8080
using namespace std;

int createSocket();

int sock = 0;
//CLIENT SIDE
int main(int argc, char const *argv[]){
	
	//Open the file stream for the joystick
	Joystick joystick("/dev/input/js0");
	JoystickEvent event;
	if(!joystick.isFound()){
		cout << "Error opening joystick" << endl;
		exit(1);
	}


	//Create the connection to the server
	createSocket();
	bool quitFlag= false;
	while(true){

		/*Set a delay*/
		usleep(20000);
		/*Convert the event to a useable data type so it can be sent*/
		char send_string;

		/*Sample the events from the joystick*/
		if (!joystick.sample(&event)) continue;

		if (event.isButton() && event.value) {
			printf("isButton: %u | Value: %d\n", event.number, event.value);
			/*Interpret the joystick input and use that input to move the Kobuki*/
			
			//START button
			if ((event.number==7)&&(event.value==1)) send_string ='s';
			
			//Logitech button is pressed
			//exit the script and close the Kobuki's connection cleanly
			else if ((event.number==8)&&(event.value)) {
				send_string='q';
				quitFlag=true;
			}
		}
		if (event.isAxis()) {
			printf("isAxis: %u | Value: %d\n", event.number, event.value);
			//D-Pad Left/Right
			if (event.number==6){
				if (event.value > 100) send_string='r';
				else if (event.value < -100) send_string='l';
			}

			// D-Pad Up/Down
			if (event.number==7) {
				if (event.value < -100) send_string='u';
				else if (event.value > 100) send_string='d';
			}
		}

	
		/*Print the data stream to the terminal*/
		cout<< "Send String: "<<send_string<<endl;
		/*Send the data to the server*/
		
		send(sock , &send_string , strlen(&send_string) , 0);

		/*Closes out of all connections cleanly*/
		//When you need to close out of the connection, please
		//close TTP/IP data streams.
		//Not doing so will result in the need to restart
		//the raspberry pi and Kobuki
		if(quitFlag) {
			send(sock , "q" , strlen(&send_string) , 0);
			close(sock);
			exit(0);
		}
		/*Set a delay*/
		usleep(20000);
	}
	return 0;
}

//Creates the connection between the client and
//the server with the controller being the client

int createSocket() {
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\nSocket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);

	/*Use the IP address of the server you are connecting to*/
	if(inet_pton(AF_INET, "127.0.0.1" , &serv_addr.sin_addr) <= 0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\nConnection Failed \n");
		return -1;
	}
	return 0;
}

