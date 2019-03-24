#ifndef iSYNC_H
#define iSYNC_H

#include "Arduino.h"
#include "lib/ArduinoJson/ArduinoJson.h"
#include "lib/PubSubClient/PubSubClient.h"

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiClientSecure.h>
#elif defined(ESP32)
    #include <WiFi.h>
    #include <WiFiClientSecure.h>
#endif


#define Version "\niSYNC library Version 0.2"
#define SERVER_API "api.isync.pro"
#define SERVER_MQTT "mq.isync.pro"

#if defined(ESP8266) || defined(ESP32)
    #include <functional>
    #define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
    #define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

class iSYNC
{
    private:
        bool debug;
        bool httpRuning;
        // WiFiClientSecure clientSecure;
        MQTT_CALLBACK_SIGNATURE;
        String auth,username;
        Client *clientSecure;
    public:
        iSYNC(Client&);
        void debugMode(bool);
        void begin(String,String);  //setup wifi

        /**
         * HTTP API lib
         * */
        String HTTP_GET_RAW(String,String);
        String HTTP_POST(String,String,String);
        String HTTP_GET(String,String);
        String getPayload(String);

        
        PubSubClient *MQTT;
        void mqInit(String, String);
        void mqInit(String, String, bool);
        iSYNC& mqCallback(MQTT_CALLBACK_SIGNATURE);
        void mqLoop();
        bool mqPub(String,String);
        bool mqSub(String);
        bool mqSubProject();
        bool mqConnect();
        bool mqConnected();

        String getVersion(){return Version;};
};

#endif