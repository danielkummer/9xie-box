/*uint32_t colorPattern[3][2] = { 
  {matrix.Color(102, 255, 255), matrix.Color(255,200,10)}, //light blue, dirty green
  {0,0}
  };*/

void handleCommand() {
  if(startsWith("help",received_command_data_buffer)) {
    Serial.println(F("Help: <mandatory> (optional), [select,mandatory]"));        
    Serial.println(F("nixie<command>= Tube control"));    
    Serial.println(F("V=[ON,OFF]      turn on/off high voltage power"));
    Serial.println(F("Interval=(dur)  set nixie refresh interval"));    
    Serial.println(F("Run=[COUNTER, T_COUNTER, DISPLAY, SCROLL, PULSE, WHIPE]"));    
    Serial.println(F("Write=<value>   set nixie value (max 9 chars)"));            
    
    Serial.println(F("pixel<command>= Pixel control"));        
    Serial.println(F("V=[ON,OFF]          turn on/off pixels"));        
    Serial.println(F("Duration=<interval> set run duration"));    
    Serial.println(F("Interval=<interval> set run interval"));    
    Serial.println(F("Brightness=<0-255>  set brightness"));
    Serial.println(F("Run=[NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, PULSE]          set run mode to"));        
    Serial.println(F("Color1=<r,b,g>      set primary pixel color"));
    Serial.println(F("Color2=<r,b,g>      set secondary pixel color"));
    Serial.println(F("Mask=[0-1+,NONE]    set pixel-mask"));
            
    Serial.println(F("info   get info"));
    return;
  } else if(startsWith("info",received_command_data_buffer)) {    
    log("buffer=%s", nixieControl.nixie_buffer);    
    log("hv=%d", nixieControl.hv_power);
    log("interval=%d", nixieControl.Interval);    
    log("mode=%s", MODE_STRING[nixieControl.Mode]);       
    log("pixelRun=%s", PATTERN_STRING[matrix.ActivePattern]);
    log("pixelInterval=%d", matrix.Interval);
    log("pixelTotalSteps=%d", matrix.TotalSteps);
    log("pixelIndex=%d", matrix.Index);
    log("pixelColor1=%d,%d,%d", matrix.Red(matrix.Color1), matrix.Green(matrix.Color1), matrix.Blue(matrix.Color1) );
    log("pixelColor2=%d,%d,%d", matrix.Red(matrix.Color1), matrix.Green(matrix.Color1), matrix.Blue(matrix.Color1) );
    log("pixelDirection=%d", matrix.Direction);
    log("pixelBrightness=%d", matrix.getBrightness());
    log("buttonColor1=%d,%d,%d", buttonLed.Red(buttonLed.Color1), buttonLed.Green(buttonLed.Color1), buttonLed.Blue(buttonLed.Color1) );    
  } else if(startsWith("pixel",received_command_data_buffer)) {
    char* matrixCommand = &received_command_data_buffer[5];     
    handlePixelCommand(matrixCommand);    
  } else if(startsWith("nixie",received_command_data_buffer)) {      
    char* nixieCommand = &received_command_data_buffer[5]; 
    handleNixieCommand(nixieCommand);
  } else if(startsWith("buttonColor1=",received_command_data_buffer)) {        
      uint32_t color = getIntColor(&received_command_data_buffer[13]);    
      buttonLed.ColorSet(color);
      buttonLed.Color1 = color;       
  } else {
    Serial.println(F("error=unknown command"));
    return;
  }
  Serial.println(F("success"));      
}

uint32_t getIntColor(const char* intcolors) {
  char delimiter[] = ",";
  char *ptr;

  // initialisieren und ersten Abschnitt erstellen
  ptr = strtok(intcolors, delimiter); 
  return matrix.Color(atoi(ptr), atoi(strtok(NULL, delimiter)), atoi(strtok(NULL, delimiter)));
}

bool startsWith(const char *pre, const char *str) {
  return strncmp(pre, str, strlen(pre)) == 0;
}

bool isOn(char* str) {
  return startsWith("=ON", str);
}

bool isOff(char* str) {
  return startsWith("=OFF", str);
}

