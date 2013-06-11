#include <serial.h>

const int FirstLightPin = 2;
const int LightCount = 6;

const int FirstButtonPin = 8;
const int ButtonCount = 4;

#define PWMLOG 5
const int Periods[10] = {0, 1, 2, 4, 6, 10, 14, 19, 25, 1 << PWMLOG};

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
  unsigned char period;
  const char* track;
};

struct Track Prog1[LightCount] = {
  {1, BLINK},
  {2, BLINK},
  {3, BLINK},
  {4, BLINK},
  {5, BLINK},
  {6, BLINK},
};

struct Track Prog2[LightCount] = {
  {3, "1"},
  {3, "2"},
  {3, "3"},
  {3, "4"},
  {7, "/09"},
  {4, "/"FADE},
};

struct Track Prog3[LightCount] = {
  {3, "00000000000000000000000000001000000000000000000000000000000000000000000000000000"},
  {3, "00000000000000000000000000000100000000000000000000000000000000000000000000000000"},
  {3, "00000000000000000000000000000010000000000000000000000000000000000000000000000000"},
  {3, "00000000000000000000000000000001000000000000000000000000000000000000000000000000"},
  {3, "00000000000000000000000000000000100000000000000000000000000000000000000000000000"},
  {3, "00000000000000000000000000000000010000000000000000000000000000000000000000000000"},
};

struct Track Prog4[LightCount] = {
  {3, ON},
  {3, ON},
  {3, ON},
  {3, ON},
  {3, ON},
  {3, ON},
};

// Declare one program for each button
struct Track * Programs[ButtonCount] = { Prog1, Prog2, Prog3, Prog4 };

// State variables
struct Track * program;
unsigned long time = ~0;

struct Light {
  char prev, level, fading;
  unsigned int frame;
};
Light lights[LightCount] = {0};

char buttonState[ButtonCount] = {0};

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
  program = Prog1;
  // for debugging  
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {  
  
  const int period = ++time & ((1 << PWMLOG) - 1);

  for (int l = 0; l < LightCount; ++l)
  {
    struct Light &light = lights[l];
    struct Track &track = program[l];
    
    const int periodLog = track.period;
    const int interpolate = (time >> PWMLOG) & ((1 << periodLog) - 1);
    if (period == 0 && interpolate == 0)
    {
      const char * track = program[l].track;
      char ch;
      while(1) 
      {
        ch = track[++light.frame];
        switch (ch)
        {
        case '/':
          light.fading = 1;
          continue;
        case '-':
          light.fading = 0;
          continue;
        case 0:
          light.frame = -1;
          continue;
        }
        break;
      }
      int p = Periods[ch - '0'];
      light.prev = light.fading ? light.level : p;
      light.level = p;
    }
    
    int ll = light.prev + (((light.level - light.prev) * interpolate) >> periodLog);
    //Serial.println(ll);
    digitalWrite(FirstLightPin + l, ll > period);
  }

  if (period == 0)
  {
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
        lights[l].frame = -1;
        lights[l].fading = 0;
      }
    }
  }

  //delay(1);
}
