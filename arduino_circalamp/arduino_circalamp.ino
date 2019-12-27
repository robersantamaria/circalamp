#include <AltSoftSerial.h>
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
#include <Wire.h>
#include <RTClib.h>

#define FASTLED_INTERNAL // skip pragma output
#include "FastLED.h"
#include "LedTemplates.h"

#ifndef ESP01SPEED
#define ESP01SPEED 9600
#define SERIALSPEED 115200
#define RX_PIN 9 // fixed by AltSoftSerial, cannot be altered (constant unused)
#define TX_PIN 8 // fixed by AltSoftSerial, cannot be altered (constant unused)
#endif

AltSoftSerial esp;
RTC_DS3231 rtc;

int led = 13;
String inputString = "";
bool stringComplete = false;
unsigned long lastEspChar;

struct AlarmSettings
{
  bool enabled;
  char time[6];
};
AlarmSettings alarm;
bool alarmOn = false;
bool alarmRunning = false;

#define NUM_LEDS 30
#define LED_STRIP_PIN 5

CRGB leds[NUM_LEDS];
WarmUp lightSet;

void handleDemo(String command)
{
  if (command == "DEMO SET on")
  {
    Serial.println("START DEMO");
    alarmOn = true;
  }
  else if (command == "DEMO SET off")
  {
    Serial.println("STOP DEMO");
    alarmOn = false;
  }
}

void handleTime(String command)
{
  delay(50);
  if (command.startsWith("TIME GET"))
  {
    DateTime now = rtc.now();
    Serial.println("Sending " + now.timestamp());
    esp.print(now.timestamp());
    esp.write("\n");
  }
  else if (command.startsWith("TIME SET"))
  {
    String dateTime = command.substring(8);
    dateTime.trim();
    int i = 0;
    int j = 0;
    j = dateTime.indexOf('-');
    String year_s = dateTime.substring(i, j);
    int year_i = year_s.toInt();
    i = j + 1;
    j = dateTime.indexOf('-', i);
    String month_s = dateTime.substring(i, j);
    int month_i = month_s.toInt();
    i = j + 1;
    j = dateTime.indexOf('T');
    String day_s = dateTime.substring(i, j);
    int day_i = day_s.toInt();
    i = j + 1;
    j = dateTime.indexOf(':');
    String hour_s = dateTime.substring(i, j);
    int hour_i = hour_s.toInt();
    i = j + 1;
    j = i + 2;
    String minute_s = dateTime.substring(i, j);
    int minute_i = minute_s.toInt();
    DateTime setTime = DateTime(year_i, month_i, day_i, hour_i, minute_i, 0);
    Serial.println("Set Time " + setTime.timestamp());
    rtc.adjust(setTime);
  }
}

void handleAlarm(String command)
{
  if (command.startsWith("ALARM GET"))
  {
    char output[10];
    strcpy(output, alarm.time);
    output[5] = ' ';
    if (alarm.enabled)
    {
      output[6] = 'o';
      output[7] = 'n';
      output[8] = '\0';
    }
    else
    {
      output[6] = 'o';
      output[7] = 'f';
      output[8] = 'f';
      output[9] = '\0';
    }
    Serial.print("Sending ");
    Serial.println(output);
    esp.println(output);
  }
  else if (command.startsWith("ALARM_TIME SET"))
  {
    String newAlarmTime = command.substring(14);
    newAlarmTime.trim();
    int i = newAlarmTime.indexOf(':');
    if (i == 2 & newAlarmTime.length() == 5)
    {
      newAlarmTime.toCharArray(alarm.time, 6);
    }
  }
  else if (command.startsWith("ALARM_SWITCH SET"))
  {
    String newAlarmSwitch = command.substring(16);
    newAlarmSwitch.trim();
    newAlarmSwitch.toLowerCase();
    alarm.enabled = newAlarmSwitch.equals("on");
  }
}

void handleDebug(String command)
{
}

void handleCommands(String command)
{
  Serial.println(">" + command + "<");
  if (command.startsWith("DEMO"))
  {
    handleDemo(command);
  }
  else if (command.startsWith("DBG"))
  {
    handleDebug(command);
  }
  else if (command.startsWith("TIME"))
  {
    handleTime(command);
  }
  else if (command.startsWith("ALARM"))
  {
    handleAlarm(command);
  }
  else
  {
    Serial.println("Invalid command");
    esp.println("ERROR");
  }
}

void setup()
{
  // set the data rate for the SoftwareSerial port
  esp.begin(ESP01SPEED);

  pinMode(led, OUTPUT);
  // Open serial communications and wait for port to open:
  Serial.begin(SERIALSPEED);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  Serial.println("Finding RTC");
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
  }
  Serial.println("RTC Found");

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  else
  {
    Serial.println("RTC time is OK");
  }

  Serial.println("Arduino start!");
  DateTime now = rtc.now();
  Serial.print("Startup time is ");
  Serial.println(now.timestamp());
  char alarmBuf[] = "hh:mm";
  char *currentTime = now.toString(alarmBuf);
  alarm.enabled = false;
  strcpy(alarm.time, currentTime);
  lastEspChar = millis();

  lightSet.init(leds, NUM_LEDS);
  FastLED.addLeds<WS2811, LED_STRIP_PIN>(leds, NUM_LEDS);
  FastLED.show();
}

void loop() // run over and over
{
  if (alarm.enabled & !alarmOn)
  {
    char alarmFormat[] = "hh:mm";
    DateTime now = rtc.now();
    char *currentTime = now.toString(alarmFormat);
    if (strcmp(currentTime, alarm.time) == 0)
    {
      alarmOn = true;
    }
  }

  if (alarmOn)
  {
    digitalWrite(led, HIGH);
    alarmRunning = true;
    if (lightSet.update())
    {
      FastLED.show();
    };

    if (lightSet.over())
    {
      alarmOn = false;
    }
  }

  if (alarmRunning)
  {
    if (!alarmOn)
    {
      digitalWrite(led, LOW);
      lightSet.reset();
      FastLED.show();
      alarmRunning = false;
    }
  }

  if (esp.available())
  {
    char c = (char)esp.read();
    inputString += c;
    // Serial.write(c);
    if (c == '\n')
    {
      stringComplete = true;
    }
    else
    {
      unsigned long now = millis();
      if (now - lastEspChar > 2000)
      {
        inputString = "";
        inputString += c;
        stringComplete = false;
      }
    }
    lastEspChar = millis();
  }
  if (stringComplete)
  {
    inputString.trim();
    handleCommands(inputString);
    stringComplete = false;
    inputString = "";
  }

  // if (Serial.available())
  //   esp.write(Serial.read());
}
