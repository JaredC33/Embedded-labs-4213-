//use g++ -std=c++11 -o Lab3EX1 Lab3EX1.cpp -lwiringPi

#include <iostream>
#include <unistd.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <chrono>
using namespace std;
using namespace std::chrono;

void movement(int, int);
int kobuki;

int main(){
	wiringPiSetup();
	
	kobuki = serialOpen("/dev/kobuki", 115200); 

	//The Kobuki accepts data packets at a rate of 20 ms.
	//To continually move, data needs to be sent continuously. Therefore, 
	//you need to call void movement(int sp, int r) in a for or while loop 
	//in order to run a specific distance.
	
	//Due to machine error, the calculated value of the time needed
	//will not be exact, but can give you a rough starting value.


	/*Rotate the Kobuki 90 degrees*/
		
		
	/*Move along the vertical side*/
	//move 50cm forward

	double speed=10; //unit of mm/sec
	
	duration<double> time_max= duration<double>(500/speed);	//time_max=distance(mm)/speed(mm/sec)
	high_resolution_clock::time_point start_time= high_resolution_clock::now();
	high_resolution_clock::time_point curr_time = high_resolution_clock::now();
	
	duration<double> time_span= duration_cast<duration<double>>(curr_time-start_time);
	
	while (time_span<time_max) {
		movement((int)speed, 0);
		curr_time = high_resolution_clock::now();
		time_span= duration_cast<duration<double>>(curr_time-start_time);
	}

	/*Move along quarter circle*/



	//Close the serial stream to the Kobuki
	//Not doing this will result in an unclean disconnect
	//resulting in the need to restart the Kobuki
	//after every run.
	serialClose(kobuki);
	return(0);
}

void movement(int sp, int r){
	//Create the byte stream packet with the following format:
	unsigned char b_0 = 0xAA; /*Byte 0: Kobuki Header 0*/
	unsigned char b_1 = 0x55; /*Byte 1: Kobuki Header 1*/
	unsigned char b_2 = 0x04; /*Byte 2: Length of Payload*/
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
	
	char send[]={b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8, checksum};
	cout<<send<<endl;
	for (unsigned int i = 0; i < 9; i++)
		serialPutchar(kobuki, send[i]);
	cout<<"sent data"<<endl;

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);
}

