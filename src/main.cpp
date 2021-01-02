#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LittleFS.h"
#include <WiFiUdp.h>
#include <TaskScheduler.h>
#include <device.cpp>

/*Put your SSID & Password*/
const char* ssid = "";  // Enter SSID here
const char* password = "";  //Enter Password here

ESP8266WebServer server(80);

void t2Callback();
Task t2(60000, TASK_FOREVER, &t2Callback);

void t2Callback() {
    Serial.print("t2: ");
    Serial.println(random(10,20));
}

Scheduler runner;

uint8_t LED1pin = D4;
bool LED1status = LOW;

uint8_t LED2pin = D6;
bool LED2status = LOW;

String webhtml = "";

Device bombaDeAgua(D0);
unsigned long startMillis; 
unsigned long currentMillis;
const unsigned long period = 60000;

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Sistema de Riego Automatizado</h1>\n";
  ptr +="<h3>Nodo 1</h3>\n";
  
  if(led1stat)
  {ptr +="<p>Estado Bomba: Encendida</p><a class=\"button button-off\" href=\"/led1off\">Apagar</a>\n";}
  else
  {ptr +="<p>Estado Bomba: Apagada</p><a class=\"button button-on\" href=\"/led1on\">Encender</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


void handle_OnConnect() {
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(bombaDeAgua.getPinState(),LED2status)); 
}

void handle_led1on() {
  bombaDeAgua.setPinState(HIGH);
  server.send(200, "text/html", SendHTML(bombaDeAgua.getPinState(),LED2status)); 
}

void handle_led1off() {
  bombaDeAgua.setPinState(LOW);
  server.send(200, "text/html", SendHTML(bombaDeAgua.getPinState(),LED2status)); 
}

void handle_status() {
  int status = digitalRead(LED1pin);
  Serial.println("GPIO0 Status:");
  Serial.print(status);
  server.send(200, "text/html", webhtml); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

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

  Serial.println(F("Inizializing FS..."));
  if (LittleFS.begin()){
      Serial.println(F("done."));
  }else{
      Serial.println(F("fail."));
  }

      FSInfo fs_info;
    LittleFS.info(fs_info);
 
    Serial.println("File sistem info.");
 
    Serial.print("Total space:      ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");
 
    Serial.print("Total space used: ");
    Serial.print(fs_info.usedBytes);
    Serial.println("byte");
 
    Serial.print("Block size:       ");
    Serial.print(fs_info.blockSize);
    Serial.println("byte");
 
    Serial.print("Page size:        ");
    Serial.print(fs_info.totalBytes);
    Serial.println("byte");
 
    Serial.print("Max open files:   ");
    Serial.println(fs_info.maxOpenFiles);
 
    Serial.print("Max path lenght:  ");
    Serial.println(fs_info.maxPathLength);
 
    Serial.println();
 

    File file = LittleFS.open("/index.html","r");
    while(file.available()){
      webhtml+=(char)file.read();
    }
    file.close();

    // File jsfile = LittleFS.open("/index.js","r");
    // while(jsfile.available()){
    //   webjs+=(char)jsfile.read();
    // }
    // jsfile.close();
    
  server.serveStatic("/index.js", LittleFS, "/index.js", "max-age=43200");
  server.on("/", handle_OnConnect);
  server.on("/status", handle_status);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
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