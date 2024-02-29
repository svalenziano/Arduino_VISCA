void resetJoysticks(){
  digitalWrite(inputLed, HIGH);
  //Reset Pot Center Values (assumes joysticks are centered- wiggle them a little bit to make sure)
  panPinCenter= analogRead(panPin);    //The -20 is to compensate for a poorly-centering joystick
  tiltPinCenter= analogRead(tiltPin);
  zoomPinCenter= analogRead(zoomPin);
  Serial.print(" Centers, PTZ="); Serial.print(panPinCenter); Serial.print("-"); Serial.print(tiltPinCenter); Serial.print("-"); Serial.print(zoomPinCenter);
  digitalWrite(inputLed, LOW);
}

 void readPotValues(){
  
  //Read pot values
    //Update old pot values:
    oldPanValue= panValue;
    oldTiltValue= tiltValue;
    oldZoomValue= zoomValue;
    
    panValue= analogRead(panPin);
    tiltValue= analogRead(tiltPin);
    zoomValue= analogRead(zoomPin);
    
    /*
      //RAW VALUES
      Serial.print(" Raw pot values>>> ");
      Serial.print(" panPin= ");Serial.print(panValue);
      Serial.print(" tiltPin= "); Serial.print(tiltValue);
      Serial.print(" zoomPin= "); Serial.print(zoomValue);
      Serial.println();
      */
    
    
    //Normalize pot values Values ... map(value, fromLow, fromHigh, toLow, toHigh))
    panValue= map(panValue, panPinCenter-panPinRange, panPinCenter+panPinRange, -100, 100);
    tiltValue= map(tiltValue, tiltPinCenter-tiltPinRange, tiltPinCenter+tiltPinRange, -100, 100);
    zoomValue= map(zoomValue, zoomPinCenter-zoomPinRange, zoomPinCenter+zoomPinRange, -100, 100);
    //Constrain to avoid out-of-range values
    panValue= constrain(panValue,-100,100);
    tiltValue= constrain(tiltValue,-100,100);
    zoomValue= constrain(zoomValue,-100,100);
    
    /*
          //PROCESSED VALUES
      Serial.print(" [[[Mapped Values >>>");
      Serial.print(" new PTZ values= ");Serial.print(panValue);
      Serial.print(","); Serial.print(tiltValue);
      Serial.print(","); Serial.print(zoomValue);
      //Old values
      Serial.print(" old PTZ values="); Serial.print(oldPanValue); Serial.print(","); Serial.print(oldTiltValue); Serial.print(","); Serial.print (oldZoomValue);
      Serial.println();
      */
      
      //IF INVERT OPTIONS ARE TURNED ON, INVERT VALUES
      if (ptzInverts[0] == true){
        panValue = map(panValue, -100,100, 100, -100);
        //Serial.print(" [panInvert to > ");Serial.print(panValue); Serial.println(" ] ");
      }
      if (ptzInverts[1] == true){
        tiltValue = map(tiltValue, -100, 100, 100, -100);
        //Serial.print(" [tiltInvert to > "); Serial.print(tiltValue); Serial.println(" ] ");
      }
      if (ptzInverts[2] == true){
        zoomValue = map(zoomValue, -100, 100, 100, -100);
      }
    
    

      
      
      
    
    

 }
