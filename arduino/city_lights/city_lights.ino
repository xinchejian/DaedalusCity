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

struct Track
{
  int period;
  const char* track;
};

struct Track Prog1[LightCount] = {
  {100, BLINK_SLOW},
  {100, BLINK_SLOW},
  {100, BLINK_SLOW},
  {100, FADE},
  {100, BLINK FADE "00000000000" FADE},
  {100, "001238763984569361401058123790612430679124360796079341212347129703406794123"},
};

struct Track Prog2[LightCount] = {
  {100, BLINK},
  {100, BLINK_SLOW},
  {100, ON OFF ON ON OFF OFF OFF},
  {100, "4"},
  {100, OFF},
  {100, FADE},
};

struct Track Prog3[LightCount] = {
  {100, "00000000000000000000000000001000000000000000000000000000000000000000000000000000"},
  {100, "00000000000000000000000000000100000000000000000000000000000000000000000000000000"},
  {100, "00000000000000000000000000000010000000000000000000000000000000000000000000000000"},
  {100, "00000000000000000000000000000001000000000000000000000000000000000000000000000000"},
  {100, "00000000000000000000000000000000100000000000000000000000000000000000000000000000"},
  {100, "00000000000000000000000000000000010000000000000000000000000000000000000000000000"},
};

struct Track Prog4[LightCount] = {
  {100, ON},
  {100, ON},
  {100, ON},
  {100, ON},
  {100, ON},
  {100, ON},
};

// Declare one program for each button
struct Track * Programs[ButtonCount] = { Prog1, Prog2, Prog3, Prog4 };

// State variables
struct Track * program = Programs[0];
int time = -1;
int prevLevel[LightCount] = {0};
int lightLevel[LightCount] = {0};
int lightFrame[LightCount] = {-1,-1,-1,-1,-1,-1};
char buttonState[ButtonCount] = {0};
char fading[LightCount] = {0};

// the loop routine runs over and over again forever:
void loop() {  
  
  const int period = ++time % PeriodPWM;

  for (int l = 0; l < LightCount; ++l)
  {
    const int periodInterpolate = program[l].period;

    const int interpolate = (time / PeriodPWM) % periodInterpolate;
    if (period == 0 && interpolate == 0)
    {
      const char * track = program[l].track;
      prevLevel[l] = lightLevel[l];
      char ch;
      do {
        ch = track[++lightFrame[l]];
        switch (ch)
        {
        case '/':
          fading[l] = 1;
          continue;
        case '-':
          fading[l] = 0;
          continue;
        case 0:
          lightFrame[l] = -1;
          continue;
        }
      }
      while (0);
      lightLevel[l] = Periods[ch - '0'];
    }
    
    // TODO: only interpolate in certain cases
    int ll = lightLevel[l];
    if (fading[l])
      ll = (prevLevel[l] * (periodInterpolate - interpolate) + ll * interpolate) / periodInterpolate;
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
    for (int l = 0; l < LightCount; ++l) {
      lightFrame[l] = -1;
      fading[l] = 0;
    }
  }

  //delay(1);
}
