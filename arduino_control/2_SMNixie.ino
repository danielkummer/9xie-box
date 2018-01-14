
State NixieBoot() {
  // 
}

char lastValue[10];    

State FlashOnUpdateMode() {
  if(SMNixie.Timeout(info.interval)) {    
    if(strcmp(info.nixie_buffer, lastValue) != 0) {
      sprintf(lastValue, "%s", info.nixie_buffer);                 
      matrix.Pulse(matrix.Color1, 10, 100);
      resetAfterComplete = true;
    } 
    nixie.write(info.nixie_buffer);           
    SMNixie.Set(FlashOnUpdateMode);
  }  
}

State SimpleCounter() {  
  static long counter = 0;
  if(SMNixie.Timeout(info.interval)) {    
    char counterBuffer[10];
    sprintf(counterBuffer, "%09d", counter);
    nixie.write(counterBuffer);    
    counter++;    
    SMNixie.Set(SimpleCounter);
  }   
}

State TripleCounter() {
  static int counter1 = 0;
  static int counter2 = 0;
  static int counter3 = 0;
  
  if(SMNixie.Timeout(info.interval)) {    
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
    nixie.write(counterBuffer);            
    SMNixie.Set(TripleCounter);    
  } 
}

State Display() {
  if(SMNixie.Timeout(info.interval)) {
    nixie.write(info.nixie_buffer);        
    SMNixie.Set(Display);  
  }
}
  
    /*
x 0 0
0 0 0
0 0 0   

0 x 0
x 0 0
0 0 0

0 0 x     
0 x 0
x 0 0

1 2 3
4 5 6
7 8 9

swipePattern
"1        "
" 2 2     "
"  3 3 3  "
"     2 2 "
"        1"

     */

State NixieScroll() {
  if(SMNixie.Timeout(info.interval)) {
    rotate(info.nixie_buffer, 1);    
    nixie.write(info.nixie_buffer);        
    SMNixie.Set(NixieScroll);
  }
  //todo - scroll neopixels as well
}

void writeNumber(long number, char* symbols) {
  if(number > 999999 || strlen(symbols) != 3) {
    return;
  }
  char numberString[7];
  sprintf(numberString,"%0d",number);  

  //optimize
  info.nixie_buffer[0] = numberString[0];
  info.nixie_buffer[1] = numberString[1];
  info.nixie_buffer[2] = symbols[0];
  info.nixie_buffer[3] = numberString[1];
  info.nixie_buffer[4] = numberString[1];
  info.nixie_buffer[5] = symbols[1];
  info.nixie_buffer[6] = numberString[1];
  info.nixie_buffer[7] = numberString[1];
  info.nixie_buffer[8] = symbols[2];    
}

void writeRow(short number, char symbol, short row) {
  if(number > 99) {
    return;
  }
  short startIdx = row*3;
  char numberString[3];
  sprintf(numberString,"%d",number);  
  strncpy(&info.nixie_buffer[startIdx], numberString,2);
  info.nixie_buffer[startIdx+2] = symbol;  
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


