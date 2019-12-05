#include "Arduino.h"
#define FASTLED_INTERNAL
#include "FastLED.h"
#include "LedTemplates.h"

void CircalampLightSet::init(CRGB *leds, int num_leds)
{
  _leds = leds;
  _num_leds = num_leds;
  this->reset();
};

void CircalampLightSet::setInterval(unsigned int interval)
{
  _interval = interval;
}

bool CircalampLightSet::needsUpdate()
{
  unsigned long now = millis();
  if (now >= this->_last_update + this->_interval)
  {
    this->_last_update = now;
    return true;
  }
  else
  {
    return false;
  }
}

void CircalampLightSet::reset()
{
  for (int i = 0; i < _num_leds; i++)
  {
    _leds[i] = CHSV(0, 0, 0);
  }
}

// ********************
// *    FirstLight    *
// ********************

void FirstLight::reset()
{
  _hue = 0;
  _bright = 0;
  _hue_dir = +1;
  _bright_dir = +3;
  this->setInterval(FIRST_LIGHT_INTERVAL);
  CircalampLightSet::reset();
}

bool FirstLight::update()
{
  if (this->needsUpdate())
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
    return true;
  }
  return false;
}

// ****************
// *    WarmUp    *
// ****************

void WarmUp::reset()
{
  _i = -1;
  _active_leds = 0;
  _saturation = 255;
  this->setInterval(WARMUP_INTERVAL);
  CircalampLightSet::reset();
}

bool WarmUp::update()
{
  if (this->needsUpdate())
  {
    _i = _i + 1;

    if (_i % WARMUP_LED_INTERVAL == 0)
    {
      _active_leds = _active_leds + 1;
      if (_active_leds >= _num_leds)
      {
        _saturation = _saturation - 1;
        _active_leds = _num_leds;
      }
    }

    if (_saturation < 0)
    {
      _saturation = 0;
    }

    for (int n = 0; n < _active_leds; n++)
    {
      int _brightness = (_i - (n * WARMUP_LED_INTERVAL)) / WARMUP_BRIGHTNESS_INTERVAL;
      if (_brightness < 0)
      {
        _brightness = 0;
      }
      else if (_brightness >= 255)
      {
        _brightness = 255;
      }

      _leds[n] = CHSV(HSV_RED_VALUE, _saturation, _brightness);
    }
    return true;
  }
  return false;
}

void Rainbow::reset()
{
  this->setInterval(RAINBOW_INTERVAL);
  _state_n = 0;
  _states[0] = CHSV(0, 0, 0); // val = 0 > LED off
  _states[1] = CHSV(0, 64, 0);
  _states[2] = CHSV(0, 128, 0);
  _states[3] = CHSV(0, 192, 0);
  _states[4] = CHSV(0, 255, 0);
  _states[5] = CHSV(0, 0, 64); // sat = 0 > white color
  _states[6] = CHSV(0, 64, 64);
  _states[7] = CHSV(0, 128, 64); // val low: LED on
  _states[8] = CHSV(0, 192, 64);
  _states[9] = CHSV(0, 255, 64); // sat = 255 > intense color
  _states[10] = CHSV(0, 0, 128);
  _states[11] = CHSV(0, 64, 128);
  _states[12] = CHSV(0, 128, 128);
  _states[13] = CHSV(0, 192, 128);
  _states[14] = CHSV(0, 255, 128);
  _states[15] = CHSV(0, 0, 192);
  _states[16] = CHSV(0, 64, 192);
  _states[17] = CHSV(0, 128, 192);
  _states[18] = CHSV(0, 192, 192);
  _states[19] = CHSV(0, 255, 192);
  _states[20] = CHSV(0, 0, 255); // val high: intense light
  _states[21] = CHSV(0, 64, 255);
  _states[22] = CHSV(0, 128, 255);
  _states[23] = CHSV(0, 192, 255);
  _states[24] = CHSV(0, 255, 255);

  CircalampLightSet::reset();
}

bool Rainbow::update()
{
  if (this->needsUpdate())
  {
    _state_n = _state_n + 1;
    if (_state_n >= RAINBOW_STATES)
    {
      _state_n = 0;
    }

    CHSV led_state = _states[_state_n];
    for (int n = 0; n < 16; n++)
    {
      led_state.hue = n * 16;
      _leds[n] = led_state;
    }
    return true;
  }
  return false;
}
