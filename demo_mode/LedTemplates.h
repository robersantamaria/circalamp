#ifndef LedTemplates_h
#define LedTemplates_h

#define HSV_YELLOW_VALUE 64
#define HSV_ORANGE_VALUE 80
#define HSV_RED_VALUE 96

class CircalampLightSet
{
public:
  void init(CRGB *leds, int num_leds);
  virtual bool update();
  virtual void reset();
  virtual bool over();

protected:
  void setInterval(unsigned int interval);
  bool needsUpdate();
  int _num_leds;
  CRGB *_leds;
  unsigned long _last_update;

private:
  unsigned int _interval; // in millis
};

#define FIRST_LIGHT_INTERVAL 100 // in millis

class FirstLight : public CircalampLightSet
{
public:
  virtual bool update();
  virtual void reset();
  virtual bool over();

private:
  uint8_t _hue;
  uint8_t _bright;
  int _hue_dir;
  int _bright_dir;
};

#define WARMUP_INTERVAL 1000         // in millis (default to 1000)
#define WARMUP_LED_INTERVAL 60       // one more led each minute (for 1 sec interval)
#define WARMUP_BRIGHTNESS_INTERVAL 6 // reach full brightness in 25 min (for 1 sec interval)
#define WARMUP_STATIC_DURATION 300   // 5 min at top brightness (for 1 sec interval)

class WarmUp : public CircalampLightSet
{
public:
  virtual bool update();
  virtual void reset();
  virtual bool over();

private:
  int _i;
  int _active_leds;
  int _saturation;
  int _static_duration;
};

#define RAINBOW_INTERVAL 5000 // in millis
#define RAINBOW_STATES 25

class Rainbow : public CircalampLightSet
{
public:
  virtual bool update();
  virtual void reset();
  virtual bool over();

private:
  int _state_n;
  CHSV _states[RAINBOW_STATES];
};

#endif
