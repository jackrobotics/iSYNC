#include <WiFiClientSecure.h>
#include <iSYNC.h>

WiFiClientSecure clientSecure;
iSYNC iSYNC(clientSecure);

String ssid = "HONEYLab";
String pass = "@HONEYLab";

String iSYNC_USERNAME = "admin";
String iSYNC_KEY = "5c888df67f56637c67cac702";
String iSYNC_AUTH = "5c888c84d2d6ae7c58c2ba69"; //auth project

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print("[iSYNC]-> ");
	for (int i = 0; i < length; i++)Serial.print((char)payload[i]);
	Serial.println();
}

void connectMQTT(){
	while(!iSYNC.mqConnect()){
		Serial.println("Reconnect MQTT...");
		delay(3000);
	}
	iSYNC.mqSubProject(); //subscribe all key in your project
//  iSYNC.mqSub(iSYNC_KEY); //subscribe key
}

void setup() {
		Serial.begin(115200);
		Serial.println(iSYNC.getVersion());

		iSYNC.begin(ssid,pass);
		iSYNC.mqInit(iSYNC_USERNAME,iSYNC_AUTH,true);
		iSYNC.MQTT->setCallback(callback);
		connectMQTT();
}

long last = 0;
void loop() {
		if (!iSYNC.mqConnected())connectMQTT();
		iSYNC.mqLoop();

		if (millis() - last > 5000) {
				last = millis();
				String payload="{\"Temperature\":"+String(random(0,100))+",\"Humidity\":"+String(random(0,100))+"}";
				Serial.println("[iSYNC]<- "+payload);
				iSYNC.mqPub(iSYNC_KEY,payload);   //Publish
		}
}