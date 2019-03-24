#include <SPI.h>
#include <Ethernet.h>
#include <iSYNC.h>

EthernetClient client;
iSYNC iSYNC(client);

String iSYNC_USERNAME = "admin";
String iSYNC_KEY = "5c888df67f56637c67cac702";
String iSYNC_AUTH = "5c888c84d2d6ae7c58c2ba69"; //auth project

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

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
	//  iSYNC.mqSub(iSYNC_KEY); //subscribe key
}

void setup()
{
	Serial.begin(115200);
	Serial.println(iSYNC.getVersion());

	Serial.println("Initialize Ethernet with DHCP:");
	if (Ethernet.begin(mac) == 0){
		Serial.println("Failed to configure Ethernet using DHCP");
		// Check for Ethernet hardware present
		if (Ethernet.hardwareStatus() == EthernetNoHardware){
			Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
			while (true){
				delay(1); // do nothing, no point running without Ethernet hardware
			}
		}
		if (Ethernet.linkStatus() == LinkOFF){
			Serial.println("Ethernet cable is not connected.");
		}
	}else{
		Serial.print("  DHCP assigned IP ");
		Serial.println(Ethernet.localIP());
	}
	iSYNC.mqInit(iSYNC_USERNAME, iSYNC_AUTH);
	iSYNC.mqCallback(callback);
	connectMQTT();
}

long last = 0;
void loop()
{
	if (!iSYNC.mqConnected())
		connectMQTT();
	iSYNC.mqLoop();

	if (millis() - last > 5000)
	{
		last = millis();
		String payload = "{\"Temperature\":" + String(random(0, 100)) + ",\"Humidity\":" + String(random(0, 100)) + "}";
		Serial.println("[iSYNC]<- " + payload);
		iSYNC.mqPub(iSYNC_KEY, payload); //Publish
	}
}