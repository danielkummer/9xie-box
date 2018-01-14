uint32_t colorPattern[3][2] = { 
  {matrix.Color(102, 255, 255), matrix.Color(255,200,10)}, //light blue, dirty green
  {0,0}
  };


void handleCommand() {
  if(startsWith("help",received_command_data_buffer)) {
    Serial.println(F("Help: <mandatory> (optional), [select,mandatory]"));
    Serial.println(F("hv=[ON,OFF]              turn on/off high voltage power"));
    Serial.println(F("matrix=[ON,POFF]          turn on/off pixels"));        
    Serial.println(F("matrixDuration=<interval>      set run duration"));    
    Serial.println(F("matrixInterval=<interval> set run interval"));    
    Serial.println(F("matrixBrightness=<0-255>  set brightness"));
    Serial.println(F("matrixRun=[NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, PULSE]          set run mode to"));        
    Serial.println(F("matrixColor1=<r,b,g>      set primary pixel color"));
    Serial.println(F("matrixColor2=<r,b,g>      set secondary pixel color"));
    Serial.println(F("nixieInterval=(duration)  set nixie refresh interval"));    
    Serial.println(F("nixieRun=[COUNTER, T_COUNTER, DISPLAY, SCROLL, PULSE, WHIPE]          set nixie run mode"));    
    Serial.println(F("nixieWrite=<value>        set nixie value (max 9 chars)"));            
    Serial.println(F("info                     get info"));
    return;
  } else  
  if(startsWith("info",received_command_data_buffer)) {
    log("nixieBuffer=%s", nixieControl.nixie_buffer);    
    log("nixieHvPower=%d", nixieControl.hv_power);
    log("nixieInterval=%d", nixieControl.Interval);    
    log("nixieMode=%s", MODE_STRING[nixieControl.Mode]);       
    log("matrixRun=%s", PATTERN_STRING[matrix.ActivePattern]);
    log("matrixInterval=%d", matrix.Interval);
    log("matrixTotalSteps=%d", matrix.TotalSteps);
    log("matrixIndex=%d", matrix.Index);
    log("matrixColor1=%d,%d,%d", matrix.Red(matrix.Color1), matrix.Green(matrix.Color1), matrix.Blue(matrix.Color1) );
    log("matrixColor2=%d,%d,%d", matrix.Red(matrix.Color1), matrix.Green(matrix.Color1), matrix.Blue(matrix.Color1) );
    log("matrixDirection=%d", matrix.Direction);
    log("matrixBrightness=%d", matrix.getBrightness());
    log("buttonColor1=%d,%d,%d", buttonLed.Red(buttonLed.Color1), buttonLed.Green(buttonLed.Color1), buttonLed.Blue(buttonLed.Color1) );    
  } else  
  if(startsWith("hvPower=ON",received_command_data_buffer)) {
    nixieControl.hv_power = 1;  
  } else 
  if(startsWith("hvPower=OFF",received_command_data_buffer)) {
    nixieControl.hv_power = 0;
  } else 
  if(startsWith("matrix=ON",received_command_data_buffer)) {    
    matrix.ColorSet(matrix.Color1);
    matrix.setBrightness(255);
  } else 
  if(startsWith("maxtrix=OFF",received_command_data_buffer)) {    
    matrix.ColorSet(0);    
    matrix.setBrightness(0);
  } else 
  if(startsWith("matrixDuration=",received_command_data_buffer)) {    
    int duration = atoi(&received_command_data_buffer[15]);
    if(duration > 0) {      
      matrix.TotalSteps = duration;
    }    
  } else 
  if(startsWith("matrixInterval=",received_command_data_buffer)) {    
    int interval = atoi(&received_command_data_buffer[15]);
    if(interval > 0) {      
      matrix.Interval = interval;
    }    
  } else 
  if(startsWith("matrixRun=",received_command_data_buffer)) {    
    char* patternString = &received_command_data_buffer[10];
    
    if(startsWith("NONE",patternString)) {
      matrix.ActivePattern = NONE;
    } else
    if(startsWith("RAINBOW",patternString)) {
      matrix.RainbowCycle(255);            
    } else
    if(startsWith("THEATER",patternString)) {
      matrix.ActivePattern = THEATER_CHASE;
      matrix.Interval = 100;      
    } else
    if(startsWith("COLOR",patternString)) {
      matrix.ActivePattern = COLOR_WIPE; 
    } else
    if(startsWith("SCANNER",patternString)) {
      matrix.ActivePattern = SCANNER;
    } else
    if(startsWith("FADE",patternString)) {
      matrix.ActivePattern = FADE;
    } else   
    if(startsWith("PULSE",patternString)) {
      matrix.ActivePattern = PULSE;           
    } else {
      Serial.println(F("error=unknown pattern"));       
    }
  } else 
  if(startsWith("matrixBrightness=",received_command_data_buffer)) {        
    matrix.setBrightness(atoi(&received_command_data_buffer[17]));
    matrix.show();
  } else  
  
  if(startsWith("matrixColor1=",received_command_data_buffer)) {        
    uint32_t color = getIntColor(&received_command_data_buffer[13]);    
    matrix.ColorSet(color);
    matrix.Color1 = color;   
  } else 
  if(startsWith("matrixColor2=",received_command_data_buffer)) {        
    uint32_t color = getIntColor(&received_command_data_buffer[13]);        
    matrix.Color2 = color;   
  } else 
  if(startsWith("buttonColor1=",received_command_data_buffer)) {        
    uint32_t color = getIntColor(&received_command_data_buffer[13]);    
    buttonLed.ColorSet(color);
    buttonLed.Color1 = color;   
  } else 
  //--------------- NIXIE CONTROL
  if(startsWith("nixieInterval=",received_command_data_buffer)) {    
    int interval = atoi(&received_command_data_buffer[14]);
    if(interval > 0) {
      nixieControl.Interval = interval;      
    }    
  } else   
  if(startsWith("nixieWrite=",received_command_data_buffer)) {    
    char* bufferData = &received_command_data_buffer[11];
    strncpy(nixieControl.nixie_buffer, bufferData, 9);    
  } else
  if(startsWith("nixieRun=",received_command_data_buffer)) {    
    char* mode = &received_command_data_buffer[9];
    
    if(startsWith("COUNTER",mode)) {      
      nixieControl.SimpleCounter(100);    
    } else
    if(startsWith("T_COUNTER",mode)) {      
      nixieControl.TrippleCounter(100);    
    } else
    if(startsWith("DISPLAY",mode)) {
      nixieControl.Display(50);          
    } else
    if(startsWith("SCROLL",mode)) {
      nixieControl.Scroll(100);    
    } else 
    if(startsWith("PULSE",mode)) {
      nixieControl.PulseOnUpdate(100);                
      //SMNixie.Set(FlashOnUpdateMode);                   
      //matrix.ActivePattern = NONE;
    } else   
    if(startsWith("WHIPE",mode)) {
      nixieControl.Whipe();                      
    } else {
      Serial.println(F("error=unknown mode"));       
    }
    
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
