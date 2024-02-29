//PAN/TILT BYTES///////////////////////////////////////////////////////////////////////////////
 
void sendPanTilt_Home(){
   byte panTilt_Home[5]= {0x81, 0x01, 0x06, 0x04, 0xFF};
   for (int i=0; i<5; i++){
     Serial3.write(panTilt_Home[i]);
   }
   Serial.print(" executed sendPanTilt_Home()");
}
 
void sendPanTilt_Stop(){
  byte panTilt_Stop[9]= {0x81, 0x01, 0x06, 0x01, 0x09, 0x09, 0x03, 0x03, 0xFF};
  for (int i=0; i<9; i++){
    Serial3.write(panTilt_Stop[i]);
  }
  Serial.print(" executed sendPanTilt_Stop()");
}

//Check pan/tilt position (you must send pan/tilt inquiry first)
boolean ptPacketCheck(){
  checkSerial();  //Update serial buffer
  //If PTposition packet is detected
    //READ PAN/TILT INFO
    if (inBuffer[0]== 0x90  && inBuffer[1]==0x50 && inBuffer[9]!=0xFF && inBuffer[10]==0xFF){
      Serial.println(" [[ Pan/Tilt packet detected!>>");
      Serial.print(" Pan=");
      //Read bytes 2 thru 5 for pan position
      for (int i=2; i<6; i++){
        Serial.print(inBuffer[i], HEX); //print byte
        Serial.print("-"); //Separator for readability
      }
      Serial.print(" Tilt=");
      //Read bytes 6 thru 9 for pan position
      for (int i=6; i<10; i++){
        Serial.print(inBuffer[i], HEX); //print byte
        Serial.print("-"); //Separator for readability
      }
      return true;
    }
    else {
      Serial.println(" [No Pan/Tilt packet detected] ");
      return false;
    }
}
  
