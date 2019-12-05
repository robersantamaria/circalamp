#define FASTLED_INTERNAL // skip pragma output
#include "FastLED.h"
#include "LedTemplates.h"

#define NUM_LEDS 30
#define LED_STRIP_PIN 5
CRGB leds[NUM_LEDS];
WarmUp lightSet;

#define SERIALSPEED 115200

void setup()
{
  Serial.begin(SERIALSPEED);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  Serial.println("Arduino start!");

  lightSet = WarmUp();
  lightSet.init(leds, NUM_LEDS);
  FastLED.addLeds<WS2811, LED_STRIP_PIN>(leds, NUM_LEDS);
  FastLED.show();
}

void loop()
{
  if (lightSet.update())
  {
    FastLED.show();
  }
}
