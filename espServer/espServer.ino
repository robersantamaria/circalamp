#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "network.h"

#define SERIALSPEED 9600

#define TEXTPLAIN "text/plain"
#define TEXTHTML "text/html"
#define OK 200
#define ACCEPTED 202
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define NOT_ALLOWED 405
#define GWY_TIMEOUT 504

const char *ssid = CIRCALAMP_STASSID;
const char *password = CIRCALAMP_STAPSK;

ESP8266WebServer server(80);

const int led = 13;

String pendingCommand = "";
String serialInput = "";
bool serialComplete = false;
unsigned long lastCharMillis;

void handleRoot()
{
  digitalWrite(led, 1);
  writeSerial("ROOT");
  String out;
  out.reserve(3700);
  out += "<!DOCTYPE html><html> <head> <meta charset=\"utf-8\"/> <title>Circalamp</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"/> <script src=\"https://unpkg.com/axios/dist/axios.min.js\"></script> <script src=\"https://cdn.jsdelivr.net/npm/vue/dist/vue.js\"></script> <style>html{position: relative;}body{background: #ebebeb; margin: 0;}main{padding: 20px;}.main-column{margin-bottom: 100px;}.footer{background-color: #f5f5f5;}.footer > .container{padding-right: 15px; padding-left: 15px;}</style> </head> <body class=\"d-flex flex-column h-100\"> <main role=\"main\" class=\"flex-shrink-0\"> <div id=\"lamp\"> <div>Current time:{{currentTime}}</div><div> Set time: <input v-model=\"newTime\"/> <button v-on:click=\"setTime()\">Set</button> </div><div>Current alarm:{{currentAlarmSwitch ? 'ON' : 'OFF'}}@{{currentAlarmTime}}</div><div> Set alarm: <input v-model=\"newAlarmTime\"/> <input type=\"checkbox\" v-model=\"newAlarmSwitch\"/> <button v-on:click=\"setAlarm()\">Set</button> </div><button v-on:click=\"postDemo('off')\">Demo OFF</button> <button v-on:click=\"postDemo('on')\">Demo ON</button> <div> Output: <ul> <li v-for=\"entry in outputLog\">{{entry}}</li></ul> </div></div></main> <footer class=\"footer mt-auto py-3\"> <div class=\"container\"> <span class=\"text-muted\">...</span> </div></footer> <script>var lamp=new Vue({el: \"#lamp\", data:{outputLog: [\"...\"], currentTime: \"...\", newTime: \"...\", currentAlarmTime: \"...\", currentAlarmSwitch: false, newAlarmTime: \"...\", newAlarmSwitch: false}, created: function(){this.getTime(); this.getAlarm();}, methods:{getTime: function(){axios .get(\"/time\") .then(response=>{var entry=\"GET /time \" + response.statusText; if (!!response.data){entry=entry + \": \" + response.data;}this.outputLog.push(entry); this.currentTime=response.data;}) .catch(e=>{this.outputLog.push(e);});}, setTime: function(){axios .post(\"/time\", \"time=\" + this.newTime) .then(response=>{var entry=\"POST /time \" + response.statusText; if (!!response.data){entry=entry + \": \" + response.data;}this.outputLog.push(entry); this.currentTime=response.data;}) .catch(e=>{this.outputLog.push(e);});}, getAlarm: function(){var alarmResponse; axios .get(\"/alarm\") .then(response=>{var entry=\"GET /alarm \" + response.statusText; if (!!response.data){entry=entry + \": \" + response.data;}this.outputLog.push(entry); alarmResponse=response.data.trim().split(\" \"); this.currentAlarmTime=alarmResponse[0]; this.currentAlarmSwitch=alarmResponse[1].trim()===\"on\";}) .catch(e=>{this.outputLog.push(e);});}, setAlarm: function(){var alarmSwitchText=this.newAlarmSwitch ? \"on\" : \"off\"; axios .post( \"/alarm\", \"alarm_time=\" + this.newAlarmTime + \"&alarm_switch=\" + alarmSwitchText ) .then(response=>{var entry=\"POST /alarm \" + response.statusText; if (!!response.data){entry=entry + \": \" + response.data;}this.outputLog.push(entry); alarmResponse=response.data.trim().split(\" \"); this.currentAlarmTime=alarmResponse[0]; this.currentAlarmSwitch=alarmResponse[1].trim()===\"on\";}) .catch(e=>{this.outputLog.push(e);});}, postDemo: function(mode){axios .post(\"/lights\", \"demo=\" + mode) .then(response=>{var entry=\"POST /lights \" + response.statusText; if (!!response.data){entry=entry + \": \" + response.data;}this.outputLog.push(entry);}) .catch(e=>{this.errors.push(e); this.outputLog.push(e);});}}}); </script> </body></html>\n";
  server.send(OK, TEXTHTML, out);
  digitalWrite(led, 0);
}

