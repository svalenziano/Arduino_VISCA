byte CAM_VersionInq[5]= {0x81, 0x09, 0x00, 0x02, 0xFF};

//Compares 2 arrays
boolean arrayCompare(byte *a, byte *b, int len_a, int len_b){
      int n;

      // if their lengths are different, return false
      if (len_a != len_b) return false;

      // test each element to be the same. if not, return false
      for (n=0; n<len_a; n++) if (a[n]!=b[n]) return false;

      //ok, if we have not returned yet, they are equal :)
      return true;
}

//READS BYTES SENT FROM CAMERA, ECHOES THEM TO THE PC FOR DEBUGGING
//NOTE: YOU CAN CHANGE HOW MANY BYTES ARE WATCHED FOR
void echoSerial(){
    // If a new byte is available (not 0xFF) read from Camera, send to Computer (debug) port:
  if (Serial3.available()) {
    
    checkSerial();  //Update software buffer
    
    //The following code watches for zoom info returning from the camera, and updates pan/tilt speeds correspondingly
    //If zoom inquiry packet is detected
    if (inBuffer[0]==0x90  && inBuffer[1]==0x50  && inBuffer[5]!=0xFF && inBuffer[6]==0xFF){  //  
                      Serial.println("[[Zoom packet detected!>>");
                      //for (int i=2; i<6; i++){ int bufferByte= 0; bufferByte= (int) inBuffer[i]; Serial.print(bufferByte); Serial.print("-");}    //Print individual INTEGERS (converted from bytes)
                      //for (int i=2; i<6; i++){Serial.print(inBuffer[i], HEX); Serial.print("-");}  //Print individual bytes
                      
                      //Calculate and print zoomTotal  
                        int bufferByte=0;
                        int zoomInquiry=0;
                        int digitPlace=3;
                        //READ ZOOM INFO
                        //Read bytes 2 thru 5 (as integers)
                        for (int i=2; i<6; i++){
                          bufferByte= (int) inBuffer[i];  //Read digit from buffer-- the first reading is actually digit 4 (leftmost digit)
                          Serial.print("("); Serial.print(bufferByte); Serial.print(")");
                          //Calculate total zoomValue
                          bufferByte= bufferByte*( pow(16,digitPlace) );  //Multiply *16 for each digit place, because it's base 16 (hexadecimal).  Use pow(base,exponent) not ^.... ^ is a bitwise operator!
                          Serial.print(bufferByte); Serial.print(",");
                          zoomInquiry = zoomInquiry + bufferByte;  //Add each digit to buffer
                          digitPlace--;  //Decrement digit place 
                        }
                        Serial.print(" zoomInquiry="); Serial.print (zoomInquiry);  //Print total
                        //Map number recieved from camera to zoomMultiplier.  Higher zoom should result in slower zooming.
                        ptMultiplier= map(zoomInquiry, 0, 16383, 0, slowZoom);  //Don't try to do addtl math on this line, it will not work
                        ptMultiplier= 1 - (ptMultiplier/100);  //Invert, and put on scale of 0-1
                        Serial.print(" ptMultiplier="); Serial.print(ptMultiplier);
                        
                        
                        
                      //Serial.print(inBuffer[2]+inBuffer[3]+inBuffer[4]+inBuffer[5]);  //Add up buffer and print
                      Serial.print("<<");
                      Serial.println();
    }
    
    
    
    //Send Buffer to computer
    Serial.println(" Camera says: ");
    for (int i=0; i<ACKlength; i++){
      Serial.print(inBuffer[i], HEX);
      Serial.print("-");  //For readability
    }
    //Serial.flush();  //Flush buffer
    
    Serial.println(); 
  }
  
  /*
  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial3.write(inByte); 
  }
  */
}

void checkSerial(){
  //Initialize array as all 0's
    for (int i=0; i<ACKlength; i++){
      inBuffer[i] = 0;
    }
    
    //Fill array with Serial buffer
    for (int i=0; i<ACKlength; i++){
      inBuffer[i] = Serial3.read();
    }
}

void watchButtons(){
  
  //BUTTON 1
  int button1State=digitalRead(button1);
  if(button1State==HIGH){
    Serial.print("Button 1 high");
    //digitalWrite(ledPin, HIGH);
    //Cancel previous commands
    //sendCommandCancel();
    delay(delayTime);
    //Send zoom tele command
    setPanTiltLimit();
  }
  
    //ZOOM WIDE
  int button2State=digitalRead(button2);
  if(button2State==HIGH){
    Serial.print("Button 2 high");
    //Cancel prev commands
    //sendCommandCancel();
    delay(delayTime);
    //digitalWrite(ledPin, HIGH);
    sendI_PanTiltPos();
  }
  /*
  if(button2State==LOW && button1State==LOW){
    for (int i=0; i<6; i++){
      Serial3.write(zoom_stop[i]);
      //Serial.print(zoom_stop[i]);  //for debugging
    }
  }
  */
}

/*
//Archived function (not used)
//Use this function if you want a numerical value for the pan/Tilt
void readPanTiltValue(){
  //If PTposition packet is detected
    //READ PAN/TILT INFO
    if (inBuffer[0]== 0x90  && inBuffer[1]==0x50 && inBuffer[9]!=0xFF && inBuffer[10]==0xFF){
      Serial.println(" [[ Pan/Tilt packet detected!>>");
      int bufferByte=0;
      int panInquiry=0;
      int tiltInquiry=0;
      int digitPlace=3;
      
      //Read bytes 2 thru 5 for pan position
      for (int i=2; i<6; i++){
        bufferByte = (int) inBuffer[i];
        //Print Raw values
        Serial.print("("); Serial.print(bufferByte); Serial.print(")");
        //Calculate total panInquiry value
        bufferByte= bufferByte*( pow(16,digitPlace) );  //Multiply *16 for each digit place, because it's base 16 (hexadecimal).  Use pow(base,exponent) not ^.... ^ is a bitwise operator!
        //Print modified values
        Serial.print(bufferByte); Serial.print(",");
        panInquiry = panInquiry + bufferByte;  //Add each digit to total
        digitPlace--;  //Decrement digit place 
      }
      
      digitPlace=3;  //Reset digit counter
      //Read bytes 6 thru 9 for pan position
      for (int i=6; i<10; i++){
        bufferByte = (int) inBuffer[i];
        Serial.print("("); Serial.print(bufferByte); Serial.print(")");
        //Calculate total panInquiry value
        bufferByte= bufferByte*( pow(16,digitPlace) );  //Multiply *16 for each digit place, because it's base 16 (hexadecimal).  Use pow(base,exponent) not ^.... ^ is a bitwise operator!
        Serial.print(bufferByte); Serial.print(",");
        tiltInquiry = tiltInquiry + bufferByte;  //Add each digit to total
        digitPlace--;  //Decrement digit place 
      }
      
      Serial.print(" panInquiry (Pan currently reads)="); Serial.print (panInquiry);  //Print totals
          //Tilt total ranges from 
      Serial.print(" tiltInquiry (Tilt currently reads)="); Serial.print (tiltInquiry);  //Print totals
                        //Map number recieved from camera to zoomMultiplier.  Higher zoom should result in slower zooming.
    }
}
*/