void handlePixelCommand(const char* matrixCommand) {
  if(startsWith("V",matrixCommand)) {    
      matrix.ColorSet(isOn(&matrixCommand[1]) ? matrix.Color1 : 0);
      matrix.setBrightness(isOn(&matrixCommand[1]) ? 255 : 0);           
    } else     
    if(startsWith("Duration=",matrixCommand)) {    
      int duration = atoi(&matrixCommand[9]);
      if(duration > 0) {      
        matrix.TotalSteps = duration;
      }    
    } else 
    if(startsWith("Interval=",matrixCommand)) {    
      int interval = atoi(&matrixCommand[9]);
      if(interval > 0) {      
        matrix.Interval = interval;
      }    
    } else 
    if(startsWith("Run=",matrixCommand)) {    
      char* patternString = &matrixCommand[4];
      
      if(startsWith("NO",patternString)) { //NONE
        matrix.ActivePattern = NONE;
      } else
      if(startsWith("RA",patternString)) { //RAINBOW
        matrix.RainbowCycle(255);            
      } else
      if(startsWith("TH",patternString)) { //THEATER
        matrix.ActivePattern = THEATER_CHASE;
        matrix.Interval = 100;      
      } else
      if(startsWith("CO",patternString)) { //COLOR
        matrix.ActivePattern = COLOR_WIPE; 
      } else
      if(startsWith("SC",patternString)) { //SCANNER
        matrix.ActivePattern = SCANNER;
      } else
      if(startsWith("FA",patternString)) { //FADE
        matrix.ActivePattern = FADE;
      } else   
      if(startsWith("PULSE",patternString)) {
        matrix.ActivePattern = PULSE;           
      } else {
        Serial.println(F("error=unknown pattern"));       
      }
    } else 
    if(startsWith("Brightness=",matrixCommand)) {        
      matrix.setBrightness(atoi(&matrixCommand[11]));
      matrix.show();
    } else  
    if(startsWith("Mask=",matrixCommand)) {                  
      char* maskString = &matrixCommand[5];
      if(startsWith("NONE",maskString)) {
        matrix.UnMask();
      } else {
        matrix.Mask(maskString); 
      }
    } else 
    if(startsWith("Color1=",matrixCommand)) {        
      uint32_t color = getIntColor(&matrixCommand[7]);    
      matrix.ColorSet(color);
      matrix.Color1 = color;   
    } else 
    if(startsWith("Color2=",matrixCommand)) {        
      uint32_t color = getIntColor(&matrixCommand[7]);        
      matrix.Color2 = color;   
    }      
}

void handleNixieCommand(const char* nixieCommand) {
  if(startsWith("V",nixieCommand)) {
    nixieControl.hv_power = isOn(&nixieCommand[1]) ? 1 : 0;  
  } else if(startsWith("Interval=",nixieCommand)) {    
    int interval = atoi(&nixieCommand[9]);
    if(interval > 0) {
      nixieControl.Interval = interval;      
    }    
  } else if(startsWith("Write=",nixieCommand)) {    
    char* bufferData = &nixieCommand[6];
    strncpy(nixieControl.nixie_buffer, bufferData, 9);    
  } else if(startsWith("Run=",nixieCommand)) {    
    char* mode = &nixieCommand[4];
    
    if(startsWith("CO",mode)) {    //COUNTER    
      nixieControl.SimpleCounter(100);    
    } else if(startsWith("T_CO",mode)) { //T_COUNTER
      nixieControl.TrippleCounter(100);    
    } else if(startsWith("DI",mode)) { //DISPLAY
      nixieControl.Display(50);          
    } else if(startsWith("SC",mode)) { //SCROLL
      nixieControl.Scroll(100);    
    } else if(startsWith("PU",mode)) { //PULSE
      nixieControl.PulseOnUpdate(100);                
      //SMNixie.Set(FlashOnUpdateMode);                   
      //matrix.ActivePattern = NONE;
    } else if(startsWith("WH",mode)) { //WHIPE
      nixieControl.Whipe();                      
    } else {
      Serial.println(F("error=unknown mode"));       
    }  
  }
}  
  

