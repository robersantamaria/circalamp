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
#define DEMO_LED_PIN 13

CRGB leds[NUM_LEDS];
WarmUp lightSet;

#define BUTTON_PIN 3
int lastButtonState = LOW;
int lastDebouncedState = LOW;
int currentDebouncedState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // in msec

#define ESP_RESET_PIN 4

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
  else if (command.startsWith("ROOT"))
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

void kernelPanic()
{
  int ledState = LOW;
  while (true)
  {
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
    digitalWrite(DEMO_LED_PIN, ledState);
    delay(200);
  }
}

void setup()
{
  pinMode(DEMO_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(ESP_RESET_PIN, OUTPUT);
  digitalWrite(ESP_RESET_PIN, HIGH);

  esp.begin(ESP01SPEED);
  Serial.begin(SERIALSPEED);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Native USB only
  }

  Serial.print("Searching RTC... ");
  if (!rtc.begin())
  {
    Serial.println("couldn't find RTC, stopping.");
    kernelPanic();
  }
  Serial.print("found; ");

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting clock to compilation time.");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  else
  {
    Serial.println("time is OK.");
  }

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

  delay(1000);
  Serial.print("Resetting ESP... ");
  digitalWrite(ESP_RESET_PIN, LOW);
  delay(50);
  digitalWrite(ESP_RESET_PIN, HIGH);
  Serial.println("done.");
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

  int buttonReading = digitalRead(BUTTON_PIN);
  if (buttonReading != lastButtonState)
  {
    lastDebounceTime = millis();
  }
  lastButtonState = buttonReading;

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    currentDebouncedState = buttonReading;
  }

  if (lastDebouncedState == LOW & currentDebouncedState == HIGH)
  {
    lastDebouncedState = HIGH;
    Serial.println("Button pressed, holding on.");
  }
  if (lastDebouncedState == HIGH & currentDebouncedState == LOW)
  {
    lastDebouncedState = LOW;
    Serial.println("Button unpressed, switching alarm off.");
    alarmOn = false;
  }

  if (alarmOn)
  {
    digitalWrite(DEMO_LED_PIN, HIGH);
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
      digitalWrite(DEMO_LED_PIN, LOW);
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
