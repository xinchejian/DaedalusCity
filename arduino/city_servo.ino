
/* 
 Based on:
 Stepper Motor Control - one revolution
 
 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe
 
 */
 
 /*
   Stepper motor control for City project
   First test with pushbuttons for moving a building up and down
   
   http://wiki.xinchejian.com/wiki/City_project
   
   February, 2
   
 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor
const int buttonDown = 12; // set button pin for down  pin 12
const int buttonUp = 13; // set button pin for up  pin 13
const int switchStart = 7;
const int switchEnd = 6;
const int LED = 13;

Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

long pos = 0;
long max = 0;

void setup() {
  Serial.begin(115200);
  
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
  
  home();
}

void loop() {

  if (digitalRead(buttonUp)) {
    up();
  }else if (digitalRead(buttonDown)){
    down();
  }
  
  if(Serial.available() > 0) {
    // get incoming byte:
    char cmd = Serial.read();
    switch(cmd){
      case 'u':
        up();
        break;
      case 'd':
        down();
        break;
      case 'h':
        home();
        break;
      case 't':
        top();
        break;
      case 'm':
        center();
        break;
    }
    // Serial.println(pos); // comment by lumi
  }
  //stop();
}

void up(){
  int t = 10;
  while(digitalRead(switchEnd) == 0 && t-- > 0){
    myStepper.step(-1);
    pos ++;
    serial(); // added by lumi
  }
}

void down(){
  int t = 10;
  while(digitalRead(switchStart) == 0 && t-- > 0) {
    myStepper.step(1);
    pos --;
    serial(); // added by lumi
  }
}

void home(){
  // Move to 0 position
  while(digitalRead(switchStart) == 0){
    myStepper.step(1);
    pos--;
    serial(); // added by lumi
  }
  pos = 0;
  // serial(); // comment by lumi
}

void top(){
  // Move to 0 position
  while(digitalRead(switchEnd) == 0){
    myStepper.step(-1);
    pos ++;
    serial(); // added by lumi
  }
  max = pos;
}

// Got to the center position, WARNING: NO CHECKING
void center(){
  long middle = max / 2;
  myStepper.step(pos - middle);
  
  pos = middle;
}

void stop(){
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}
// for easy use. the serial function is here
void serial(){
  // serial commands for Touch Designer
    Serial.print("Position=");
    Serial.print(pos);
    Serial.print('\n');
    // serial end
}

