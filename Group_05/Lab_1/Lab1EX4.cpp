// g++ -std=c++11 -o Lab1EX4 Lab1EX4.cpp -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#define SERVO_MIN_MS 5
#define SERVO_MAX_MS 25
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

/* signal pin of the servo*/
#define SERVOPIN 1

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, int angle){ 
    double time_in_ms = 0;
    /* map the desired angle to time*/
    time_in_ms = 0.5 + (double)angle / 90; 
    softPwmWrite(pin, (int)10*time_in_ms);   
}

int main(void)
{
    wiringPiSetup();    
    softPwmCreate(SERVOPIN,  0, 200);

    cout<< "initializing... 0 degrees"<<endl;
    servoWrite(SERVOPIN, SERVO_MIN_ANGLE);
    delay(500);



    while(1){

        /*make servo rotate from minimum angle to maximum, use the function 
        servoWrite(int pin, int angle), increase 1 degree each time*/
        for (int i=SERVO_MIN_ANGLE; i<=SERVO_MAX_ANGLE; i++) {
            servoWrite(SERVOPIN, i);
            delay(10);
            if (i%10==0) cout << "reached " <<i<< " deg"<<endl;

        }
        delay(500);
        cout << "reached 180 deg"<<endl;

        /*make servo rotate from maximum angle to minimum, use the function 
        servoWrite(int pin, int angle), increase 1 degree each time*/

        for (int i=SERVO_MAX_ANGLE; i>=SERVO_MIN_ANGLE; i--) {
            servoWrite(SERVOPIN, i);
            delay(10);
            if (i%10==0) cout << "reached " <<i<< " deg"<<endl;
        }
        
        cout << "reached 0 deg"<<endl;
        delay(500);
    }
    return 0;
}

