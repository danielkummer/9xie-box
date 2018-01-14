uint32_t pushColor = buttonLed.Color(102, 255, 255); //light blue
uint32_t defaultColor = buttonLed.Color(255,200,10); //"dirty" green
//button pulse
short remainingPulseCount = 0; 

void click1() {
  publishButton("CLICK");
  buttonLed.Pulse(pushColor, 10, 100);   
} 

// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
  publishButton("DOUBLECLICK");
  remainingPulseCount++;  
  buttonLed.Pulse(pushColor, 10, 100);     
} 


// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() {
  publishButton("LONGPRESS");
  buttonLed.Pulse(pushColor, 10, 500);     
} 


// This function will be called once, when the button1 is pressed for a long time.
void switchLongPressStart1() {
  publishSwitch(1);
} 

// This function will be called once, when the button1 is released after beeing pressed for a long time.
void switchLongPressStop1() {
  publishSwitch(0);
} 

void publishButton(char* value) {
  char buttonBuffer[15];
  sprintf(buttonBuffer, "BT=%s", value);
  Serial.println(buttonBuffer);
}

void publishSwitch(int value) {
  nixieControl.hv_power = value;
  //turn off leds
  if(value == 0) {
    matrix.setBrightness(0);
    buttonLed.setBrightness(0);  
  } else {
    matrix.setBrightness(255);
    buttonLed.setBrightness(255);  
  }
  
  char switchBuffer[5];
  sprintf(switchBuffer, "SW=%1d", value);
  Serial.println(switchBuffer);
}

void ButtonLedComplete() {    
  if(remainingPulseCount > 0) {
    remainingPulseCount = remainingPulseCount - 1;    
    buttonLed.Pulse(pushColor, buttonLed.Interval, buttonLed.TotalSteps);    
  } else {
    //reset to default
    if(buttonLed.ActivePattern == FADE) {
      buttonLed.ActivePattern = NONE;  
    } else {
      buttonLed.Color1 = defaultColor;
      buttonLed.Fade(buttonLed.Color(0, 0, 0), defaultColor, 100, 40);      
    }
  }
}

