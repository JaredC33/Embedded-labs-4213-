//Use g++ -std=c++11 -o Lab4EX3 Lab4EX3.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

unsigned int bumper =0;
unsigned int drop = 0;
unsigned int cliff = 0;
unsigned int button = 0;
unsigned int read_char = 0; 
bool flag;

void movement(int, int);
void readData();

int main(){
	//Create connection to the Kobuki
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	
	while(serialDataAvail(kobuki) != -1){
		/*Read the initial data. If there are no flags,
		the default condition is forward.*/
		/*Move slowly to give the sensors enough time to read data,
		the recommended speed is 100mm/s*/
		readData();
		bool flag = false;

		/*Create different states as to satisfy the conditions above.
		Remember, a single press of a bumper may last longer
		than one data cycle.*/
				
		/*Cleanly close out of all connections using Button 1.*/
		if (button == 2) {
			for(int i=0; i<10; ++i)	movement(0,0);
			serialFlush(kobuki);
			serialClose(kobuki);
			cout << "kobuki closing..."<<endl;
			return 0;
		}

		string bumper_s, wdrop_s, cliff_s;
		if (bumper == 1)  {
			flag = true;
			bumper_s = "right bumper";
			//turn left
			cout<<"\t\t\t\t\t\t\t left"<<endl;
			for(int i=0; i<125; ++i) movement(100, 1);
			for(int i=0; i<10; ++i)	movement(0,0);
		}

		else if (bumper == 2) {
			flag = true;

			bumper_s = "center bumper";
			//reverse and turn 180 deg
			movement(-100, 0);
			for(int i=0; i<250; ++i) movement(100, -1);
			for(int i=0; i<10; ++i)	movement(0,0);

		}
		else if (bumper == 4) {
			flag = true;
			bumper_s = "left bumper";
			//turn right
			
			cout<<"\t\t\t\t\t\t\t right"<<endl;
			for(int i=0; i<125; ++i) movement(100, -1);
			//stop
			for(int i=0; i<10; ++i)	movement(0,0);
		}
		
		if (drop == 1) {
			flag = true;
			wdrop_s = "right wheel";
			//turn left
			cout<<"\t\t\t\t\t\t\t left"<<endl;
			for(int i=0; i<125; ++i) movement(100, 1);
			for(int i=0; i<10; ++i)	movement(0,0);
		}
		if (drop == 2) {
			flag = true;
			wdrop_s = "left wheel";
			//turn right
			cout<<"\t\t\t\t\t\t\t right"<<endl;
			for(int i=0; i<125; ++i) movement(100, -1);
			//stop
			for(int i=0; i<10; ++i)	movement(0,0);
		}

		switch (cliff) {
		case 1: {
			flag = true;
			cliff_s = "right cliff sensor";
			//turn left
			cout<<"\t\t\t\t\t\t\t left"<<endl;
			for(int i=0; i<125; ++i) movement(100, 1);
			for(int i=0; i<10; ++i)	movement(0,0);
			break;
		}
		case 2: {
			flag = true;
			cliff_s = "center cliff sensor";
			//reverse and turn 180 deg
			movement(-100, 0);
			for(int i=0; i<250; ++i) movement(100, -1);
			for(int i=0; i<10; ++i)	movement(0,0);
			break;
			}
		case 4: {
			flag = true;
			cliff_s = "left cliff sensor";
			//turn right
			cout<<"\t\t\t\t\t\t\t right"<<endl;
			for(int i=0; i<125; ++i) movement(100, -1);
			//stop
			for(int i=0; i<10; ++i)	movement(0,0);
			break;
			}
		}
		if (!flag) {
			movement(100, 0);
		}

		/*Print the data to the screen.*/
		cout << "Bumper: "<< bumper_s << " Wheel drop: "<<wdrop_s << " Cliff sensor: "<< cliff_s<<endl;


		/*Use serialFlush(kobuki) to discard all data received, or waiting to be send down the given device.*/
		serialFlush(kobuki);
		//Pause the script so the data read receive rate is the same as the Kobuki send rate.
		usleep(20000);
		
	}
}

void movement(int sp, int r){
	// you can reuse your code from Lab 3
	
	//Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x06; /*Byte 2: Length of Payload*/
	unsigned char b_3 = 0x01; /*Byte 3: Sub-Payload Header*/
	unsigned char b_4 = 0x04; /*Byte 4: Length of Sub-Payload*/
	unsigned char b_5 = sp & 0xff;	//Byte 5: Payload Data: Speed(mm/s)
	unsigned char b_6 = (sp >> 8) & 0xff; //Byte 6: Payload Data: Speed(mm/s)
	unsigned char b_7 = r & 0xff;	//Byte 7: Payload Data: Radius(mm)
	unsigned char b_8 = (r >> 8) & 0xff;	//Byte 8: Payload Data: Radius(mm)
	unsigned char checksum = 0;		//Byte 9: Checksum
	
	//Checksum all of the data
	char packet[] = {b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8};
	for (unsigned int i = 2; i < 9; i++)
		checksum ^= packet[i];

	/*Send the data (Byte 1 - Byte 9) to Kobuki using serialPutchar (kobuki, );*/
	char send[] ={b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8,};
	
	for (unsigned int i = 0; i < 9; i++)
		serialPutchar(kobuki, send[i]);

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);

}

void readData(){
	// you can reuse your code from EXE1, Lab 4

	while(true){
		//If the bytes are a 1 followed by 15, then we are
		//parsing the basic sensor data packet
			read_char = serialGetchar(kobuki);
			if(read_char == 1){
				if(serialGetchar(kobuki) == 15) break;
			}
		}
	//Read past the timestamp
	serialGetchar(kobuki);
	serialGetchar(kobuki);
	/*Read the bytes containing the bumper, wheel drop,
		and cliff sensors. You can convert them into a usable data type.*/
	bumper = serialGetchar(kobuki);
	drop = serialGetchar(kobuki);
	cliff = serialGetchar(kobuki);
	/*Read through 6 bytes between the cliff sensors and
	the button sensors.*/
	for (int i = 0; i<6; i++) serialGetchar(kobuki);
	/*Read the byte containing the button data.*/
	button = serialGetchar(kobuki);
	cout<<"button: "<< button<<endl;

}
