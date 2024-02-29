
// Startup sequence: this function will execute all items necessary to 
// start communication with camera and properly setup limits, etc.
void startupSequence(){
  digitalWrite(inputLed, HIGH);
  //Send startup sequence to camera
  sendAddressCommand();  
    delay(delayTime);
  sendIfClear();  
    delay(delayTime);  
  gotoHomePosition();  //Pan to home position (user-configurable)
    delay(delayTime);
  sendDZoom_Off();  //Turn off digital zoom
    delay(delayTime);
  sendZoom_WideVar(55);  //Zoom out
    delay(1000);  //Longer delay to allow zoom all the way out
  setPanTiltLimit();  //Set pan/tilt limits
    delay(delayTime);
  
  Serial.println( "[SETUP DONE]" );
  digitalWrite(inputLed, LOW);
}

//INQUIRY BYTES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// "I" for inquiry

// Zoom position inquiry, returns zoom amount
void sendI_ZoomPos(){
  //Send inquiry  
  byte i_ZoomPos[5] = {0x81, 0x09, 0x04, 0x47, 0xFF};
  for (int i=0; i<5; i++){
    Serial3.write( i_ZoomPos[i] );
  }
  Serial.println(" [Sent zoom position inquiry]");
}

//Pan/Tilt position inquiry, watch serial for response
void sendI_PanTiltPos(){
  byte i_PanTiltPos[5]= {0x81, 0x09, 0x06, 0x12, 0xFF};
  //Send inquiry
  for (int i=0; i<5; i++){
    Serial3.write( i_PanTiltPos[i] );
  }
  Serial.println(" [Sent pan/tilt position inquiry]");
}

//CONTROL BYTES//////////////////////////////////////////////////////////////////////////////


void sendCommandCancel(){  //Cancel previous command
  byte commandCancel[3]= {0x81,0x21,0xFF};
  for (int i=0; i<3; i++){
    Serial3.write(commandCancel[i]);
  }
}

void sendDisplayToggle(){  //Toggles on-screen display display on and off
  byte displayToggle[6]= {0x81, 0x01, 0x04, 0x15, 0x10, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(displayToggle[i]);
  }
  Serial.println(" Executed sendDisplayToggle()");
}

void sendManualFocus(){  //Turns off autofocus
  byte manualFocus[6]= {0x81, 0x01, 0x04, 0x38, 0x03, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(manualFocus[i]);
  }
  Serial.println(" Executed sendManualFocus()");
}

void sendAutoFocus(){  //Switches on autofocus
  byte autoFocus[6]= {0x81, 0x01, 0x04, 0x38, 0x02, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(autoFocus[i]);
  }
  Serial.println(" Executed sendAutoFocus()");
}

void sendManualExposure(){  //Turns off auto-exposure
  byte manualExposure[6]= {0x81, 0x01, 0x04, 0x39, 0x03, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(manualExposure[i]);
  }
  Serial.println(" [Executed sendManualExposure()] ");
}

void sendAutoExposure(){  //Turns on auto-exposure
  byte autoExposure[6]= {0x81, 0x01, 0x04, 0x39, 0x00, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(autoExposure[i]);
  }
  Serial.println(" [Executed sendAutoExposure()] ");
}


//STARTUP BYTES//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//These commands are required for startup
void sendAddressCommand(){
  byte addressCommand[4]= {0x88,0x30,0x01,0xFF};
  for (int i=0; i<4; i++){
    Serial3.write(addressCommand[i]);
  }
  Serial.print(" executed AddressCommand()");
}

void sendIfClear(){  //See camera documentation
  byte ifClear[5]= {0x88,0x01,0x00,0x01,0xFF};
  for (int i=0; i<5; i++){
    Serial3.write(ifClear[i]);
  }
  Serial.print(" executed IFClear()");
}
