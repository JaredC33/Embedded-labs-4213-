//Use g++ -std=c++11 -o Lab4EX1 Lab4EX1.cpp -lwiringPi

#include <string>
#include <iostream>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>
using namespace std;

int kobuki;

int main(){
	wiringPiSetup();
	kobuki = serialOpen("/dev/kobuki", 115200);
	unsigned int bumper;
	unsigned int drop;
	unsigned int cliff;
	unsigned int button;
	unsigned int read;

	while(serialDataAvail(kobuki) != -1){

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
			and cliff sensors. You can convert them into a usable data type.*/
		char bumper = serialGetchar(kobuki);
		char wheel_drop = serialGetchar(kobuki);
		char cliff = serialGetchar(kobuki);

		/*Print the data to the screen.*/
		string bumper_s, wdrop_s, cliff_s;
		if (bumper == 1) bumper_s = "right bumper";
		else if (bumper == 2) bumper_s = "center bumper";
		else if (bumper == 4) bumper_s = "left bumper";
		
		if (wheel_drop == 1) wdrop_s = "right wheel";
		if (wheel_drop == 2) wdrop_s = "left wheel";
		
		if (cliff == 1) cliff_s = "right cliff sensor";
		if (cliff == 2) cliff_s = "center cliff sensor";
		if (cliff == 4) cliff_s = "left cliff sensor";

		cout << "Bumper: "<< bumper_s << " Wheel drop: "<<wdrop_s << " Cliff sensor: "<< cliff_s<<endl;

		/*Read through 6 bytes between the cliff sensors and
		the button sensors.*/
		for (int i = 0; i<6; i++) serialGetchar(kobuki);
		/*Read the byte containing the button data.*/
		char button = serialGetchar(kobuki);
		/*Close the script and the connection to the Kobuki when
		Button 1 on the Kobuki is pressed. Use serialClose(kobuki);*/
		if (button == 2) {
			serialClose(kobuki);
			cout << "kobuki closing..."<<endl;
		}

		//Pause the script so the data read receive rate is the same as the Kobuki send rate.
		usleep(20000);
			
		
	}

	return(0);
}
