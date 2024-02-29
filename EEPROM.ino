//FUNCTIONS ASSOCIATED WITH SETTING PAN/TILT LIMITS AND SENDING PAN/TILT POSITION INQUIRY

/*EEPROM Formatting
  0  -  Pan invert (0= false, 1=true)
  1  -  Tilt invert
  2  -  Zoom invert
  3,4,5,6 -  Pan Right limit
  7,8,9,10 - Tilt Up limit
  11,12,13,14 - Pan Left limit
  15,16,17,18 - Tilt Down limit
  19 - Start-up delay (0=false, 1=true)
  20,21,22,23 - Home pan position
  24,25,26,27 - Home tilt position
  
  */

void setHomePosition(){
  Serial.println(  "Begin setHomePosition"  );
  sendI_PanTiltPos();  //Ask for pan/tilt position
  delay(400);  //Wait for camera to respond
  //If a pan/tilt packed is recieved
  if (ptPacketCheck()==true){
    //Update EEPROM with new HomePosition pan/tilt values
    Serial.println(" Updating EEPROM (addr, value): ");
    for (int i=20; i<24; i++){  //Update EEPROM 20 thru 23
      EEPROM.write(i, inBuffer[i-18]);  //...Pan Values (inBuffer 2 thru 5)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-18],HEX);  //debug info
    }
    for (int i=24; i<28; i++){  //Update EEPROM 24 thru 27
      EEPROM.write(i, inBuffer[i-18]);  //...Tilt Values (inBuffer 6 thru 9)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-18],HEX);  //debug info
    }
    Serial.println("...Updating EEPROM complete ");
    Serial.println(" setHomePosition() complete ");
  }
  else { Serial.println(" No ptPacket detected, abort setHomePosition() ");}
}

void gotoHomePosition(){
      sendZoom_WideVar(55);  //Zoom out 
      byte panTilt_AbsolutePos[15]= {0x81, 0x01, 0x06, 0x02, //Bytes 0-3...header   ... for terminal program: 0x81, 0x01, 0x06, 0x02, 0x08, 0x08, 0x00, 0x08, 0x0D, 0x0B, 0x00, 0x04, 0x0B, 0x00, 0xFF
                     0x12,  //VV= pan speed 01 to 18
                     0x12,  //WW= tilt speed 01 to 17
                     0x00, 0x00, 0x00, 0x00, //Bytes 6 thru 9: 0Y 0Y 0Y 0Y= pan position, where YYYY= pan position. Range= F725 to 08DB (center 0000)
                     0x00, 0x00, 0x00, 0x00,  //Bytes 10 thru 13: 0Z 0Z 0Z 0Z Tilt position, where ZZZZ=tilt position.  Range= FE70 to 04B0 (Image Flip: OFF); 
                     //OR Range= FB50 to 0190 (Image Flip: ON) (center 0000) see page 51 of tech mnaual for info
                     0xFF};  //Byte 14 - tail
     //Update pan position to value stored in EEPROM
     Serial.println(" Home position is ");
     for (int i=6; i<10; i++){  //For camera bytes 7 thru 10
       panTilt_AbsolutePos[i] = EEPROM.read(i+14);  //Update with EEPROM values 20 thru 23
       Serial.print(i); Serial.print(","); Serial.println(panTilt_AbsolutePos[i],HEX);  //debug info
     }
     //Update tilt position
     for (int i=10; i<14; i++){  //For camera bytes 11 thru 14
       panTilt_AbsolutePos[i] = EEPROM.read(i+14);  //Update with EEPROM values 24 thru 27
       Serial.print(i); Serial.print(","); Serial.println(panTilt_AbsolutePos[i],HEX);  //debug info
     }
     //Send to camera
     Serial.println("[Sending home position to Camera]");
     for (int i=0; i<15; i++){
       Serial3.write(panTilt_AbsolutePos[i]);
       Serial.print(panTilt_AbsolutePos[i], HEX); Serial.print(" , ");
       delay(5);
     }
     Serial.println("[Sent home position to Camera]");
}
  

void setUpRightLimits(){
  Serial.println( "Begin setUpRightLimits" );
  sendI_PanTiltPos();  //Ask for pan/tilt position
  delay(250);  //Wait for camera to respond
  //If a pan/tilt packet is recieved
  if (ptPacketCheck()==true){
    //Update EEPROM with new UpRight pan/tilt values
    Serial.println(" Updating EEPROM (addr, value): ");
    for (int i=3; i<7; i++){  //Update EEPROM 3 thru 7 with...
      EEPROM.write(i, inBuffer[i-1]);  //...Pan Right Values (inBuffer 2 thru 5)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-1],HEX);  //debug info
    }
    for (int i=7; i<11; i++){  //Update EEPROM 7 thru 11 with...
      EEPROM.write(i, inBuffer[i-1]);  //...Tilt Up Values (inBuffer 6 thru 9)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-1],HEX);
    }
  Serial.println("...Updating EEPROM complete ");  
  setPanTiltLimit();  //Send new limits to Camera
  }
}

void setDownLeftLimits(){
  Serial.println( "Begin setDownLeftLimits" );
  sendI_PanTiltPos();  //Ask for pan/tilt position
  delay(250);  //Wait for camera to respond
  //If a pan/tilt packet is recieved
  if (ptPacketCheck()==true){
    //Update EEPROM with new UpRight pan/tilt values
    Serial.println(" Updating EEPROM (addr, value): ");
    for (int i=11; i<15; i++){  //Update EEPROM 11 thru 14 with...
      EEPROM.write(i, inBuffer[i-9]);  //...Pan Left Values (inBuffer 2 thru 5)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-1],HEX);  //debug info
    }
    for (int i=15; i<19; i++){  //Update EEPROM 15 thru 18 with...
      EEPROM.write(i, inBuffer[i-9]);  //...Tilt Up Values (inBuffer 6 thru 9)
      Serial.print(i); Serial.print(","); Serial.println(inBuffer[i-1],HEX);
    }
  Serial.println("...Updating EEPROM complete ");
  setPanTiltLimit();  //Send new limits to Camera
  }
}

