
/* 
 Based on:
 Stepper Motor Control - one revolution
 
 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe
 
 */

 /**
   Stepper motor control for City project
   First test with pushbuttons for moving a building up and down
   
   http://wiki.xinchejian.com/wiki/City_project
   
   February, 2
   
   Github
   https://github.com/xinchejian/DaedalusCity
   
   Motor will do calibration at startup to calculate the 0 position, travel range.
   
   COMMANDS:
   u##    go up ## steps where ## is an integer number. e.g. u100
   d##    go down ## steps where ## is an integer number. e.g. d50
   h      go home, back to 0 position
   t      go top, to max position
   m      go to mid point
   p##    go to position ##, where ## is an integer number, e.g. p100
   c      do calibration, go home, then top then home.
   r      report range.
   s      stop at current position
   g      let go, release motor
 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor
const int buttonDown = 12; // set button pin for down  pin 12
const int buttonUp = 13; // set button pin for up  pin 13
const int switchStart = 7;
const int switchEnd = 6;
const int LED = 13;
const int reportInterval = 20; // report current position for every 20ms (50Hz), reduce useless traffic

Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

long pos = 0;
long max = 0;
long target = 0;
long reportTime = 0;

int letGo = false;

void setup() {
  Serial.begin(115200);
  
  // Start up blink
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);

  
  // set the speed at 60 rpm:
  myStepper.setSpeed(120);
  pinMode(buttonUp, INPUT); // down pushbutton @ pin 12 as input
  pinMode(buttonDown, INPUT); // up pushbutton @ pin 13 as input
  pinMode(switchStart, INPUT);
  pinMode(switchEnd, INPUT);
  
  // Init loop, go home to find zero, top to find out max range, then back to home again.
  calibrate();
  
  // Report the range and current position.
  reportRange();
  report();
  reportTime = millis();
}

void loop() {
  if (digitalRead(buttonUp)) {
    target = pos + 20;
  }else if (digitalRead(buttonDown)){
    target = pos - 20;
  }
  
  if(Serial.available() > 0) {
    // get incoming byte:
    char cmd = Serial.read();
    int tmp = 0;
    switch(cmd){
      case 'u':
        tmp = Serial.parseInt();
        target = pos + tmp;
        break;
      case 'd':
        tmp = Serial.parseInt();
        target = pos - tmp;
        break;
      case 'h':
        target = 0;
        break;
      case 't':
        target = max;
        break;
      case 'm':
        target = max / 2;
        break;
      case 'p':
        tmp = Serial.parseInt();
        target = tmp;
        break;
      case 'c':
        calibrate();
        break;
      case 'r':
        reportRange();
        break;
      case 's':
        target = pos;
        break;
    }
    if(cmd == 'g'){
      stop();
      letGo = true;
    }else {
      letGo = false;
    }
  }
  
  // If it is in let go status, do not do anything
  if(letGo) continue;
  
  // Move the motor towards the target pos by 1 step.
  if(target > pos){
    up();
  }else if(target < pos){
    down();
  }
  
  // Send it home if it suppose to be at home
  if(pos == 0 && digitalRead(switchStart) == 0) home();
  
  // Report current position for every reportInterval
  // if now < reportTime, we had a overflow. Congrats, the setup is running for more than 50 days if this happens
  long now = millis();
  if(now - reportTime > reportInterval || now < reportTime){ 
    report();
    reportTime = millis();
  }
}

void calibrate(){
  home();
  top();
  home();
}

void up(){
  while(digitalRead(switchEnd) == 0){
    myStepper.step(-1);
    pos ++;
  }
}

void down(){
  while(digitalRead(switchStart) == 0) {
    myStepper.step(1);
    pos --;
  }
}

void home(){
  // Move to 0 position
  while(digitalRead(switchStart) == 0){
    myStepper.step(1);
    pos--;
  }
  pos = 0;
}

void top(){
  // Move to 0 position
  while(digitalRead(switchEnd) == 0){
    myStepper.step(-1);
    pos ++;
  }
  max = pos;
}

void stop(){
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

// Report position to controlling machine.
void report(){
  Serial.println(pos);
}

// Reports max range
void reportRange(){
  Serial.print("Range : ");
  Serial.println(max);
}

