#include <serial.h>

const int FirstLightPin = 2;
const int LightCount = 6;

const int FirstButtonPin = 8;
const int ButtonCount = 4;

const int PeriodPWM = 16;
const int PeriodInterpolate = 64;
const int Periods[10] = {0, 1, 2, 3, 4, 5, 6, 8, 11, PeriodPWM};

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pins for the lights as outputs.
  for (int led = 0; led < LightCount; ++led)
  { 
    pinMode(FirstLightPin + led, OUTPUT);
  }    
  // initialize the digital pins for the buttons as inputs
  for (int but = 0; but < ButtonCount; ++but)
  { 
    pinMode(FirstButtonPin + but, INPUT_PULLUP);
  }
  // for debugging  
  Serial.begin(9600);
}

// Predefined patterns:
#define ON "9"
#define OFF "0"
#define FADE "012345678987654321"
#define BLINK ON OFF
#define BLINK_SLOW ON ON OFF OFF
#define RANDOM  "9099990099090990000090990009999000999090999909009090900900099909090999090909009090000099090000900009"
#define RANDOM2 "3675356291270936062618792023759228973612931947845036106320615547656937452547443078688431492068926649"

// The programs:

const char* const Prog1[LightCount] = {
  BLINK_SLOW,
  BLINK_SLOW,
  BLINK_SLOW,
  FADE,
  BLINK FADE "00000000000" FADE,
  "001238763984569361401058123790612430679124360796079341212347129703406794123",
};

const char* const Prog2[LightCount] = {
  BLINK,
  BLINK_SLOW,
  ON OFF ON ON OFF OFF OFF ,
  "4",
  OFF,
  FADE,
};

const char* const Prog3[LightCount] = {
  "00000000000000000000000000001000000000000000000000000000000000000000000000000000",
  "00000000000000000000000000000100000000000000000000000000000000000000000000000000",
  "00000000000000000000000000000010000000000000000000000000000000000000000000000000",
  "00000000000000000000000000000001000000000000000000000000000000000000000000000000",
  "00000000000000000000000000000000100000000000000000000000000000000000000000000000",
  "00000000000000000000000000000000010000000000000000000000000000000000000000000000",
};

const char* const Prog4[LightCount] = {
  ON,
  ON,
  ON,
  ON,
  ON,
  ON,
};

// Declare one program for each button
const char* const* const Programs[ButtonCount] = { Prog1, Prog2, Prog3, Prog4 };

// State variables
const char* const* program = Programs[0];
int time = PeriodInterpolate * PeriodPWM;
int prevLevel[LightCount] = {0,0,0,0,0,0};
int lightLevel[LightCount] = {0,0,0,0,0,0};
int lightFrame[LightCount] = {-1,-1,-1,-1,-1,-1};
int interpolate = 0;
char buttonState[ButtonCount] = {0,0,0,0};

// the loop routine runs over and over again forever:
void loop() {  
  
  const int period = ++time % PeriodPWM;
  const int interpolate = time / PeriodPWM;

  if (interpolate == PeriodInterpolate)
  {
    time = 0;
    for (int l = 0; l < LightCount; ++l) 
    {
      prevLevel[l] = lightLevel[l];
      char ch = program[l][++lightFrame[l]];
      if (ch == 0)
        ch = program[l][lightFrame[l] = 0];
      lightLevel[l] = Periods[ch - '0'];
    }
  }
  
  for (int l = 0; l < LightCount; ++l)
  {
    int ll = (prevLevel[l] * (PeriodInterpolate - interpolate) + lightLevel[l] * interpolate) / PeriodInterpolate;
    //Serial.println(ll);
    digitalWrite(FirstLightPin + l, ll > period);
  }

  int pressed = -1;
  for (int but = 0; but < ButtonCount; ++but)
  {
    char prev = buttonState[but];
    char now = buttonState[but] = (digitalRead(FirstButtonPin + but) == HIGH);
    if (!prev && now)
      pressed = but;
  }
  if (pressed >= 0)
  {
    program = Programs[pressed];
    for (int l = 0; l < LightCount; ++l)
      lightFrame[l] = -1;
  }

  //delay(1);
}
