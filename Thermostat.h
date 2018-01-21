/*
  Thermostat.h - Library for smart thermostat
  Olmo Mezger
*/

#ifndef Thermostat_h
#define Thermostat_h

#include "Arduino.h"



class Thermostat {
  private:
    DHT _dht{DHTPIN, DHTTYPE}; // https://stackoverflow.com/a/48367246/1862909
    String _id;
    float _Tset;
    float _dT_hist;
    bool _currentState_Temp = false; // initialize with Heating OFF
    bool _currentState_Time = true; // initialize with Heating OFF
    float _dt_step = 10*60; // the min grid in SECONDS
    

    int _numReadings;
    // the average temperature time is then dt * numReadings in minutes! 
    // dt = 5 and numReadings 12 ==> 1 hour!
    // dt = 2 and numReadings 12 ==>  24 min. 
    
    float _readings[NAVERAGE];      // the readings from the analog input
    int _readIndex;              // the index of the current reading
    float _total;                  // the running total
    float _Taverage;                // the average
    
    float _hum;  //Stores humidity value
    float _hum_prev;  //Stores humidity value
    float _temp; //Stores temperature value
    float _temp_prev; //Stores temperature value

    // PWM Output
    bool _PWM[4];
    int _iPWM;
    int _Period;
    
  public:
    Thermostat();
    void setFilter(int);
    void initializeTemperatureArray();
    void setOutputPWM(bool, bool, bool, bool);
    void doLoop(unsigned long);
    void readDHTValues(void);
    float getTemp();
    float getHum();
    bool currentStatus;
    signed int getTempTrend();
    signed int getHumTrend();

};
// class initialization 
Thermostat::Thermostat(){  // construct DHT instance with expected parameters
  _Tset = TSET;
  _dT_hist = DT_HIST;
}

void Thermostat::initializeTemperatureArray(){
  if (DODEBUG){ Serial.println(F("Initializing Temperature Arraz")); }
  _dht.begin();
  delay(1000);
  if (DODEBUG){ Serial.println(F("DHT initialized. ")); }
  _temp= _dht.readTemperature();
  if (DODEBUG){ 
    Serial.print("Read Temperature: "); 
    Serial.println(_temp);
  }
  
  Serial.print(F("--- Initializing averages: "));
  for (int thisReading = 0; thisReading < _numReadings; thisReading++) {
    _readings[thisReading] = _temp;
    _total = _total + _temp; // create the total at iniliazation 
    if(DODEBUG){
      Serial.print(F("Inserted in array: "));
      Serial.print(_readings[thisReading]);  
    }
  }
  Serial.println(" ");
}

void Thermostat::setFilter(int Naverage){
  _numReadings = Naverage;
  _readings[_numReadings];      // the readings from the analog input
  _readIndex = 0;              // the index of the current reading
  _total = 0;                  // the running total
  _Taverage = 0;                // the average
  initializeTemperatureArray();
}

void Thermostat::setOutputPWM(bool el0, bool el1,bool el2, bool el3){
  // PWM on the output
  _PWM[0] = el0; // sets the period of the loop and the PWM of ON. 
  _PWM[1] = el1; // sets the period of the loop and the PWM of ON. 
  _PWM[2] = el2; // sets the period of the loop and the PWM of ON. 
  _PWM[3] = el3; // sets the period of the loop and the PWM of ON. 
  _Period = sizeof(_PWM ) / sizeof( bool );
  _iPWM = 0;
  if(DODEBUG){
    Serial.println("Debuging PWM array");
    for (int i = 0; i<4;i++){
      Serial.print(_PWM[i]);
      Serial.print(F(" ,"));
      
    }
    Serial.println("");
  }
  pinMode(RELAISPIN, OUTPUT);
}

void Thermostat::doLoop(unsigned long mytime){
  readDHTValues();
  // first, let's check if we are in PWM and can be theoretically ON:
  _currentState_Time = _PWM[_iPWM];
  
  //
  // subtract the last reading:
  _total = _total - _readings[_readIndex];
  // read from the sensor:
  _readings[_readIndex] = _temp;
  // add the reading to the total:
  _total = _total + _readings[_readIndex];
  // advance to the next position in the array:
  _readIndex = _readIndex + 1;
  // if we're at the end of the array...
  if (_readIndex >= _numReadings) {
    // ...wrap around to the beginning:
    _readIndex = 0;
  }
  // calculate the average:
  _Taverage = _total / _numReadings;
  
  if (!_currentState_Temp){
    // heating is OFF.
    if (_Taverage<= _Tset-(_dT_hist/2)){
      // heating was off and is getting ON. 
      _currentState_Temp = HIGH; // turn heating on. 
    }else{
      // heating is OFF and temperature is above Tset
    }
  }else{
    // heating is ON
    if (_temp >= _Tset + (_dT_hist/2)){
      // if going up, let's dont ake the average into account but rather the actual temp. 
      // so are we faster.
      _currentState_Temp = LOW; // turn OFF
    }else{
      // heating is ON, and temp is below Tset
    }
  }
  if(DODEBUG){
    Serial.print(mytime);
    Serial.print(",");
    Serial.print(_iPWM);
    Serial.print(",");
    Serial.print(_temp);
    Serial.print(",");
    Serial.print(_Taverage);
    Serial.print(",");
    Serial.print(_currentState_Time);
    Serial.print(",");
    Serial.print(_currentState_Temp);
    Serial.println("");
  }
  
  

  _iPWM = _iPWM + 1;
  if (_iPWM >= _Period){
    // overflow
    _iPWM = 0;
  }
  
  bool _currentState = _currentState_Time * _currentState_Temp;
  
  digitalWrite(RELAISPIN, _currentState);
}

void Thermostat::readDHTValues(){
  _temp_prev = _temp;
  _temp= _dht.readTemperature();
  _hum_prev = _hum;
  _hum = _dht.readHumidity();
}

float Thermostat::getTemp(){
  return _temp;
}
float Thermostat::getHum(){
  return _hum;
}
signed int Thermostat::getTempTrend(){
  if (_temp_prev == _temp){
    return 0;
  }else{
     if (_temp_prev < _temp){
      return 1; // going up
    }else{
      return -1; // going down 
    } 
  }
  
}

signed int Thermostat::getHumTrend(){
  if (_hum_prev == _hum){
    return 0;
  }else{
     if (_hum_prev < _hum){
      return 1; // going up
    }else{
      return -1; // going down 
    } 
  } 
}

#endif

