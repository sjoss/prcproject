/* 
 * File:   PCR.cpp
 * Author: Joss
 * 
 * Created on 22 septembre 2014, 14:19
 */

#include "PCR.h"
#include "Adafruit_MAX31855.h"
#include <SPI.h>
/* PCR code to cycle temperatures for denaturing, annealing and extending DNA.
Stacey Kuznetsov (stace@cmu.edu) and Matt Mancuso (mcmancuso@gmail.com)
July 2012


*** All temperatures at in degrees Celcius.
*/


/* PCR VARIABLES*/
double DENATURE_TEMP = 94;
double ANNEALING_TEMP = 60.00;
double EXTENSION_TEMP = 72;

// Phase durations in ms. I suggest adding 3-5 seconds to
// the recommended times because it takes a second or two
// for the temps to stabilize
unsigned int DENATURE_TIME = 33000;
unsigned int ANNEALING_TIME= 33000;
unsigned int EXTENSION_TIME = 35000;

// Most protocols suggest having a longer denature time during the first cycle
// and a longer extension time for the final cycle.
unsigned int INITIAL_DENATURE_TIME = 300000;
unsigned int FINAL_EXTENSION_TIME = 600000;

// how many cycles we should do. (most protocols recommend 32-35)
int NUM_CYCLES = 32;  
 
/* Hardware variables */
int heatPin = 7;  // pin that controls the relay w resistors

// Thermocouple pins
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);


int fanPin = 9; // pin for controling the fan
 

//safety vars
short ROOM_TEMP = 18; // if initial temp is below this, we assume thermocouple is diconnected or not working
short MAX_ALLOWED_TEMP = 100; // we never go above 100C
double MAX_HEAT_INCREASE = 2.5; // we never increase the temp by more than 2.5 degrees per 650ms


/* stuff that the program keeps track of */
short CURRENT_CYCLE = 0; // current cycle (kept track by the program)

// current phase. H-> heating up
char CURRENT_PHASE='H'; 

unsigned long time;  // used to track how long program is running
double curTemp; // current temperature

PCR::PCR() {
}

void PCR::run(){
  for (int cycles=0; cycles < NUM_CYCLES; cycles++) {
    CURRENT_CYCLE = cycles;
    unsigned long cycleStartTime = millis();
    Serial.print("///CYCLE  ");
    Serial.print(cycles);
      
    time = millis();
    Serial.println("HEATING UP");
    CURRENT_PHASE='H';
    if(!heatUp(DENATURE_TEMP)){
      // if unable to heat up, stop
      Serial.println("Unable to heat up... something is wrong :(");
      cycles = NUM_CYCLES;
      break;
    }
    
    long dif = millis() - time;
    Serial.print("***TOTAL HEAT TIME ");
    Serial.println(dif);
    Serial.println();
   
    time = millis();
    Serial.println("DENATURATION");
    CURRENT_PHASE='D';
    if(cycles > 0) {
      holdConstantTemp(DENATURE_TIME, DENATURE_TEMP);
    } else {
      // if this is the first cycles, hold denature temp for longer
      holdConstantTemp(INITIAL_DENATURE_TIME, DENATURE_TEMP);
    }
    Serial.println();
  
    Serial.println("COOLING");
    time = millis();
    CURRENT_PHASE='C';
    coolDown((ANNEALING_TEMP));
    dif = millis()-time;
    Serial.print("***TOTAL COOLING TIME ");
    Serial.println(dif);
    Serial.println();
     
    Serial.println("ANNEALING");
    time = millis();
    CURRENT_PHASE='A';
    holdConstantTemp(ANNEALING_TIME, ANNEALING_TEMP);
    dif = millis()-time;
    Serial.print("***TOTAL ANNEALING TIME ");
    Serial.println(dif);
    Serial.println();
    
    
    Serial.println("HEATING UP");
    time =millis();
    CURRENT_PHASE='D';
    heatUp((EXTENSION_TEMP));
    dif = millis()-time;
    Serial.print("***TOTAL HEAT UP TIME IS ");
    Serial.println(dif);
    Serial.println();
  
     
    Serial.println("EXTENSION");
    time = millis();
    CURRENT_PHASE='E';
    if (cycles<(NUM_CYCLES-1)) {
      holdConstantTemp(EXTENSION_TIME, EXTENSION_TEMP);
    } else {
       // if this is the last cycle, hold extension temp for longer
       holdConstantTemp(FINAL_EXTENSION_TIME, EXTENSION_TEMP);
    }
    dif = millis()-time;
    Serial.print("***TOTAL EXTENSION TIME IS ");
    Serial.println(dif);
    Serial.println();
    Serial.println();
    
    Serial.print("///TOTAL CYCLE TIME: ");
    Serial.println(millis()-cycleStartTime);
    Serial.println();
} 
    
  Serial.println("DONE");
}
void PCR::stop(){
   std::cout <<"stop\n";
    
}

