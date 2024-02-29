#include <Streaming.h>

#include <EEPROM.h>


/*
OVERVIEW: This program is used in conjunction with an RS232 / RS485 circuit (for example, MAX232) to 
control a Pan-tilt-zoom camera with the Sony VISCA Protocol.  It has only been tested with 
the Sony EVI-D70 Pan-tilt-zoom camera.

Created by Steven Valenziano, 2013.  All code is public domain.
WRITTEN FOR ARDUINO MEGA (relies on multiple hardware serial ports)
There's a lot of room for improvement.  Please send me your code if you make any changes!

FIRST-TIME SETUP:
  -Ensure you're using an Arduino Mega (uses 2 hardware serial ports)
  -This should work for many Sony VISCA Cameras, but only the EVI-D70 has been tested
  -Connect everything
      -Joysticks to Analog 1, 2, and 3
      -RS232 cable between controller and Camera
      -Power via USB or 12VDC Wallwart
  -Upload code
  -Temporarily clear pan/tilt limits by sending 'C' via the serial monitor
  -Format EEPROM (see "PC_Control" tab for more info)
  -Check joystick orientation, and invert if necessary (see "PC_Control" tab for more info)
  -Set pan/tilt limits (PC_Control tab)
  -Ensure start-up delay is on (PC Control) (you can turn this off to speed up debugging)
  
  
FUTURE IMPROVEMENTS (aka, somebody smarter than me should really do this...):
  -Make all options editable via more intuitive commands via the Serial terminal
  -Simplify code (make faster and more understandable)
  -Turn this into a library that allows multiple cameras.  Library should allow for control of 
      PTZ cameras AND Box cameras (only zoom, no pan/tilt).
  -Make useable with cheaper dev boards/ micros (the Arduino Mega is overkill, since only a few pins are used) - (serial buffer?)
  -Simplify debug info (Serial.print) with Streaming library (http://arduiniana.org/libraries/streaming/ )
  
CHANGELOG
  -v1.02 - added HomePosition function, changed "PC_Control" commands
  
 */
 
//OPTIONS-- CHANGE THESE TO CHANGE THE BEHAVIOR OF THE SYSTEM
  int ptThreshold= 50;  //Pan-tilt Threshold: Minimum amt of pan and tilt that the joystick will respond to
  int zoomThreshold= 55;  //Changes minimum amt of zoom that the joystick will respond to
  int panSpeedLimit= 10;    //Limits pan speed (higher values can be difficult to control) Range is 1-24
  int tiltSpeedLimit= 10;   //Same as above, exept for tilt. Range is 1-23
  int slowZoom= 70;    /*Affects how much pan and tilt slow down when zoomed in.  Prevents uncontrollably fast movement when zoomed in.
                         The closer this value is to 100, the slower zoom will be at telephoto- Range 0-100*/
  const int focusTimerDuration = 20000;  //How long should AF and AE remain active after a person manipulates controls?  In milliseconds. 20000 is 20 seconds.
  int ptzInverts[3] = {-1, -1, -1};  //Joysticks sending camera in wrong direction?- use commands in 'PC_Control' tab to manipulate.  Initialize at -1 for error checking
  const int focusInterval= 120000;  /*While not in use, how often should the camera check in and re-focus/ expose?  In milliseconds. 120000 is 2 minutes.
                                    //THIS NUMBER SHOULD ALWAYS BE LARGER THAN "focusTimerDuration." */
  //OTHER OPTIONS: See "PC_Control" tab  
                                 

//TIMERS
//Timer keeps track of when to send zoom hex codes to camera
  unsigned long zoomSendTimer= 0;  //unsigned, because millis is an unsigned long >>  http://arduino.cc/en/Reference/Millis
  const int zoomSendTimerDuration= 100;  //in Milliseconds
  const int delayTime= 150;
  int zoomSendCount = 0; //counts number of times the zoom has been sent.
  //Focus timer keeps track of when to turn off autofocus and auto-exposure (AF and AE)
  unsigned long focusTimer= 0;
  //See focusTimerDuration in "OPTIONS"
  boolean focusActive = true;  //Keeps track of whether focus should be active or not
  boolean lastFocusActive = false;  //Keeps track of last state of focus active
  //Timer to turn on AE and AF every once in a while
  unsigned long focusIntervalTimer=0;
  //see focusInterval in "OPTIONS"


//INPUT & OUTPUT PINS
//DIGITAL I/O
  const int button1=2;  //Flex button
  const int button2=3;  //flex button
  const int resetButton = 5;  //Reset button - sends reset commands to camera
  const int inputLed= 6;  //LED to indicate input
  const int ledPin =  13;      // the number of the LED pin
//ANALOG-IN
  const int panPin= 0;  //Analog pin for pan potentiometer
  const int tiltPin= 1;  //Analog pin for tilt pot
  const int zoomPin= 2;  //Analog pin for zoom pot

//ANALOG SENSOR READ VALUES
  int panValue= 0;
  int tiltValue= 0;
  int zoomValue= 0;
//Old values, to see if things have changed thru each loop
  int oldPanValue= 0;
  int oldTiltValue= 0;
  int oldZoomValue= 0;

