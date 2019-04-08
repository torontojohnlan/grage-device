#include <ESP8266WiFi.h> //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>        //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoOTA.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

//this will be converted to config vars
const char *devName="mya";
const char *deviceID="test_device";

char jsonBuf[512];

void setup()
{
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  if (
      setupIO() ||
      setupWifi() ||
      setupOTA() ||
      setupWS())
  {
    ESP.restart();
  }
}

void loop() 
{
  handleOTA();
  handleIO();
  handleWS();
}
