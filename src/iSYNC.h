#ifndef iSYNC_H
#define iSYNC_H

#include "Arduino.h"
#include <ArduinoJson.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <WiFiClientSecure.h>
#elif defined(ESP32)
    #include <WiFiClientSecure.h>
#endif

#define Version "iSYNC library Version 0.1"
#define SERVER_API "www.isync.pro"

class iSYNC
{
    public:
        iSYNC();
        void debugMode(bool);
        void begin(String,String);

        String HTTP_GET_RAW(String);
        String HTTP_POST(String,String);

        String HTTP_GET(String);

        String getPayload(String);

        String getVersion(){return Version;};

    private:
        bool debug;
        bool httpRuning;
        WiFiClientSecure clientSecure;
};

#endif