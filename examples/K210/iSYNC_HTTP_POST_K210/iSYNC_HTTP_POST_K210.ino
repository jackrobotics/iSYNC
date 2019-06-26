#include "WiFiEsp.h"
#include <iSYNC.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

WiFiEspClient client;
iSYNC iSYNC(client);

char ssid[] = "HONEYLab";     // your network SSID (name)
char pass[] = "@HONEYLab";    // your network password

String iSYNC_KEY = "5c888df67f56637c67cac702";
String iSYNC_AUTH = "5c888c84d2d6ae7c58c2ba69"; //Auth project


int status = WL_IDLE_STATUS;    // the Wifi radio's status

void setup() {
    Serial.begin(115200);
    Serial.println(iSYNC.getVersion());
    
    Serial1.begin(115200);
    // initialize ESP module
    WiFi.init(&Serial1);
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(ssid, pass);
    }
    Serial.println("You're connected to the network");
}

long last = 0;
void loop() {
    if(millis() - last > 5000){
        last = millis();
        String Temperature=String(random(0,100));
        String Humidity=String(random(0,100));
        String payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
        
        iSYNC.HTTP_POST(iSYNC_KEY,iSYNC_AUTH,payload);
        Serial.print("[iSYNC] HTTP_POST => ");
        Serial.println(payload);
    }
}