//DIRECT PAN/TILT FUNCTION:  
  void updateSimplePanTilt(){
    //Variables that are shared by all parts of this function
    String ptCommand= "No ptCommand";  //Holds the name of the command sent to the camera, for debugging
    byte panSpeed;
    byte tiltSpeed;
    
    //If pan or tilt values are greater than threshold
    if (abs(tiltValue) > ptThreshold || abs(panValue)> ptThreshold){
      //Update pan and tilt speed bytes based on position of Joystick
      //Modify panValue with ptMultiplier (the more you zoom in, the slower you pan/tilt)
      int maxPanSpeed = panSpeedLimit*ptMultiplier; //Set speed in Options-- Map 1-24 because 24 dec = 18 hex (Camera acceps 0x01 to 0x18 for pan speeds)
      int maxTiltSpeed= tiltSpeedLimit*ptMultiplier;  //Map 1-23 because 23 dec = 17 hex (Camera accepts 0x01 to 0x17 for tilt speeds)
      //Map to camera-readable values
      panSpeed  = map( abs(panValue), ptThreshold, 100, 1, maxPanSpeed);  
      tiltSpeed = map( abs(tiltValue), ptThreshold, 100, 1, maxTiltSpeed);  
      //And pan/tilt in appropriate direction
      
      //TILT UP
      //If tilt value is above the minimum threshold, and pan value is neither above or below pan threshold
      if (tiltValue > ptThreshold && abs(panValue)< ptThreshold) {
        byte panTilt_Up[9] = {0x81, 0x01, 0x06, 0x01, 0x08,     0x0F,     0x03, 0x01, 0xFF};  //BYTE 5 is TILT speed 01 to 17 (REMEMBER: START COUNTING AT 0)
        //update speed
        panTilt_Up[5] = tiltSpeed;  //If you count wrong and modify panTilt_Up[6], it won't work!
        //Send command to camera
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_Up[i];
          //Serial.print(panTilt_Up[i],HEX); Serial.print("-");
        }
        ptCommand= (" sent panTilt_Up[9]");
      }
      //TILT DOWN
      else if (tiltValue < -ptThreshold && abs(panValue)< ptThreshold) {
        byte panTilt_Down[9] = {0x81, 0x01, 0x06, 0x01, 0x08,     0x17,     0x03, 0x02, 0xFF};  //BYTE 5 is tilt speed 01 to 17 (REMEMBER: START COUNTING AT 0)
        //Update speed
        panTilt_Down [5] = tiltSpeed;
        //Send command to camera
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_Down[i];
        }
          ptCommand= (" send panTilt_Down[9]");
      }
      //PAN LEFT
      //If pan value is left of threshold, and tilt value is neither above nor below threshold
      else if (panValue < -ptThreshold && abs(tiltValue)< ptThreshold){
        byte panTilt_Left[9]= {0x81, 0x01, 0x06, 0x01,    0x0A,     0x0B, 0x01, 0x03, 0xFF};  //BYTE 4 is pan speed 01 to 18
        //Update pan SPeed (previously mapped)
        panTilt_Left[4]= panSpeed;
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_Left[i];
        }
        ptCommand= (" send panTilt_Left[9]");
      }
      //PAN RIGHT
      //If pan value is right of threshold, and tilt value is neither above nor below threshold
      else if (panValue > ptThreshold && abs(tiltValue)< ptThreshold){
        byte panTilt_Right[9]= {0x81, 0x01, 0x06, 0x01,    0x0A,     0x0B, 0x02, 0x03, 0xFF};  //BYTE 4 is pan speed 01 to 18
        //Update pan SPeed (previously mapped)
        panTilt_Right[4]= panSpeed;
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_Right[i];
        }
        ptCommand= (" send panTilt_Right[9]");
      }
      
      //Pan UpLeft
      else if (panValue < -ptThreshold && tiltValue > ptThreshold){
        byte panTilt_UpLeft[9]= {0x81, 0x01, 0x06, 0x01,   0x0A, 0x0B,   0x01, 0x01, 0xFF};  //BYTE 4 is pan speed, BYTE4 is tilt speed
        //update speeds
        panTilt_UpLeft[4]= panSpeed;
        panTilt_UpLeft[5]= tiltSpeed;
        //Send to cam
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_UpLeft[i];
        }
        ptCommand= (" sent panTilt_UpLeft[9]");
    }
    
    //Pan UpRight
    else if (panValue > ptThreshold && tiltValue > ptThreshold){
        byte panTilt_UpRight[9]= {0x81, 0x01, 0x06, 0x01,    0x0A, 0x0B,    0x02, 0x01, 0xFF};
        //Update speeds
        panTilt_UpRight[4]= panSpeed;
        panTilt_UpRight[5]= tiltSpeed;
        //Send to cam
        for (int i=0; i<9; i++){
          ptByte[i] = panTilt_UpRight[i];
        }
        ptCommand= (" sent panTilt_UpRight[9]");
      }
    
    
    //Pan DownLeft
    else if (panValue < -ptThreshold && tiltValue < -ptThreshold){
      byte panTilt_DownLeft[9]= {0x81, 0x01, 0x06, 0x01,   0x0A, 0x0B,     0x01, 0x02, 0xFF};
      //Update speeds
      panTilt_DownLeft[4]= panSpeed;
      panTilt_DownLeft[5]= tiltSpeed;
      //Send to cam
      for (int i=0; i<9; i++){
        ptByte[i] = panTilt_DownLeft[i];
      }
      ptCommand= (" sent panTilt_UpRight[9]");
    }
    
    //Pan DownRight
    else if (panValue > ptThreshold && tiltValue < -ptThreshold){
      byte panTilt_DownRight[9]= {0x81, 0x01, 0x06, 0x01,   0x0A, 0x0B,    0x02, 0x02, 0xFF};
      //Update speeds
      panTilt_DownRight[4]= panSpeed;
      panTilt_DownRight[5]= tiltSpeed;
      //Send to cam
      for (int i=0; i<9; i++){
        ptByte[i] = panTilt_DownRight[i];
        }
      ptCommand= (" sent panTilt_DownRight[9]");
      }  
   }    
    //If joystick aren't reading any input above the threshold, stop all panning or tilting
    else {
        byte panTilt_Stop[9]= {0x81, 0x01, 0x06, 0x01, 0x09, 0x09, 0x03, 0x03, 0xFF};
        for (int i=0; i<9; i++){
            ptByte[i] = panTilt_Stop[i];
        }
     ptCommand= (" sent panTilt_Stop[9]");
    }
    
    //IF NEW BYTE IS DIFFERENT THAN THE LAST BYTE SENT TO THE CAMERA, Send byte to camera, and update old byte variable
    //Compare each byte in the array.  If any byte is different, send Command.
    if (arrayCompare(ptByte, ptByteOld, 9, 9) == false ){
      resetFocusTimer();  //Make sure focus timer doesn't trip
      for (int i=0; i<9; i++){  
          //Send buffered command to camera
          Serial3.write(ptByte[i]);
          //update old byte for comparison next time around
          ptByteOld[i] = ptByte[i];
          //A little delay between bytes ensures the camera gets it all.  Delete this delay and you may have some reliability issues.
          delay(5);  
        }
      //Send debug info, included pan and tilt speeds
      Serial.print(ptCommand); Serial.print(" panSpeed="); Serial.print(panSpeed, HEX); Serial.print(" tiltSpeed="); Serial.print(tiltSpeed, HEX);
      Serial.println();
      
    }
    
}

