//The stator in the Stepper Motor we have supplied has 32 magnetic poles. Therefore, to complete
// one full revolution requires 32 full steps. The rotor (or output shaft) of the Stepper 
//Motor is connected to a speed reduction set of gears and the reduction ratio is 1:64. Therefore, 
//the final output shaft (exiting the Stepper Motor’s housing) requi res 32 X 64 = 2048 
//steps to make one full revolution.

// g++ -std=c++11 -o Lab1EX3 Lab1EX3.cpp -lwiringPi
#include <stdio.h>
#include <wiringPi.h>
#include <iostream>
using namespace std;


#define min_delay_ms 3
#define IN1 1
#define IN2 4
#define IN3 5
#define IN4 6
#define CWISE 1
#define CCWISE 0


//recieves the pin number that should be high, sets that pin to high and all other pins to low.
void SetState(int high_pin) {
    digitalWrite(IN1, (IN1==high_pin));
    digitalWrite(IN2, (IN2==high_pin));
    digitalWrite(IN3, (IN3==high_pin));
    digitalWrite(IN4, (IN4==high_pin));
}

void moveOnePeriod(int dir){
    if(dir == 1){
        /* clockwise, there are four steps in one period, set a delay after each step*/
        SetState(IN1);
        delay(min_delay_ms);
        SetState(IN2);
        delay(min_delay_ms);
        SetState(IN3);
        delay(min_delay_ms);
        SetState(IN4);
        delay(min_delay_ms);

    }
    else{
        /* anticlockwise, there are four steps in one period, set a delay after each step*/
        SetState(IN4);
        delay(min_delay_ms);
        SetState(IN3);
        delay(min_delay_ms);
        SetState(IN2);
        delay(min_delay_ms);
        SetState(IN1);
        delay(min_delay_ms);
    }
}
//continuous rotation function, the parameter steps specifies the rotation cycles, every four steps is a cycle
void moveCycles(int dir,int cycles){
    int i;
    for(i=0;i<cycles;i++){
        moveOnePeriod(dir);
    }
}

int main(void){
    wiringPiSetup();
    //wiringPiSetupGpio();
    /* set the pin mode*/
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    while(1){
        /*rotating 360° clockwise, a total of 2048 steps in one full revolution, namely, 512 cycles.
        use function moveCycles(int dir,int cycles)*/
        delay(500);
        cout << "rotating clockwise"<<endl;
        moveCycles(CWISE, 512);
        /*rotating 360° anticlockwise, a total of 2048 steps in one full revolution, namely, 512 cycles.
        use function moveCycles(int dir,int cycles)*/
        delay(500);
        cout << "rotating counterclockwise"<<endl;
        moveCycles(CCWISE, 512);

    }
    return 0;
}

