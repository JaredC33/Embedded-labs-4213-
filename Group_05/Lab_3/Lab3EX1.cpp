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

void test() {
	int r, sp, trans_sp;
/*Rotate the Kobuki 90 degrees*/
	for(int i=0; i<12; i++){
		sp = -100;
		r=1; 
		movement(sp, r);
		delay(200);
	}
	delay(4000);
		
	/*Move along the vertical side*/
	for(int j=0; j<150; j++){
		sp=-100;
		r=0; 
		movement(sp, r);
		delay(200);

	} 
	delay(4000);

	/*Move along quarter circle*/
	for(int i=0; i<10; i++){
		r=500;
		sp=100;
		trans_sp=(sp*(r+0.5*230))/r;
		movement(trans_sp, 500);

		delay(200);
	}

}
int main(){
	wiringPiSetup();
	
	kobuki = serialOpen("/dev/kobuki", 115200); 

	//The Kobuki accepts data packets at a rate of 20 ms.
	//To continually move, data needs to be sent continuously. Therefore, 
	//you need to call void movement(int sp, int r) in a for or while loop 
	//in order to run a specific distance.
	
	//Due to machine error, the calculated value of the time needed
	//will not be exact, but can give you a rough starting value.
		
	test();

	/*Rotate the Kobuki 90 degrees*/
	for(int i=0; i<12; i++){
		int sp = -100;
		int r=100; 
		movement(sp, r);
		delay(200);
	}
	delay(4000);
		
	/*Move along the vertical side*/
	for(int j=0; j<200; j++){

		int sp=-100;
		int r=-0; 
		 movement(sp, r);

	} 
	delay(4000);

	/*Move along quarter circle*/
	for(int i=0; i<10; i++){
		for(int i=0; i<2; i++){
		//turn
		 movement(-100, 100);
		delay(100);
		}
		for(int i=0; i<9; i++){
		//forward
		movement(-100, 0);
		delay(100);
		}
	}



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
		cout<<"checksum: "<<checksum<<endl;
	/*Send the data (Byte 1 - Byte 9) to Kobuki using serialPutchar (kobuki, );*/
	
	char send[]={b_0,b_1,b_2,b_3,b_4,b_5,b_6,b_7,b_8,};
	cout<<send<<endl;//////why is this ?
	for (unsigned int i = 0; i < 9; i++)
		serialPutchar(kobuki, send[i]);
	//cout<<"sent data"<<endl;

	/*Pause the script so the data send rate is the
	same as the Kobuki data receive rate*/
	usleep(20000);
}

