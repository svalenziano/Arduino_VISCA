//ZOOM BYTES//////////////////////////////////////////////////////////////////////////////
 
void sendZoom_Tele(){
 byte zoom_Tele[6]= {0x81,0x01,0x04,0x07,0x02,0xFF};   //Zooms in at fixed speed
 for (int i=0; i<6; i++){
   Serial3.write(zoom_Tele[i]);
 }
 Serial.print(" executed sendZoom_Tele()");
}
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//SIMPLE, NON-DIRECT, 1-SPEED ZOOMING
void updateSimpleZoom(){
    //If zoom adjust value is greater than zoom threshold, determine zoom speed, and send zoom commands
    //If joystick is past zoom in threshhold, zoom in
    if (zoomValue > zoomThreshold){
      sendZoom_Tele();  //Zoom speed doesn't work right now
    }
    //If joystick is past zoom out threshhold, zoom out
    else if (zoomValue< 0-zoomThreshold){
      sendZoom_Wide(); //Zoom out
    }
    else sendZoom_Stop();
}
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
void updateSimpleZoomVar(){  //------------------------------------------------------------------------------------------------------------------
//SIMPLE, NON-DIRECT, VARIABLE-SPEED ZOOMING
    
    String zCommand= "zCommand not set";
    
    //If zoom adjust value is greater than zoom threshold, determine zoom speed, and send zoom commands
    //If joystick is past zoom in threshhold, zoom in
    if ( abs(zoomValue) > zoomThreshold){
          if (zoomValue > zoomThreshold){
            //  Map zoom Value to new zoomSpeed integer wich ranges from 1-7 (min and maxZoomSpeeds)
            int zoomSpeed= map(zoomValue, zoomThreshold, 100, 33,39);  //ints 33-39 correspond to hex 0x21-0x27
            byte zoom_TeleVar[6]= {0x81,0x01,0x04,0x07,0x27,0xFF};  //Zoom In: 81 01 04 07 2x FF , where x is speed- 0(low)-7(hi);
            zoom_TeleVar[4]= zoomSpeed;
            //Update new zoom byte array
            for (int i=0; i<6; i++){
              zByte[i] = zoom_TeleVar[i];
            }
            zCommand= " [Zoom In]";  //debug
            //Serial.print("executed sendZoom_TeleVar"); Serial.print(zoomSpeed, HEX); Serial.print(")");
          }
          
          //If joystick is past zoom out threshhold, zoom out
          else if (zoomValue < -zoomThreshold){
            int zoomSpeed= map(zoomValue, -zoomThreshold, -100, 49,55);  //ints 49-55 correspond to hex 0x31-0x37
            byte zoom_WideVar[6]= {0x81, 0x01, 0x04, 0x07, 0x35, 0xFF};  //Zoom Out: 81 01 04 07 3x FF , where x is speed- 0(low)-7(hi);
             zoom_WideVar[4]= zoomSpeed;
             //Update new zoom byte array
             for (int i=0; i<6; i++){
             zByte[i] = zoom_WideVar[i];
             }
             zCommand = " [Zoom Out]";  //Debug
             //Send debug message with zoomSpeed included:
             //Serial.print("executed sendZoom_WideVar("); Serial.print(zoomSpeed, HEX); Serial.print(")");
          }
    }
    
    //If jostick is in center position, stop zooming
    else { 
    byte zoom_Stop[6]= {0x81, 0x01, 0x04, 0x07, 0x00, 0xFF};  //Stop all zooming
      for (int i=0; i<6; i++){
      zByte[i] = zoom_Stop[i];
      zCommand= " [Zoom Stop]";
      //Get zoom position from Camera
      //sendI_ZoomPos();
      }
    }
    
    //If the new byte is different than the last byte sent to the camera, send the new one.
    if ( arrayCompare(zByte, zByteOld, 6, 6)==false ){
      resetFocusTimer();    //To make sure autofocus is enabled
      for (int i=0; i<6; i++){
        //Send buffer to camera
        Serial3.write( zByte[i] );
        //update old byte for comparison next time around
        zByteOld[i]= zByte[i];
      }
      Serial.print(zCommand);
      Serial.println();
      zoomSendCount = 0;  //Re-start sending zoom inquiries
    }
    
}    //----------------------------------------------------------------------------------------------------------------------------------------------



  
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
void sendZoom_Wide(){
   byte zoom_Wide[6]= {0x81, 0x01, 0x04, 0x07, 0x03, 0xFF};  //Zooms out at fixed speed
   for (int i=0; i<6; i++){
     Serial3.write(zoom_Wide[i]);
   }
   Serial.print(" executed sendZoom_Wide()");
}
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
void sendZoom_WideVar(int zoomSpeed){
   byte zoom_WideVar[6]= {0x81, 0x01, 0x04, 0x07, 0x35, 0xFF};  //Zoom Out: 81 01 04 07 3x FF , where x is speed- 0(low)-7(hi);
   zoom_WideVar[4]= zoomSpeed;
   for (int i=0; i<6; i++){
     Serial3.write(zoom_WideVar[i]);
   }
   //Send debug message with zoomSpeed included:
   Serial.print("executed sendZoom_WideVar("); Serial.print(zoomSpeed, HEX); Serial.print(")");
}
 
 
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
void sendZoom_TeleVar(int zoomSpeed){
  byte zoom_TeleVar[6]= {0x81,0x01,0x04,0x07,0x25,0xFF};  //Zoom In: 81 01 04 07 2x FF , where x is speed- 0(low)-7(hi);
  zoom_TeleVar[4]= zoomSpeed;
  for (int i=0; i<6; i++){
    Serial3.write(zoom_TeleVar[i]);
  }
  Serial.print("executed sendZoom_TeleVar"); Serial.print(zoomSpeed, HEX); Serial.print(")");
}
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
 void sendZoom_Stop(){
   byte zoom_Stop[6]= {0x81, 0x01, 0x04, 0x07, 0x00, 0xFF};  //Stop all zooming
   for (int i=0; i<6; i++){
     Serial3.write(zoom_Stop[i]);
   }
}

