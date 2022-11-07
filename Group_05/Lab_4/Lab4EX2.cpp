//use g++ -std=c++11 -o Lab4EX2A Lab4EX2A.cpp -lwiringPi


#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define SONAR_PIN 1 //fill for GPIO 18
#define TIN_MAX_US 18500 //check this units of usec
#define SPEED_SOUND 340 // units of meters/sec

int kobuki;
float read_sonar();
void movement(int, int);

int main(){
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	
	/*Move from a random point within the area designated "X" to the
	point B as shown on the diagram. Use a sonar sensor to navigate through the channel.
	You can reuse your code from Lab 2 and 3*/
	int forward_sp = 200;
	//move forward
	cout<<"\t\t\t\t\t\t\t forward"<<endl;
	float distance = 100;
	while (distance > 25) {
		movement(forward_sp, 0);
		distance = read_sonar();
	}
	//stop
	for(int i=0; i<10; ++i)	movement(0,0);

	//turn right
	cout<<"\t\t\t\t\t\t\t right"<<endl;
	for(int i=0; i<125; ++i) movement(100, -1);

	//stop
	for(int i=0; i<10; ++i)	movement(0,0);

	
	//move forward
	cout<<"\t\t\t\t\t\t\t forward"<<endl;
	distance = 100;
	while (distance > 25) {
		movement(forward_sp, 0);
		distance = read_sonar();
	}
	
	//stop
	for(int i=0; i<10; ++i)	movement(0,0);

	//turn left
	cout<<"\t\t\t\t\t\t\t left"<<endl;
	for(int i=0; i<125; ++i) movement(100, 1);
	for(int i=0; i<10; ++i)	movement(0,0);

	//move forward
	cout<<"\t\t\t\t\t\t\t forward"<<endl;
	distance = read_sonar();
	while (distance>15) {
		movement(forward_sp, 0);
		distance = read_sonar();
	}
	
	//stop
	for(int i=0; i<10; ++i)	movement(0,0);

	/*Note: the Kobuki must completely pass point B as shown to receive full credit*/
	serialClose(kobuki);
	cout<<"kobuki closing"<<endl;
	return 0;

}

float read_sonar(){
	// you can reuse your code from Lab 2
	pinMode(SONAR_PIN, OUTPUT);
	digitalWrite(SONAR_PIN, LOW);
	usleep(2);
	digitalWrite(SONAR_PIN, HIGH);
	usleep(5);
	digitalWrite(SONAR_PIN, LOW);
	
	/*Echo holdoff delay 750 us*/
	usleep(750);
	
	pinMode(SONAR_PIN, INPUT); 
			
	high_resolution_clock::time_point initial =high_resolution_clock::now();		
	int pulse_width=0;

	while(digitalRead(SONAR_PIN))
	{
		// 2. defind a varable to get the current time t2.
		high_resolution_clock::time_point current =high_resolution_clock::now();		
		
		// 3. calculate the time duration: pulse_width=curr_time-initial_time
		pulse_width = chrono::duration_cast<chrono::microseconds>(current-initial).count(); 
		//cout<<" Pulse Width: "<<pulse_width<< endl;
		
		// 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
		if(pulse_width >= TIN_MAX_US) {
			cout<<"Breaking"<<endl;
			break;
		}
	}

	/*Calculate the distance by using the time duration that you just obtained.*/ //Speed of sound is 340m/s
	float distance_cm = (pulse_width/1.00e4 * SPEED_SOUND)/2;  

	/*Print the distance.*/
	cout<<"Distance: "<<distance_cm<<" cm"<< endl; 

	/*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
	usleep(2000);
	return distance_cm;
}
	

void movement(int sp, int r) {

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
	