void handleTime()
{
  if (server.method() == HTTP_GET)
  {
    waitForCommand("TIME GET");
    writeSerial("TIME GET");
    //    String out = readSerial();
    //    httpSend(OK, TEXTPLAIN, out);
  }
  else if (server.method() == HTTP_POST)
  {
    if (server.hasArg("time"))
    {
      writeSerial("TIME SET " + server.arg("time"));
      waitForCommand("TIME SET");
      writeSerial("TIME GET");
      //      String out = readSerial();
      //      httpSend(ACCEPTED, TEXTPLAIN, out);
    }
    else
    {
      httpSend(BAD_REQUEST, TEXTPLAIN, "Missing time argument");
    }
  }
  else
  {
    httpSend(NOT_ALLOWED, TEXTPLAIN, "");
  }
}

void handleAlarm()
{
  if (server.method() == HTTP_GET)
  {
    waitForCommand("ALARM GET");
    writeSerial("ALARM GET");
    //    String out = readSerial();
    //    httpSend(OK, TEXTPLAIN, out);
  }
  else if (server.method() == HTTP_POST)
  {
    if (server.hasArg("alarm_time") && server.hasArg("alarm_switch"))
    {
      writeSerial("ALARM_TIME SET " + server.arg("alarm_time"));
      writeSerial("ALARM_SWITCH SET " + server.arg("alarm_switch"));
      waitForCommand("ALARM SET");
      writeSerial("ALARM GET");
      //      String out = readSerial();
      //      httpSend(ACCEPTED, TEXTPLAIN, out);
    }
    else
    {
      httpSend(BAD_REQUEST, TEXTPLAIN, "Missing alarm_time or alarm_switch argument");
    }
  }
  else
  {
    httpSend(NOT_ALLOWED, TEXTPLAIN, "");
  }
}

void handleLights()
{
  if (server.method() == HTTP_POST)
  {
    if (server.hasArg("demo"))
    {
      writeSerial("DEMO SET " + server.arg("demo"));
      httpSend(ACCEPTED, TEXTPLAIN, "");
    }
    else
    {
      httpSend(BAD_REQUEST, TEXTPLAIN, "Missing demo argument");
    }
  }
  else
  {
    httpSend(NOT_ALLOWED, TEXTPLAIN, "");
  }
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  httpSend(NOT_FOUND, TEXTPLAIN, message);
}

void writeSerial(String command)
{
  Serial.println(command);
}

String readSerial()
{
  while (!Serial.available())
  {
    delay(50);
  }
  String serialIn = Serial.readString();
  return serialIn;
}

void httpSend(int code, String contentType, String payload)
{
  digitalWrite(led, 1);
  server.send(code, contentType, payload);
  digitalWrite(led, 0);
}

void waitForCommand(String command)
{
  if (pendingCommand != "")
  {
    writeSerial("DBG incoming >" + command + "< on unfinished >" + pendingCommand + "<");
  }
  pendingCommand = command;
  lastCharMillis = millis();
}

void handleCommands(String output)
{
  if (output.startsWith("TIMEOUT"))
  {
    writeSerial("DBG " + output + " on >" + pendingCommand + "<");
    httpSend(GWY_TIMEOUT, TEXTPLAIN, output);
  }
  else if (pendingCommand == "TIME GET" || pendingCommand == "ALARM GET")
  {
    httpSend(OK, TEXTPLAIN, output);
  }
  else if (pendingCommand == "TIME SET" || pendingCommand == "ALARM SET")
  {
    httpSend(ACCEPTED, TEXTPLAIN, output);
  }
  else
  {
    writeSerial("DBG Unexpected >" + output + "< for command >" + pendingCommand + "<");
  }
  pendingCommand = "";
}

void setup(void)
{
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(SERIALSPEED);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("DBG .");
  }
  Serial.print("DBG Connected to ");
  Serial.print(ssid);
  Serial.print(":");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(CIRCALAMP_HOSTNAME))
  {
    Serial.print("DBG MDNS responder started on ");
    Serial.println(CIRCALAMP_HOSTNAME);
  }

  server.on("/", handleRoot);
  server.on("/time", handleTime);
  server.on("/alarm", handleAlarm);
  server.on("/lights", handleLights);

  server.on("/inline", []() {
    server.send(OK, TEXTPLAIN, "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("DBG HTTP server started");
  lastCharMillis = millis();
}

void loop(void)
{
  unsigned long now = millis();
  if (Serial.available())
  {
    char c = (char)Serial.read();
    serialInput += c;
    if (c == '\n')
    {
      serialComplete = true;
    }
    else if (now - lastCharMillis > 2000)
    {
      serialInput = "";
      serialInput += c;
      serialComplete = false;
      if (pendingCommand != "")
      {
        handleCommands("TIMEOUT 1");
      }
    }
    lastCharMillis = millis();
  }
  else if (now - lastCharMillis > 5000 && pendingCommand != "")
  {
    serialInput = "";
    serialComplete = false;
    handleCommands("TIMEOUT 2");
  }

  if (serialComplete)
  {
    serialInput.trim();
    handleCommands(serialInput);
    serialComplete = false;
    serialInput = "";
  }

  if (pendingCommand == "")
  {
    server.handleClient();
  }

  MDNS.update();
}
