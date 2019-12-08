
#include <EEPROM.h>

#define SERIALSPEED 115200

#define ALARM_EEPROM_ADDRESS 0
struct AlarmSettings
{
  bool enabled;
  char time[5];
};

AlarmSettings alarm;

void setup()
{
  Serial.begin(SERIALSPEED);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  Serial.println("Arduino start!");

  Serial.print("sizeof: ");
  Serial.println(sizeof(alarm));

  EEPROM.get(ALARM_EEPROM_ADDRESS, alarm);
  Serial.print("Alarm switch: ");
  Serial.println(alarm.enabled);
  Serial.print("Alarm time: ");
  Serial.println(alarm.time);

  // delay(10000);
  alarm.enabled = true;
  String time = "21:16";
  time.toCharArray(alarm.time, time.length() + 1);

  EEPROM.put(ALARM_EEPROM_ADDRESS, alarm);
}

void loop()
{
}
