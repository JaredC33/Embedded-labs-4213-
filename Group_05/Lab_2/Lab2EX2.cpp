//Use g++ -std=c++11 -o Lab2EX2 Lab2EX2.cpp -lwiringPi

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <chrono>
#include <cmath>

using namespace std::chrono;
using namespace std;

// functions
void sigroutine(int);
int adcVal();
void PID(float, float, float);
float read_potentionmeter();
float read_sonar();


// variables
float distance_previous_error, distance_error;
float obj_value, measured_value; // potentionmeter reading, sonar reading
int adc;
float PID_p,  PID_d, PID_total, PID_i = 0;
int time_inter_ms = 23; // time interval, you can use different time interval

/*set your pin numbers and pid values*/
#define MOTOR_PIN 26
#define SONAR_PIN 1
#define TIN_MAX_US 18500 //check this units of usec
#define SPEED_SOUND 340 // units of meters/sec

float kp= 12.8; 
float kd= 5500;
float ki= .001;

//float kp= 5; 
//float kd= 30;
//float ki= 0.01; 

int main(){
	wiringPiSetup();
    adc = wiringPiI2CSetup(0x48);

    /*Set the pinMode (fan pin)*/
    pinMode(MOTOR_PIN, PWM_OUTPUT);
    
    // This part is to set a system timer, the function "sigroutine" will be triggered  
    // every time_inter_ms milliseconds. 
    struct itimerval value, ovalue;
    signal(SIGALRM, sigroutine);
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = time_inter_ms*1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = time_inter_ms*1000;
    setitimer(ITIMER_REAL, &value, &ovalue);    

	while(true){
        cout<<"obj_value: "<<obj_value<<" measured_value: "<<measured_value<<endl;
        cout<<"PID_p: "<<PID_p<<endl;
        cout<<"PID_i: "<<PID_i<<endl;
        cout<<"PID_d: "<<PID_d<<endl;
        cout<<"PID_total: "<<PID_total<<endl;
        delay(20);
	}
}


void sigroutine(int signo){
    PID(kp, ki, kd);
    return;
}


/* based on the obj distance and measured distance, implement a PID control algorithm to 
the speed of the fan so that the Ping-Pang ball can stay in the obj position*/
void PID(float kp, float ki, float kd){
    
    /*read the objective position/distance of the ball*/
    obj_value=read_potentionmeter();
    
    /*read the measured position/distance of the ball*/
    measured_value=read_sonar();

    /*calculate the distance error between the obj and measured distance */
    distance_error = obj_value - measured_value; //SHOULD THIS BE PERCENTAGE?

    /*calculate the proportional, integral and derivative output */
    PID_p = kp*distance_error;
    PID_i = ki*(distance_error*time_inter_ms);
    PID_d = kd* (distance_error-distance_previous_error)/time_inter_ms;
    PID_total = PID_p + PID_d + PID_i; 

    /*assign distance_error to distance_previous_error*/
    distance_previous_error=distance_error;

    /*use PID_total to control your fan*/
    cout<<"PID Total: "<<PID_total<<endl;
    int pwm;
    pwm=(int)PID_total;
    //if (PID_total>1024) pwm=1024;
    //if (PID_total<0) pwm=0;
    
    pwmWrite(MOTOR_PIN, pwm);
}


/* use a sonar sensor to measure the position of the Ping-Pang ball. you may reuse
your code in EX1.*/
float read_sonar()
{
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
			if(pulse_width>=TIN_MAX_US) {
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

/* use a potentiometer to set an objective position (10 - 90 cm) of the Ping-Pang ball, varying the potentiometer
can change the objective distance. you may reuse your code in Lab 1.*/
float read_potentionmeter()
{
    int adc_max=1735;
    int adc_val=adcVal();
    cout<<"adc value: "<<adc_val<<endl;
    float minD = 10;
    float maxD = 90;
    float pot_distance=(maxD-minD)/adc_max * adc_val + minD;
    return pot_distance;
}



int adcVal(){
	uint16_t low, high, value;
	wiringPiI2CWriteReg16(adc, 0x01,  0xC5C1);
	usleep(1000);
    uint16_t data = wiringPiI2CReadReg16(adc,  0x00);
    low = (data & 0xFF00) >> 8;
    high = (data & 0x00FF) << 8;
    value = (high | low)>>4;
	return value;
}
