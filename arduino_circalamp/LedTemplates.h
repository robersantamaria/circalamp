#ifndef LedTemplates_h
#define LedTemplates_h

class FirstLight
{
public:
  FirstLight(CRGB *leds, int num_leds);
  void update();
  void reset();

private:
  int _num_leds;
  CRGB *_leds;
  uint8_t _hue;
  uint8_t _bright;
  int _hue_dir;
  int _bright_dir;
};

#endif