#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "CameraHelper.hpp"

AsyncWebServer server(80);
DNSServer dns;

void startCameraServer();
void configCamera();
void setupWebServer();

void configModeCallback(AsyncWiFiManager* myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//flag for saving data
bool shouldSaveConfig = false;

void saveConfigCallback() {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();


  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

  AsyncWiFiManager wifiManager(&server, &dns);

  wifiManager.setAPCallback(configModeCallback);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  // uint16_t chip = (uint16_t)(chipid >> 32);

  // char ssid[17];

  // snprintf(ssid, 17, "Otto-%04X%08X", chip, (uint32_t)chipid);

  String hostName = "otto-tony";

  // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length

  String hostNameBuilder = "<b>" + hostName + "</b>";

  AsyncWiFiManagerParameter hostNameParameter(hostNameBuilder.c_str());
  // AsyncWiFiManagerParameter custom_mqtt_server("Server", "mqtt server", "", 40);
  // AsyncWiFiManagerParameter custom_mqtt_port("Port", "mqtt port", "", 5);
  // AsyncWiFiManagerParameter custom_mqtt_username("Username", "username", "", 16);
  // AsyncWiFiManagerParameter custom_mqtt_password("Password", "password", "", 16);
  // AsyncWiFiManagerParameter custom_mqtt_topic("Topic", "topic", Config::defaultTopic().c_str(), 32);

  //add all your parameters here
  wifiManager.addParameter(&hostNameParameter);
  // wifiManager.addParameter(&custom_mqtt_server);
  // wifiManager.addParameter(&custom_mqtt_port);
  // wifiManager.addParameter(&custom_mqtt_username);
  // wifiManager.addParameter(&custom_mqtt_password);
  // wifiManager.addParameter(&custom_mqtt_topic);

  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID()); // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.print("Host name: ");
  Serial.println(hostName);
  ArduinoOTA.setHostname(hostName.c_str());
  ArduinoOTA.setPassword("123123321");

  ArduinoOTA.onStart([]()
    { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
    { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
    {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    });
  ArduinoOTA.begin();
  Serial.println("OTA ready");

  // Serial.print("Camera Ready! Use 'http://");
  // Serial.print(WiFi.localIP());
  // Serial.println(":8080' to connect");
  setupWebServer();
  configCamera();

  // Start server
  server.begin();
}

// // You can use this function if you'd like to set the pulse length in seconds
// // e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
// void setServoPulse(uint8_t n, double pulse) {
//   double pulselength;

//   pulselength = 1000000;   // 1,000,000 us per second
//   pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
//   Serial.print(pulselength); Serial.println(" us per period");
//   pulselength /= 4096;  // 12 bits of resolution
//   Serial.print(pulselength); Serial.println(" us per bit");
//   pulse *= 1000000;  // convert input seconds to us
//   pulse /= pulselength;
//   Serial.println(pulse);
//   pwm.setPWM(n, 0, pulse);
// }

void loop() {

  ArduinoOTA.handle();
}

CameraHelper* cameraHelper = nullptr;

void configCamera() {
  cameraHelper = new CameraHelper();
  cameraHelper->startCameraServer(&server);
}

void setupWebServer()
{
  MDNS.addService("http", "tcp", 80);

  SPIFFS.begin();

  server.end();

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest* request)
    { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/config", HTTP_GET, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/state", HTTP_POST, [](AsyncWebServerRequest* request)
    {

      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
      delay(1000);
      ESP.restart();
    });

  server.on("/shutdown", HTTP_POST, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/light", HTTP_GET, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/light", HTTP_POST, [](AsyncWebServerRequest* request)
    {
      request->send(200, "application/json", "{\"result\":true}");
    });

  server.on("/update_mqtt", HTTP_POST, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", "{\"result\":true}");

    });

  server.on("/move", HTTP_POST, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", "{\"result\":true}");
    });
}