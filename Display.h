/*
  Displaz.h - Library for smart thermostat Display
  Olmo Mezger
*/

#ifndef Display_h
#define Display_h

#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display {
  private:
    Adafruit_SSD1306 _display{OLED_RESET};
    
  public:
    Display();
    void displayIni();
    void printBootDisplayAdafruit();
    void printSreen1(float, float, signed int, signed int);
    

};
// class initialization 
Display::Display(){  // construct DHT instance with expected parameters
  
}

void Display::displayIni(){
  _display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  _display.display();
}

void Display::printBootDisplayAdafruit(){
  
}

void Display::printSreen1(float Temperature, float Humidity,signed int TempTrend, signed int HumTrend){
   // Clear the buffer.
   char tempTrendChar;
   char humTrendChar; 

   switch (TempTrend) {
    case 0:
      tempTrendChar = (char)45;
      break;
    case 1:
      tempTrendChar = (char)24;
      break;
    case -1:
      tempTrendChar = (char)25;
      break;
    default:
      tempTrendChar = (char)45;
   }

   switch (HumTrend) {
    case 0:
      humTrendChar = (char)45;
      break;
    case 1:
      humTrendChar = (char)24;
      break;
    case -1:
      humTrendChar = (char)25;
      break;
    default:
      humTrendChar = (char)45;
   }
  
  _display.clearDisplay();
  // text display tests
  _display.setTextSize(1);
  _display.setTextColor(WHITE);
  _display.setCursor(0,2);
  _display.println("Temperature  Humidity");
  _display.setCursor(0,17);
  _display.setTextSize(2);
  _display.setTextColor(WHITE);
  _display.print("");
  _display.print(Temperature,1);
  _display.print(tempTrendChar);
  _display.print("  ");
  _display.print(Humidity,0);
  _display.print(humTrendChar);
  _display.display();
}


#endif

