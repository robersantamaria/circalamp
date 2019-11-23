#include "Arduino.h"
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "LedTemplates.h"

FirstLight::FirstLight(CRGB *leds, int num_leds)
{
  _leds = leds;
  _num_leds = num_leds;
  _hue = 0;
  _bright = 0;
  _hue_dir = +1;
  _bright_dir = +3;
}

void FirstLight::update()
{
  _hue = _hue + _hue_dir;
  if (_hue >= 255 || _hue <= 0)
  {
    _hue_dir = -_hue_dir;
  }
  _bright = _bright + _bright_dir;
  if (_bright >= 255 || _bright <= 0)
  {
    _bright_dir = -_bright_dir;
  }
  for (int i = 0; i < _num_leds; i++)
  {
    _leds[i] = CHSV(_hue, 255, _bright);
  }
}

void FirstLight::reset()
{
  for (int i = 0; i < _num_leds; i++)
  {
    _leds[i] = CHSV(0, 0, 0);
  }
}