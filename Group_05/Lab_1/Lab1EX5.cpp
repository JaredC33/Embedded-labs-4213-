// g++ -std=c++11 -o Lab1EX5 Lab1EX5.cpp -lwiringPi

#include <wiringPi.h>
#include <softPwm.h>
#include <stdio.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <iostream>
#include <softPwm.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>
#include <signal.h>
#include <iomanip>
#include <unistd.h>


#define SERVO_MIN_MS 5
#define SERVO_MAX_MS 25
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
using namespace std;
bool clockwise=true;

/* signal pin of the servo*/
#define servoPin  1 
#define BUTTONPIN  4

//Specific a certain rotation angle (0-180) for the servo
void servoWrite(int pin, double angle){ 
    long time = 0;
    time = 10 * (0.5 + angle/90);
    softPwmWrite(pin,time);   
}


/* Sefind your callback function to handout the pressing button interrupts. */
void press_button()
{
    if (digitalRead(BUTTONPIN) == HIGH)   clockwise=(!clockwise);
}

//This function is used to read data from ADS1015
int adcVal(){

	uint16_t low, high, value;
	// Refer to the supplemental documents to find the parameters. In this lab, the ADS1015
	// needs to be set in single-end mode, FSR (full-scale range)is 6.144, you can choose 
	// any input pin (A0, A1, A2, A3) you like.
	int adc = wiringPiI2CSetup(0x48);/*Address of ADS1015(HEX)*/
	wiringPiI2CWriteReg16(adc, 0x01 /*Configuration Register address pointer(HEX)*/, 0xC5C1 /*Configuration Register*/);
	usleep(1000);
    uint16_t data = wiringPiI2CReadReg16(adc, 0x00 /*Conversion Register address pointer(HEX)*/);


    low = (data & 0xFF00) >> 8;
    high = (data & 0x00FF) << 8;
    value = (high | low)>>4;
	return value;

}///c5c1

//0-1740

int conv_adc_angle(int adc) {
    //int adc_min=0;
    int adc_max=1735;
    int angle= SERVO_MIN_ANGLE + SERVO_MAX_ANGLE * adc/(adc_max);
    return angle;
}

int main(void)
{
    wiringPiSetup();    
    pinMode(servoPin, OUTPUT);
    softPwmCreate(servoPin,  0, 200);
    wiringPiISR(BUTTONPIN, INT_EDGE_RISING, &press_button);

    /* Use wiringPiISR() to setup your interrupts. Refer to document WiringPi_ Interrupts.pdf. */


    int angle;
    int adc_val;

    while(1){

        /* read ADS1015 value */
        adc_val = adcVal();

        /* convert the obtained ADS1015 value to angle 0 - 180*/
        if (clockwise) angle = conv_adc_angle(adc_val);
        else angle = 180 - conv_adc_angle(adc_val);
        
        cout<< "ADC val: "<<adc_val<<" angle: "<<angle <<endl;

        /* use the angle to control the servo motor*/
        servoWrite (servoPin, angle);

        usleep(100000);

    }
    return 0;
}


