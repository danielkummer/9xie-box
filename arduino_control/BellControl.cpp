#include <Arduino.h>

class BellControl {
  public:

    uint8_t Pin;
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    uint16_t Index;  // current step within the pattern   
    uint16_t TotalSteps = 0;  // total number of steps in the pattern

    boolean isOn = false;
    
    void (*OnComplete)();  // Callback on completion of pattern

   // Constructor - calls base-class constructor to initialize strip
    BellControl(uint8_t pin, void (*callback)()) {
        OnComplete = callback;
        Pin = pin;        
        pinMode(Pin, OUTPUT);   
    }

    void Ring(uint8_t times, uint16_t interval) {
      Interval = interval;
      TotalSteps = times;
    }
    
    void Update() {      
        if((millis() - lastUpdate) > Interval) {
          if(TotalSteps > 0) {
            lastUpdate = millis();            
              if(isOn) {                
                isOn = false;
              } else {                
                isOn = true;
                digitalWrite(Pin, isOn);
                Increment();
              }              
          }            
        }
    }

    // Increment the Index and reset at the end
    void Increment() {        
           Index++;
           if (Index >= TotalSteps) {
                Index = 0;               
                TotalSteps = 0; 
                digitalWrite(Pin, 0); //ensure off position
                if (OnComplete != NULL) {                  
                    OnComplete(); 
                }
            }        
    }

   
};

