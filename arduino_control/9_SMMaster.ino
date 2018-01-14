State MasterBoot() {      
  EXEC(SMNixie);  

  if(SMMaster.Timeout(300)) {
    digitalWrite(HIGH_VOLTAGE_EN_PIN, info.hv_power);    
  } 
}

State MasterRun() {
  if(SMMaster.Timeout(300)) {
    digitalWrite(HIGH_VOLTAGE_EN_PIN, info.hv_power);    
  }
  
  EXEC(SMNixie);      
}