/*ANALOG CALIBRATION VALUES
  Used to calibrate joystick potentiometers
  Center value is what the pot reads when jostick is at center-- 
  ALL CENTER VALUES are reset in setup- no need to set this manually
  Range is the max value (when the joystick is all the way to one side)
  IE: it is (maximum raw potentiometer reading -mininum value)/2  
  */
    int panPinCenter= 0;
    int panPinRange= 325;
    int tiltPinCenter= 0;
    int tiltPinRange= 325;  //300 works
    int zoomPinCenter= 0;
    int zoomPinRange= 180;

//ZOOM VARIABLES
    int zoomDirectValue= 0;  //0000 to 4444 (tele)
//See zoomThreshold in "Options" section
    float zoomMultiplier= 1.0;  //Must be greater than 0 (?)...Larger value= faster zooming.
    const int maxZoom= 4444;  //Value at full tele
    const int maxZoomSpeed= 7;  //zoom speed range is 0 to 7
    byte zByte[6];  //Holds current byte to send to camera
    byte zByteOld[6];  //Holds old bytes to compare.  If new byte is detected, they are sent to camera.

//PAN-TILT VARIABLES
//See ptThreshold in "Options" section
    byte ptByte[9];  //Holds byte to send for panning tilting
    byte ptByteOld[9];  //Holds last sent pan/tilt byte
    float ptMultiplier= 1.0;  //Modifies pan/tilt speed based on zoom (zoomed in results in slower panning)

//SERIAL COMMUNICATION VARIABLES
//These variables are used to create array to hold message from Camera
  const int ACKlength= 14; // MAX length of acknowledge message from camera.  Set to 14 to accommodate 2 7-byte messages.
  byte inBuffer[ACKlength];  //array to hold input from camera

void setup() {  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //Set pin modes
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(resetButton, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(inputLed, OUTPUT);
  
  digitalWrite(inputLed, HIGH);  //Turn on input LED for duration of setup.  Don't touch joysticks until LED is off.
  
  // initialize both serial ports:
  Serial.begin(9600);
  Serial3.begin(9600);
  Serial.println("[SETUP BEGIN...]");
  // For LEONARDO board: while the serial stream is not open, do nothing:
   //while (!Serial) ;
  
    if (EEPROM.read(19)== 1){
    delay(15000);  //Delay 15 seconds to allow camera to get ready
  }
  // Update controller-specific preferences from EEPROM (see "PC_Control tab")
  updatePTZInverts();  //Update invert variable based on EEPROM
  
  resetJoysticks();  //Reset Pot Center Values (assumes joysticks are centered- wiggle them a little bit to make sure)
  startupSequence();  //Send start-up commands to Camera
  printCommands();  //If computer is attached, show debug commands to PC User
  
}




void loop() {   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
  //sendZoom_DirectInOut();  //Test function-- you should run this on it's own, without other code.
  
  //Joystick input
  readPotValues();
 
  //Turn Indicator LED on if recieving input
  if ( abs(tiltValue)>ptThreshold || abs(panValue)>ptThreshold || abs(zoomValue)>zoomThreshold){
    digitalWrite(inputLed, HIGH);
  }
  else {
    digitalWrite( inputLed, LOW);
  }


 serialControl();  //Allows control of camera via Serial Monitor (see PC_Control tab)
 
  //Echo serial communication from camera back to debug computer
  echoSerial();  //see Test_Functions tab
  /* Common return messages
    90-41-FF > Acknowledge receipt  90-51-FF Complete << (Socket 1)
    90-42-FF > Acknowledge receipt  90-52-FF Complete << (Socket 2)
    90-60-03-FF > 2 commands are currently being executed, command could not be accepted
    90-61/62-03-FF> Command not executable (61/62 differentiates between socket #1 and #2)
  */
  
  //Execute startup sequence and reset joystick centers if reset button is pressed
  if (digitalRead(resetButton) == HIGH){
    resetJoysticks();
    startupSequence();
  }
  
  //Functions for test pushbuttons - uncomment this if you are using the hardware buttons
  //watchButtons();  //see Test_Functions tab
  
  //Send commands to Camera, based on Joystick input
  updateSimplePanTilt();
  updateSimpleZoomVar();
  
   //Update timer-based events (based on http://arduino.cc/en/Tutorial/BlinkWithoutDelay )

    if (zoomSendCount < 5){
      if (millis()- zoomSendTimer > 150){
         sendI_ZoomPos();  //Request zoom position from camera
         zoomSendCount++;  //Increment zoomSendCount
         Serial.print(" zoomSendCount="); Serial.print(zoomSendCount);
         zoomSendTimer=millis();  //Re-set timer
         //
      }
      
      Serial.print(" zoomSendCount="); Serial.print(zoomSendCount);
      //zoomSendTimerActive= false;  //Deactivate timer.  Will be re-activated, if necessary, by zoom command
    }
    
    runFocusTimer();
    runFocusIntervalTimer();
    changeFocus();

delay(50);  //This 50ms delay improves communications with camera.  If you leave it out, single messages from the camera will often be interpreted as multiple messages
    

}

//END of LOOP  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

