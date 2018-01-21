
#include <DHT.h>
#include "config.h"
#include "Thermostat.h"

#include "Display.h"

Display display;

float dt_step = DT_STEP; // the min grid in SECONDS
 
Thermostat myThermostat;


unsigned long mytime;


void setup() {
  Serial.begin(250000);
  Serial.println("Initializing");
  // put your setup code here, to run once:
  myThermostat.setFilter(NAVERAGE);
  // the average temperature time is then dt * numReadings in minutes! 
  // dt = 5 and numReadings 12 ==> 1 hour!
  // dt = 2 and numReadings 12 ==>  24 min. 
  
  myThermostat.setOutputPWM(1,1,0,0);
  display.displayIni();
  delay(1000);
}

void loop() {
  if(DODEBUG){Serial.println("########### .  outer loop");}
  mytime = millis();
  //Read data and store it to variables hum and temp
  myThermostat.doLoop(mytime);
  display.printSreen1(myThermostat.getTemp(),myThermostat.getHum(),myThermostat.getTempTrend(),myThermostat.getHumTrend());
  delay(dt_step*1000);
  //delay(5000);
}

/*
 * //Print temp and humidity values to serial monitor
 * Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    */


