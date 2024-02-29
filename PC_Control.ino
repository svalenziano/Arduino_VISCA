//This function improves readability of debug commands
void printLinebreak(){
  Serial.println("---------------------------------------------------------");
}

  /*SERIAL COMMANDS: SEND THESE CHARACTERS VIA SERIAL TERMINAL TO MANUALLY CONTROL SYSTEM
  

  */
 
 void printCommands(){
    Serial.println();
    Serial.println(  "COMMAND LIST:"  );
    Serial.println(    "INQUIRIES: (Get info) - lowercase");
    Serial.println(    "  ?= Print COMMAND LIST");
    Serial.println(    "  p= Get current positions");
    Serial.println(    "  e= Get EEPROM values");
    Serial.println(    "  h= Go to home position");
    
    Serial.println(    "COMMANDS: (move camera) - lowercase");
    Serial.println(    "  a= Pan Left");  //Same controls as FPS games
    //Serial.println(    "  d= Pan Right");
    //Serial.println(    "  s= Tilt Down");
    //Serial.println(    "  w= Tilt Up");
    Serial.println(    "  q= Pan/Tilt Stop");
    
    Serial.println(    "OPERATIONS: (Make changes) - UPPERCASE ");
    Serial.println(    "  C= Temporarily disable pan/tilt limits");
    Serial.println(    "  H= Set home position");
    Serial.println(    "  U= Set UpRight Limit");
    Serial.println(    "  D= Set DownLeft Limit");
    Serial.println(    "  S= Toggle Start-up delay");
    Serial.println(    "  P= Invert pan axis (joystick will send inverted commands)");
    Serial.println(    "  T= Invert tilt axis");
    Serial.println(    "  Z= Invert zoom axis");
    Serial.println(    "  F (zero)= Format EEPROM");
    Serial.println();
 }
 
 void serialControl(){
   if (Serial.available() > 0) {     
     printCommands();
     printLinebreak();
     // read the incoming byte:
          char serialString = Serial.read();

       //Print command list (for reference)
         if (serialString == '?'){
           printCommands();
         }
       
       //Get current positions (send position inquiry)
          if (serialString == 'p'){
            sendI_PanTiltPos();
            delay(250);
            ptPacketCheck();
          }
       //Dump EEPROM values
         if (serialString == 'e'){
           dumpEEPROM(30);
         }
       //Go to home position
       if (serialString == 'h'){
         gotoHomePosition();
       }
       
       if (serialString == 'a'){
         panValue= -90; tiltValue = 0;
         updateSimplePanTilt();
         delay(100);
       }
       
       if (serialString == 'q'){
         sendPanTilt_Stop();
       }
       
       //Clear all pan/tilt limits -- TEMPORARY- DOES NOT AFFECT EEPROM
          if (serialString == 'C'){
            clearPanTiltLimit();
          }
       //Set home position
          if (serialString == 'H'){
            setHomePosition();
          }
       //Get pan/tilt readings from camera and set UpRight Limit
         if (serialString == 'U'){
           setUpRightLimits();
         }
       //Get pan/tilt readings and set DownLeft Limit
       if (serialString == 'D'){
         setDownLeftLimits();
       }
       
       if (serialString == 'S'){
         invertEEPROM(19);
         Serial.println("Toggled start-up delay.  1=on, 0=off");
       }
         
       //EEPROM CONTROL
       //INVERT PAN AND TILT-- Manually invert PTZ addresses in EEPROM
         if (serialString == 'P' || serialString == 'T' || serialString =='Z'){
                 //Invert Pan
                 if (serialString == 'P'){
                   invertEEPROM(0);
                 } 
                 //Invert Tilt
                 if (serialString == 'T'){
                   invertEEPROM(1);
                 }
                 //Invert Zoom
                 if (serialString == 'Z'){
                   invertEEPROM(2);
                 }
               updatePTZInverts();  //Update invert variable based on EEPROM
             }
        //Format EEPROM- set values to zero
         if (serialString == 'F'){
           formatEEPROM(75);
         }
      
          
   printLinebreak();
   }
 }
     