//Clear all pan/Tilt Limits
void clearPanTiltLimit(){
  byte clearPanTilt[15] = {0x81, 0x01, 0x06, 0x07, 0x01, 0x01, //Byte #6: 0x01 for UpRight, 0x00 for DownLeft
                            0x07, 0x0F, 0x0F, 0x0F, 0x07, 0x0F, 0x0F, 0x0F, 0xFF};
  Serial.print(" [Sending: ");
  for (int i=0; i<15; i++){
    Serial3.write(clearPanTilt[i]);
    Serial.print(clearPanTilt[i], HEX);  Serial.print(",");  Serial.println("...done Sending] ");
  }
  clearPanTilt[5] = 0x00;  //Set to 
  Serial.print(" [Sending: ");
  for (int i=0; i<15; i++){
    Serial3.write(clearPanTilt[i]);
    Serial.print(clearPanTilt[i], HEX); Serial.print(","); Serial.println("...done Sending] ");
  } 
}
       
//Gets limits from EEPROM and send to Camera
void setPanTiltLimit(){
  byte ptLimitSet[15]= {0x81, 0x01, 0x06, 0x07, 0x00,  //Header- bytes 0-4
                        0x01,  //Byte 5 determines whether you are changing UpRight or DownLeft limits
                        0x00, 0x00, 0x00, 0x00, //Bytes 6-9 //For UpRight: Pan limit position 0001-08DB
                                                            //For DownLeft: Pan limit position F725-FFFF
                        0x00, 0x00, 0x00, 0x00, //Bytes 10-13 //For UpRight: Tilt limit position 0001-04B0 (Image flip OFF), 0001-0190 (Image flip ON)
                                                              //For DownLeft: Tilt limit position FE70-FFF (Image flip OFF), FB50-FFFF (Image flip ON)
                        0xFF};  //Tail
  //Update UpRight limits first
      //Right limit- get EEPROM bytes 3,4,5,6
      for (int i=6; i<10; i++){
        ptLimitSet[i] = EEPROM.read(i-3);  
      }
      //Up limit- get EEPROM bytes 7,8,9,10
      for (int i=10; i<14; i++){
        ptLimitSet[i] = EEPROM.read(i-3);
      }
      //Send to camera
      for (int i=0; i<15; i++){
        Serial3.write(ptLimitSet[i]);
        delay(5);
      }
      delay(delayTime);  //Larger delay to make sure camera got first command
  //Update DownLeft limits
      ptLimitSet[5]= 0x00;  //Select DownLeft mode
      //Left Limit - get EEPROM bytes 11,12,13,14
      for (int i=6; i<10; i++){
        ptLimitSet[i] = EEPROM.read(i+5);
      }
      //Down Limit - get EEPROM bytes 15,16,17,18
      for (int i=10; i<14; i++){
        ptLimitSet[i]= EEPROM.read(i+5);
      }
      //Send to camera
      for (int i=0; i<15; i++){
        Serial3.write(ptLimitSet[i]);
        delay(5);
      }
  //Debug info
      Serial.println(" Executed setPanTiltLimit... Sent:");
      for (int i=0; i<15; i++){
        Serial.print(ptLimitSet[i]); Serial.print("-");
      }
      Serial.println();
}

//Flip an EEPROM boolean value.  If it's 0, make it 1.  If it's 1, make it 0.
void invertEEPROM(int addr){
   int data = -1;
   data = EEPROM.read(addr);
   //If value of address is 0 or 1, invert 
   if (data == 0 || data == 1){
     if (data == 0){
       data=1;  //Invert byte
       EEPROM.write(addr, data);  //Write to camera
     }
     else if (data == 1){
       data=0;  //Invert byte
       EEPROM.write(addr, data);  
     }
     Serial.print(" [Set EEPROM address "); Serial.print(addr); 
     Serial.print(" to "); Serial.print(data); Serial.println("] ");
   }
   //Otherwise, print error message
   else {
     Serial.print (" [Error. EEPROM Address "); Serial.print(addr); 
     Serial.print(" = "); Serial.print(data); Serial.println(" ] ");
   }
 }
 
 //Dump EEPROM Records 0 thru 'numRecords'
 void dumpEEPROM(int numRecords){
   Serial.println("EEPROM dump:");
     for (int i=0; i< numRecords; i++){
       int data = -1;  //dummy data
       data = EEPROM.read(i);
       Serial.print (i);
       Serial.print("\t");
       Serial.print(data, DEC);
       Serial.println();
       delay(5);
     }
   Serial.println("EEPROM dump complete");
 }
 
  //Format EEPROM Records 0 thru 'numRecords'
 void formatEEPROM(int numRecords){
   Serial.println("EEPROM dump:");
     for (int i=0; i< numRecords; i++){
       EEPROM.write(i, 0);
       delay(5);
     }
   Serial.println("EEPROM dump complete");
 }
 
 //Get info from camera, update program variables
 void updatePTZInverts(){
   for (int i=0; i<3; i++){
     int data = EEPROM.read(i);
     ptzInverts[i] = data;
     Serial.print("ptzInverts["); Serial.print(i); Serial.print("]="); Serial.println(data);    
   }
 }
 
 //Resets EEPROM bytes for PTZ inverts
 void clearInverts(){
   for (int i=0; i<3; i++){
     EEPROM.write(i, 0);
   }
   Serial.println("EEPROM cleared");
   dumpEEPROM(3);
 }
