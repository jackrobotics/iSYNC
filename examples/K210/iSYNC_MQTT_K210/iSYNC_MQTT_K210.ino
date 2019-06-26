#include "WiFiEsp.h"
#include <iSYNC.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

WiFiEspClient espClient;
iSYNC iSYNC(espClient);

char ssid[] = "HONEYLab";  // your network SSID (name)
char pass[] = "@HONEYLab"; // your network password

String iSYNC_USERNAME = "admin";
String iSYNC_KEY = "5c888df67f56637c67cac702";
String iSYNC_AUTH = "5c888c84d2d6ae7c58c2ba69"; //auth project

int status = WL_IDLE_STATUS; // the Wifi radio's status

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("[iSYNC]-> ");
    for (int i = 0; i < length; i++)
        Serial.print((char)payload[i]);
    Serial.println();
}

void connectMQTT()
{
    while (!iSYNC.mqConnect())
    {
        Serial.println("Reconnect MQTT...");
        delay(3000);
    }
    iSYNC.mqSubProject(); //subscribe all key in your project
}

void setup()
{
    Serial.begin(115200);
    Serial.println(iSYNC.getVersion());

    Serial1.begin(115200);
    WiFi.init(&Serial1); // initialize ESP module
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network
        status = WiFi.begin(ssid, pass);
    }
    Serial.println("You're connected to the network");

    iSYNC.mqInit(iSYNC_USERNAME, iSYNC_AUTH);
    iSYNC.MQTT->setCallback(callback);
    connectMQTT();
}

long last = 0;
void loop()
{
    if (!iSYNC.mqConnected())connectMQTT();
    iSYNC.mqLoop();

    if (millis() - last > 5000)
    {
        last = millis();
        String payload = "{\"Temperature\":" + String(random(0, 100)) + ",\"Humidity\":" + String(random(0, 100)) + "}";
        Serial.println("[iSYNC]<- " + payload);
        iSYNC.mqPub(iSYNC_KEY, payload); //Publish
    }
}