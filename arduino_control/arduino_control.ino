#include <Adafruit_NeoPixel.h>
#include "NI2C.h"
#include <Wire.h>
#include "OneButton.h"
//#include "NeoPatterns.cpp"
#include "NixieControl.cpp"

// PINS
#define BUTTON_LED_PIN 2
#define SWITCH_PIN 3  
#define BUTTON_PIN 4
#define HIGH_VOLTAGE_EN_PIN 5
#define MATRIX_PIN 6
#define MATRIX_COUNT 9

void ButtonLedComplete();
void MatrixComplete();
void NixieComplete();

NixieControl nixieControl(&NixieComplete, 5);

OneButton mainSwitch(SWITCH_PIN, false);
OneButton button1(BUTTON_PIN, false);

NeoPatterns matrix(MATRIX_COUNT, MATRIX_PIN, NEO_GRB + NEO_KHZ800, &MatrixComplete);
NeoPatterns buttonLed(1, BUTTON_LED_PIN, NEO_RGB + NEO_KHZ400, &ButtonLedComplete);

//Serial
char received_command_data_buffer[26];
int number_of_bytes_received;

unsigned long lastButtonUpdate;

void setup() {  
  pinMode(HIGH_VOLTAGE_EN_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);   
  
  Serial.begin(9600);    
  
  //Serial.setTimeout(100);
  
  nixieControl.begin();
  matrix.begin();  
  buttonLed.begin();

  //nixieControl.SimpleCounter(100);
  nixieControl.Whipe(400);
  
  
  buttonLed.Color1 = buttonLed.Color(255,200,10); //"dirty" green
  buttonLed.ColorSet(buttonLed.Color1);
  
  buttonLed.setBrightness(255); 
  buttonLed.show();  
  
  matrix.Color1 = matrix.Color(255,200,10); //"dirty" green
  matrix.Color2 = matrix.Color(102, 255, 255);   
  matrix.TheaterChase(matrix.Color1, matrix.Color2, 500);
  
  //nixie.debug(true);
   
  button1.attachClick(click1);
  button1.attachDoubleClick(doubleclick1);
  button1.attachLongPressStart(longPressStart1);  

  mainSwitch.attachLongPressStart(switchLongPressStart1);
  mainSwitch.attachLongPressStop(switchLongPressStop1);
}

void loop() {         
       
   if(Serial.available() > 0) {     
    number_of_bytes_received = Serial.readBytesUntil(10,received_command_data_buffer,25); //10 \n (NL), 13 \r (CR)
    received_command_data_buffer[number_of_bytes_received] = 0; // add a 0 terminator to the char array   
    handleCommand();
    delay(1);    
  }
  
  nixieControl.Update();
  matrix.Update(); 
  buttonLed.Update();  

  if((millis() - lastButtonUpdate) > 100) {
    lastButtonUpdate = millis();
    button1.tick();
    mainSwitch.tick();  
    digitalWrite(HIGH_VOLTAGE_EN_PIN, nixieControl.hv_power);          
  }           
}

void log(const char* format, ...) {
  char outBuffer[100];
  va_list(args);
  va_start(args, format);
  vsprintf(outBuffer, format, args);
  va_end(args);  
  Serial.println(outBuffer);  
}

void MatrixComplete() {
  
}

void NixieComplete() {
  
}




