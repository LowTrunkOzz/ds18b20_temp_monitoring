#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiManager.h>
#include <ESP8266HTTPUpdateServer.h>    
#include <ESP8266mDNS.h>                
#include <WiFiClient.h>                 
#include <AutoConnect.h>
#include "config.h"

ESP8266WebServer httpServer;           
ESP8266HTTPUpdateServer httpUpdate;       
AutoConnect portal(httpServer);             
AutoConnectAux update("/update", "UPDATE"); 
AutoConnectAux hello;

// Data wire is plugged into port D2 on the ESP8266
#define ONE_WIRE_BUS D2

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

float tempSensor1, tempSensor2;

uint8_t sensor1[8] = {   };
uint8_t sensor2[8] = {   };
//uint8_t sensor3[8] = {   };


ESP8266WebServer server(serverPort);        


void handle_OnConnect() {
  sensors.requestTemperatures();
  tempSensor1 = sensors.getTempF(sensor1); // Gets the values of the temperature
  tempSensor2 = sensors.getTempF(sensor2); // Gets the values of the temperature
  server.send(200, "text/html", SendHTML(tempSensor1,tempSensor2)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float tempSensor1,float tempSensor2){
  String ptr = "<!DOCTYPE html>";
  ptr +="<html>";
  ptr +="<head>";
  ptr +="<title>SmartHome Tech</title>";
  ptr +="<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  ptr +="<link href='https://fonts.googleapis.com/css?family=Open+Sans:300,400,600' rel='stylesheet'>";
  ptr +="<style>";
  ptr +="html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #444444;}";
  ptr +="body{margin-top: 50px;} ";
  ptr +="h1 {margin: 50px auto 30px;} ";
  ptr +=".side-by-side{display: table-cell;vertical-align: middle;position: relative;}";
  ptr +=".text{font-weight: 600;font-size: 19px;width: 200px;}";
  ptr +=".temperature{font-weight: 300;font-size: 50px;padding-right: 15px;}";
  ptr +=".supply-line .temperature{color: #3B97D3;}";
  ptr +=".return-line .temperature{color: #F29C1F;}";
  ptr +=".superscript{font-size: 17px;font-weight: 600;position: absolute;right: -5px;top: 15px;}";
  ptr +=".data{padding: 10px;}";
  ptr +=".container{display: table;margin: 0 auto;}";
  ptr +=".icon{width:82px}";
  ptr +="</style>";
  ptr +="<script>\n";
  ptr +="setInterval(loadDoc,1000);\n";
  ptr +="function loadDoc() {\n";
  ptr +="var xhttp = new XMLHttpRequest();\n";
  ptr +="xhttp.onreadystatechange = function() {\n";
  ptr +="if (this.readyState == 4 && this.status == 200) {\n";
  ptr +="document.body.innerHTML =this.responseText}\n";
  ptr +="};\n";
  ptr +="xhttp.open(\"GET\", \"/\", true);\n";
  ptr +="xhttp.send();\n";
  ptr +="}\n";
  ptr +="</script>\n";
  ptr +="</head>";
  ptr +="<body>";
  ptr +="<h1>Wood Boiler Temperature Monitor</h1>";
  ptr +="<div class='container'>";
  ptr +="<div class='data supply-line'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<img src='https://img.icons8.com/plasticine/100/000000/left.png'>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Supply Line</div>";
  ptr +="<div class='side-by-side temperature'>";
  ptr +=(int)tempSensor1;
  ptr +="<span class='superscript'>&deg;F</span></div>";
  ptr +="</div>";
  ptr +="<div class='data return-line'>";
  ptr +="<div class='side-by-side icon'>";
  ptr +="<img src='https://img.icons8.com/plasticine/100/000000/right.png'>";
  ptr +="</div>";
  ptr +="<div class='side-by-side text'>Return Line</div>";
  ptr +="<div class='side-by-side temperature'>";
  ptr +=(int)tempSensor2;
  ptr +="<span class='superscript'>&deg;F</span></div>";
  ptr +="</div>";
  ptr +="</div>";
  ptr +="</body>";
  ptr +="</html>";
  return ptr;
}

 
void setup() {
  Serial.begin(115200);
  delay(100);
  
  unsigned long startedAt = millis();
  WiFi.printDiag(Serial); //Remove this line if you do not want to see WiFi password printed
  Serial.println("Opening configuration portal");
  
    //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;  
  //sets timeout in seconds until configuration portal gets turned off.
  //If not specified device will remain in configuration mode until
  //switched off via webserver.
  if (WiFi.SSID()!="") wifiManager.setConfigPortalTimeout(60); //If no access point name has been previously entered disable timeout.
  
  //it starts an access point 
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.startConfigPortal("ESP8266","passw0rd")) {
     Serial.println("Not connected to WiFi but continuing anyway.");
  } else {
     //if you get here you have connected to the WiFi
     Serial.println("connected...yay :)");
     }
   
    Serial.print("After waiting ");
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis()- startedAt);
  Serial.print(waited/1000);
  Serial.print(" secs in setup() connection result is ");
  Serial.println(connRes);
  if (WiFi.status()!=WL_CONNECTED){
    Serial.println("failed to connect, finishing setup anyway");
  } else {
    Serial.print("local ip: ");
    Serial.println(WiFi.localIP());
  }
  
  sensors.begin();              


  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

  httpUpdate.setup(&httpServer, USERNAME, USERPASS); 
  hello.load(HELLO_PAGE);                                
  portal.join({ hello, update });                        
  if (portal.begin()) {                                  
    if (MDNS.begin("esp-webupdate"))                     
        MDNS.addService("http", "tcp", 80);              
  }


}


void loop() {
  server.handleClient();
  MDNS.update();
  portal.handleClient();

}
