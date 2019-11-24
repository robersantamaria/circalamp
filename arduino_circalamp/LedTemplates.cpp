#include "Arduino.h"
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "LedTemplates.h"

FirstLight::FirstLight(CRGB *leds, int num_leds)
{
  _leds = leds;
  _num_leds = num_leds;
  _interval = 100;
  _hue = 0;
  _bright = 0;
  _hue_dir = +1;
  _bright_dir = +3;
  this->reset();
}

bool FirstLight::update()
{
  int now = millis();
  if (now + this->_interval >= this->_last_update)
  {
    this->_last_update = now;
  }
  else
  {
    return false;
  }
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
  return true;
}

void FirstLight::reset()
{
  for (int i = 0; i < _num_leds; i++)
  {
    _leds[i] = CHSV(0, 0, 0);
  }
}