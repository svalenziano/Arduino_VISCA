
void changeFocus(){
      //Turn on or off focus and exposure
    if (focusActive != lastFocusActive){
      if (focusActive == true){
        sendAutoFocus();
        sendAutoExposure();
      }
      if(focusActive == false){
        sendManualFocus();
        sendManualExposure();
      }
      lastFocusActive= focusActive;
    }
}

void runFocusTimer(){
    //This timer turns off AF and AE when the joysticks haven't been manipulated in a lil while
    if (millis() - focusTimer > focusTimerDuration){
      //Turn off AF and AE
      focusActive = false;
      //Serial.println(" [focusTimer expired] ");
      focusTimer= millis();  //Reset timer
    }
    

}

void resetFocusTimer(){
  focusTimer= millis();  //Reset Timer
  focusIntervalTimer= millis();  //Reset long timer, too
  focusActive= true;  //Make sure autofocus is on
}

void runFocusIntervalTimer(){
      //If the timer is expired
      if (millis() - focusIntervalTimer > focusInterval) {
        //Turn on the AF and AE on for a bit
        focusActive= true;
        gotoHomePosition();  //Go to home position (set via PC_Control)
        delay(500);  //Give camera time to go to home position
        //Serial.println(" [focusIntervalTimer Expired] ");
      }
      if (millis() - focusIntervalTimer > focusInterval+3000){ //Change this value to change how long the AE and AF remain on
        focusActive= false;
        focusIntervalTimer= millis();  //Reset timer
      }
}
