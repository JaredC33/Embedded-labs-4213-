//Use g++ -std=c++11 -o Lab2EX1 Lab2EX1.cpp -lwiringPi

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define SIG_PIN 1 //fill for GPIO 18
#define TIN_MAX_US 18500 //check this units of usec
#define SPEED_SOUND 340 // units of meters/sec

int main(){
	//Set up wiringPi
	wiringPiSetup();
	
	
	
	while(true){
		/*Set the pinMode to output and generate a LOW-HIGH-LOW signal using "digitalWrite" to trigger the sensor. 
		Use a 2 us delay between a LOW-HIGH and then a 5 us delay between HIGH-LOW. You can use
		the function "usleep" to set the delay. The unit of usleep is microsecond. */
		pinMode(SIG_PIN, OUTPUT);
		digitalWrite(SIG_PIN, LOW);
		usleep(2);
		digitalWrite(SIG_PIN, HIGH);
		usleep(5);
		digitalWrite(SIG_PIN, LOW);

		/*Echo holdoff delay 750 us*/
		usleep(750);


		/*Switch the pinMode to input*/ 
		pinMode(SIG_PIN, INPUT);
		
		
		/*Get the time it takes for signal to leave sensor and come back.*/

		// 1. defind a varable to get the current time t1. Refer to "High_Resolution_Clock_Reference.pdf" for more information
		high_resolution_clock::time_point initial =high_resolution_clock::now();		
		int pulse_width=0;
		while(digitalRead(SIG_PIN))/*read signal pin, the stay is the loop when the signal pin is high*/
		{
			
			// 2. defind a varable to get the current time t2.
			high_resolution_clock::time_point current =high_resolution_clock::now();		
			
			// 3. calculate the time duration: pulse_width=curr_time-initial_time
			pulse_width = chrono::duration_cast<chrono::microseconds>(current-initial).count(); 
			//cout<<" Pulse Width: "<<pulse_width<< endl;
			
			// 4. if the duration is larger than the Pulse Maxium 18.5ms, break the loop.
			if(pulse_width>=TIN_MAX_US) {
				//cout<<"Breaking"<<endl;
				break;
			}
		}



		/*Calculate the distance by using the time duration that you just obtained.*/ //Speed of sound is 340m/s
		double distance_cm = (pulse_width/1.00e4 * SPEED_SOUND)/2;  

		/*Print the distance.*/
		//if(pulse_width) 
		cout<<"Distance: "<<distance_cm<<" cm"<< endl; 

		/*Delay before next measurement. The actual delay may be a little longer than what is shown is the datasheet.*/
		usleep(2000);

        }
        
}
