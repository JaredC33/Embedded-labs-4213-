// g++ -std=c++11 -o FinalEXE4b_CPP FinalEXE4b_CPP.cpp -lwiringPi -pthread


#include <iostream>
#include <unistd.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <chrono>
// #include <ratio>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include<cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctime>
#include <thread>

using namespace std;
#define  PORT 8000
#define  IP "127.0.0.1"

int sock = 0;
void movement(int, int);
FILE * file;
char checksum(char *packet,int packet_size);
int createSocket();
int kobuki;

unsigned int bumper;
unsigned int drop;
unsigned int cliff;
unsigned int button;
char cmd = 's';

void readData();



void read_socket(){
	char buffer[100];
	while(1){
		read(sock , buffer, 50);
		/*Print the data to the terminal*/
		cmd = buffer[0];
		printf("received: %c\n",cmd);

		// use cmd to control the robot movement
		switch (buffer[0]) {
		case 's': //START button is pressed
					movement(0,0); //stop robot
					break;
		case 'q': //Logitech button is pressed
					//exit the script and close the Kobuki's connection cleanly
					movement(0, 0);
					cout<<"Kobuki closing..."<<endl;
					serialClose(kobuki);
					close(sock);
					exit(0);

					break;
		case 'l': 	for(int i=0; i<10; ++i) 
						movement(100, 1);
					break;
		case 'r': //rotates clockwise
					for(int i=0; i<10; ++i) movement(100, -1);
					break;
		case 'u': 	for(int i=0; i<10; ++i) movement(100, 0);
					break;
		case 'd': 	for(int i=0; i<10; ++i) movement(-100, 0);
					break;
		default: 	cout<<"Invalid command."<<endl;
	}
		
		//clean the buffer
		memset(&buffer, '0', sizeof(buffer));

	}

}

int main(){
	setenv("WIRINGPI_GPIOMEM", "1", 1);
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	createSocket();
	char buffer[10];
	std::thread  t(read_socket);

	while(serialDataAvail(kobuki) != -1)
	{
		// Read the sensor data.
		readData();


		// Construct an string data like 'b0c0d0', you can use "sprintf" function. You can also define your own data protocal.
		char char_bumper, char_cliff, char_drop;
		
		switch (bumper) {
			case 1: char_bumper = '1'; 
					cout<<"right bumper"<<endl;
					break; //right bumper
			case 2: char_bumper = '2'; break; //center bumper
			case 4: char_bumper = '4'; break; //left bumper
		}
		switch (cliff) {
			case 1: char_cliff = '1'; 
					cout<<"right cliff"<<endl;
					break; //right cliff
			case 2: char_cliff = '2'; break; //center cliff
			case 4: char_cliff = '4'; break; //left cliff
		}
		switch (drop) {
			case 1: char_drop = '1'; break; //right wheel
			case 2: char_drop = '2'; break; //left wheel
		}
		
		char data[10];
		//sprintf(data, "b%cc%cd%c", char_bumper, char_cliff, char_drop);
		sprintf(data, "b%dc%dd%d", bumper, cliff, drop);

		// Send the sensor data through the socket
		send(sock, data, strlen(data), 0);
		
		// Clear the buffer
		memset(&buffer, '0', sizeof(buffer));

		// You can refer to the code in previous labs. 
	}
	serialClose(kobuki);
	
	return(0);
}

void movement(int sp, int r){
	//Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/
	unsigned char b_3 = 0x01; /*Byte 3: Payload Header*/
	unsigned char b_4 = 0x04; /*Byte 4: Payload Data: Length*/
	unsigned char b_5 = sp & 0xff;	/*Byte 5: Payload Data: Speed(mm/s)*/
	unsigned char b_6 = (sp >> 8) & 0xff;	/*Byte 6: Payload Data: Speed(mm/s)*/
	unsigned char b_7 = r & 0xff;	/*Byte 7: Payload Data: Radius(mm)*/
	unsigned char b_8 = (r >> 8) & 0xff;		/*Byte 8: Payload Data: Radius(mm)*/
	unsigned char checksum = 0;		/*Byte 9: Checksum*/
	// file = fopen("/dev/kobuki","w");
	char packet[] = {b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8};
	for (unsigned int i = 2; i < 9; i++)
		checksum ^= packet[i];
	serialPutchar (kobuki, b_0);
	serialPutchar (kobuki, b_1);
	serialPutchar (kobuki, b_2);
	serialPutchar (kobuki, b_3);
	serialPutchar (kobuki, b_4);
	serialPutchar (kobuki, b_5);
	serialPutchar (kobuki, b_6);
	serialPutchar (kobuki, b_7);
	serialPutchar (kobuki, b_8);
	serialPutchar (kobuki, checksum);
	delay(30);
}


int createSocket(){
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
	if(inet_pton(AF_INET, IP , &serv_addr.sin_addr) <= 0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		printf("\nConnection Failed \n");
		return -1;
	}
	return 0;
}

void readData(){
	unsigned int read;
	while(true){
		//If the bytes are a 1 followed by 15, then we are
		//parsing the basic sensor data packet
			read = serialGetchar(kobuki);
			if(read == 1){
				if(serialGetchar(kobuki) == 15) break;
			}
		}

		//Read past the timestamp
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		/*Read the bytes containing the bumper, wheel drop,
			and cliff sensors.*/
		bumper = serialGetchar(kobuki);
		drop = serialGetchar(kobuki);
		cliff = serialGetchar(kobuki);

		/*Read through the bytes between the cliff sensors and
		the button sensors.*/
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		serialGetchar(kobuki);
		/*Read the byte containing the button data.*/
		button = serialGetchar(kobuki);


		if (button == 2)
		{
			cout<<"button B1 pushed"<<endl;
			serialClose(kobuki);
			// break;
		}

		/*Pause the script so the data read receive rate is
		the same as the Kobuki send rate.*/
		usleep(20000);
		
}