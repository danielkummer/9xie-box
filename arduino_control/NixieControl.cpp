#include "NI2C.h"
#include "NeoPatterns.cpp"

static const char *MODE_STRING[] = {"DIRECT_DISPLAY", "SCROLL", "TRIPPLE_COUNTER", "SIMPLE_COUNTER", "PULSE_ON_UPDATE", "WHIPE"};
enum  nixieMode { DIRECT_DISPLAY, SCROLL, TRIPPLE_COUNTER, SIMPLE_COUNTER, PULSE_ON_UPDATE, WHIPE };


class NixieControl : public NI2C {
  public:

    nixieMode Mode;
    int hv_power;

    char nixie_buffer[10];
    char lastBufferValue[10];   

    int counter1 = 0;
    int counter2 = 0;
    int counter3 = 0;

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    uint16_t Index;  // current step within the pattern
    direction Direction;     // direction to run the pattern
    uint16_t TotalSteps = 255;  // total number of steps in the pattern

    char whipePattern[5][10] = {"1        ", " 2 2     ", "  3 3 3  ", "     2 2 ", "        +"};
    /*
    char starPattern[2][10] = {" 0  0  0 ", "0 0 0 0 o"};
    char squarePattern[5][10] = {"0000 0000", "    0    "};
    "11 1 1 11"
    
    */
    /*
     * 1 2 3
     * 4 5 6
     * 7 8 9
     */    

    void (*OnComplete)();  // Callback on completion of pattern

   // Constructor - calls base-class constructor to initialize strip
    NixieControl(void (*callback)(), const uint8_t numberOfModules = 1) :NI2C(numberOfModules, 0x38) {
        OnComplete = callback;
    }
    
    void Update() {
        if((millis() - lastUpdate) > Interval) {
            lastUpdate = millis();
            switch(Mode) {
                case DIRECT_DISPLAY:
                    DisplayUpdate();
                    break;
                case TRIPPLE_COUNTER:
                    TrippleCounterUpdate();
                    break;
                case SIMPLE_COUNTER:
                    SimpleCounterUpdate();
                    break;
                case PULSE_ON_UPDATE:
                    PulseOnUpdateUpdate();
                    break;
                case WHIPE:
                    WhipeUpdate();
                    break;
                case SCROLL:
                    ScrollUpdate();
                    break;
                default:
                    break;
            }
        }
    }

    // Increment the Index and reset at the end
    void Increment() {
        if (Direction == FORWARD) {
           Index++;
           if (Index >= TotalSteps) {
                Index = 0;                
                if (OnComplete != NULL) {
                    OnComplete(); 
                }
            }
        } else {
            --Index;
            if (Index <= 0) {
                Index = TotalSteps-1;
                if (OnComplete != NULL) {
                    OnComplete();
                }
            }
        }
    }

    void Display(uint8_t interval) {
      Mode = DIRECT_DISPLAY;                    
      Interval = interval;     
    }

    void DisplayUpdate() {      
      write(nixie_buffer);
    }

    void Scroll(uint8_t interval, direction dir = FORWARD) {
      Mode = SCROLL;        
      Direction = dir;            
      Interval = interval;  
    }

    void ScrollUpdate() {      
      rotate(nixie_buffer, 1);    
      write(nixie_buffer);              
    }

    void TrippleCounter(uint8_t interval, direction dir = FORWARD) {
      Mode = TRIPPLE_COUNTER;        
      Direction = dir;            
      Interval = interval;
      counter1 = 0;
      counter2 = 0;
      counter3 = 0; 
    }

    void TrippleCounterUpdate() {               
      char counterBuffer[10];
  
      counter1++;
      if(counter1 > 99) {
        counter2++;
      }
      if(counter2 > 99) {
        counter3++;
      }    
      if(counter1 >= 999 ) {
        counter1 = 0;
        counter2 = 0;
        counter3 = 0;
      }        
      sprintf(counterBuffer, "%03d%03d%03d", counter1, counter2, counter3);   
      write(counterBuffer);                             
    }

    void SimpleCounter(uint8_t interval, uint16_t index = 0, direction dir = FORWARD) {
      Mode = SIMPLE_COUNTER;        
      Direction = dir;            
      Interval = interval; 
      Index = index; 
    }

    void SimpleCounterUpdate() {      
      sprintf(nixie_buffer, "%09d", Index);
      write(nixie_buffer);    
      Increment();          
    }

    void PulseOnUpdate(uint8_t interval, uint16_t offCount = 5) {
      Mode = PULSE_ON_UPDATE;                  
      Interval = interval;
      Direction = REVERSE;
      Index = 0; 
      TotalSteps = offCount;
    }

    //todo broken!
    void PulseOnUpdateUpdate() {            
      if (Index >= 1) {
        write("         ");
        Increment();
      } else {
        write(nixie_buffer);    
      }                         
      if(strcmp(nixie_buffer, lastBufferValue) != 0) {
        sprintf(lastBufferValue, "%s", nixie_buffer);
        Index = TotalSteps + 1;             
      } 
    }

    void Whipe(uint8_t interval = 2000, direction dir = FORWARD) {
      Mode = WHIPE;        
      Direction = dir;            
      Interval = interval;       
    }

    void WhipeUpdate() {      
      write(whipePattern[Index % 5]);
      Increment();
    }

   
    void writeNumber(long number, char* symbols) {
      if(number > 999999 || strlen(symbols) != 3) {
        return;
      }
      char numberString[7];
      sprintf(numberString,"%0d",number);  
    
      //optimize
      nixie_buffer[0] = numberString[0];
      nixie_buffer[1] = numberString[1];
      nixie_buffer[2] = symbols[0];
      nixie_buffer[3] = numberString[1];
      nixie_buffer[4] = numberString[1];
      nixie_buffer[5] = symbols[1];
      nixie_buffer[6] = numberString[1];
      nixie_buffer[7] = numberString[1];
      nixie_buffer[8] = symbols[2];    
    }
    
    void writeRow(short number, char symbol, short row) {
      if(number > 99) {
        return;
      }
      short startIdx = row*3;
      char numberString[3];
      sprintf(numberString,"%d",number);  
      strncpy(&nixie_buffer[startIdx], numberString,2);
      nixie_buffer[startIdx+2] = symbol;  
    }
    
    void reverse_string(char* str, int left, int right) {
      char* p1 = str + left;
      char* p2 = str + right;
      while (p1 < p2) {
        char temp = *p1;
        *p1 = *p2;
        *p2 = temp;
        p1++;
        p2--;
      }
    }
     
    void rotate(char* str, int k) {
      int n = strlen(str);
      reverse_string(str, 0, n-1);
      reverse_string(str, 0, k-1);
      reverse_string(str, k, n-1);
    }    
};

