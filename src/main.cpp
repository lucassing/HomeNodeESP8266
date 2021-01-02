#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LittleFS.h"
#include <WiFiUdp.h>
#include <TaskScheduler.h>
#include <device.cpp>

/*Put your SSID & Password*/
const char* ssid = getenv("WiFiPass");  // Enter SSID here
const char* password = getenv("WIfiSSID");  //Enter Password here

ESP8266WebServer server(80);

void t2Callback();
Task t2(60000, TASK_FOREVER, &t2Callback);

void t2Callback() {
    Serial.print("t2: ");
    Serial.println(random(10,20));
}

Scheduler runner;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());


  runner.init();
  Serial.println("Initialized scheduler");
  runner.addTask(t2);
  Serial.println("added t2");
  t2.enable();
  Serial.println("Enabled t2");
}

void loop() {
  server.handleClient();
  runner.execute();
}