//Controls zoom via "Direct" method (sends absolute value to camera).  Doesn't work very well- is very choppy.  May be able to be fixed, I'm not sure how.
void sendZoom_Direct(){    //----------------------------------------------------------------------------------------------------------------------------
  //Serial.print(" zoom_direct[]= ");
  byte zoom_Direct[9]= {0x81, 0x01, 0x04, 0x47,  //Paste into serial terminal: 0x81, 0x01, 0x04, 0x47, 0x04, 0x04, 0x04, 0x04, 0xFF
                        0x04, 0x04, 0x04, 0x04, 0xFF};   //0p, 0q, 0r, 0s, where pqrs=zoom position (Full wide: 0x0000,  Full telephoto: 0x400)
    
    //UPDATE ZOOM DIRECT VALUE
    //If zoom adjust value is greater than zoom threshold, make adjustments to zoomDirectValue
    if (zoomValue > zoomThreshold || zoomValue< 0-zoomThreshold){
      //Update zoomDirectValue using potentiometer reading
      zoomDirectValue= zoomDirectValue + zoomValue*zoomMultiplier;
      //Make sure zoomDirectValue doesn't exceed allowable range
      zoomDirectValue= constrain(zoomDirectValue, 0, maxZoom);
      /*
      if (mode== "valueTest"){
        Serial.print(" zoomDirectValue= "); Serial.print(zoomDirectValue);
      }
      */
    }
    
    //UPDATE 4 DIGIT ZOOM VALUE  
      int digit;  //Temporary variable to hold leftmost digit
    //For each of the 4 digits
    for (int i=0; i<4; i++){
    digit= zoomDirectValue % 10;  //take leftmost digit
    zoomDirectValue= zoomDirectValue /10;  //Cut off leftmost digit
    zoom_Direct[7-i]= digit;  //Update zoom_direct byte array
    /*
    //DEBUG:  WATCH OUT- THIS DEBUG STATEMENT WILL SLOW DOWN THE MOVEMENT OF THE CAMERA
    Serial.print(" ["); Serial.print(7-i); Serial.print("]= "); 
    Serial.print( zoom_Direct[7-i], HEX);
    */
  }
    
  
  Serial.print(" executed updateZoomByte()");
  
  for (int i=0; i<9; i++){
    Serial3.write(zoom_Direct[i]);
    /*
    Serial.print(zoom_Direct[i],HEX);
    Serial.print("-");
    */
  }
  Serial.print(" executed sendZoom_Direct()");
}   //----------------------------------------------------------------------------------------------------------------------------

//Separates digital zoom from optical zoom
void sendDZoom_Separate(){
  byte dZoom_Separate[6]= {0x81, 0x01, 0x04, 0x36, 0x01, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(dZoom_Separate[i]);
  }
  Serial.print(" executed sendDZoom_Separate()");
}

//Turns digital zoom off
void sendDZoom_Off(){
  byte dZoom_Off[6]= {0x81, 0x01, 0x04, 0x06, 0x03, 0xFF};
  for (int i=0; i<6; i++){
    Serial3.write(dZoom_Off[i]);
  }
  Serial.print(" executed sendDZoom_Off");
}


