#include "iSYNC.h"
iSYNC::iSYNC(Client& netclient)
{
    clientSecure = &netclient;
    debug = false;
    httpRuning = false;
}
void iSYNC::debugMode(bool _debug = true)
{
    debug = _debug;
}
void iSYNC::begin(String ssid, String password)
{
#if defined(ESP8266) || defined(ESP32)
    if (debug)
        Serial.print("Connect to SSID: ");
    if (debug)
        Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        if (debug)
            Serial.print(".");
        // wait 1 second for re-trying
        delay(100);
    }
    if (debug)
        Serial.print("Connected to ");
    if (debug)
        Serial.println(ssid);
#endif
}

String iSYNC::HTTP_GET_RAW(String key, String auth)
{
    String inMSG = "";
    if (httpRuning)
    {
        if (debug)
            Serial.println("\nHTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
    if (debug)
        Serial.print("\nStarting connection to server...");

#if defined(K210)
    if (!clientSecure->connectSSL(SERVER_API, 443))
#else
    if (!clientSecure->connect(SERVER_API, 443))
#endif
    // if (!clientSecure->connect(SERVER_API, 443))
    {
        if (debug)
            Serial.println("Connection failed!");
    }else{
        if (debug)
            Serial.println("Connected to server!");
        String _host = "Host: ";
        _host += SERVER_API;
        // Make a HTTP request:
        clientSecure->println("GET https://api.isync.pro/data/" + key + "?auth=" + auth + " HTTP/1.0");
        clientSecure->println(_host);
        clientSecure->println("Connection: close");
        clientSecure->println();

        while (clientSecure->connected())
        {
            String line = clientSecure->readStringUntil('\n');
            if (line == "\r")
                break;
        }

#if defined(K210)
        // wait response
        long _startMillis = millis();
        while (!clientSecure->available() and (millis() - _startMillis < 2000));
#endif

        // if there are incoming bytes available
        while (clientSecure->available())
        {
            char c = clientSecure->read();
            inMSG += c;
        }
        if (debug)
        {
            Serial.println("Received Data ::.");
            Serial.println(inMSG);
        }
        clientSecure->stop();
    }
    httpRuning = false;
    return inMSG;
}

String iSYNC::HTTP_POST(String key, String auth, String msg)
{
    String inMSG = "";
    if (httpRuning)
    {
        if (debug)
            Serial.println("\nHTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
    if (debug)
        Serial.print("\nStarting connection to server...");

#if defined(K210)
    if (!clientSecure->connectSSL(SERVER_API, 443))
#else
    if (!clientSecure->connect(SERVER_API, 443))
#endif
    // if (!clientSecure->connect(SERVER_API, 443))
    {
        if (debug)
            Serial.println("Connection failed!");
    }
    else
    {
        if (debug)
            Serial.println("Connected to server!");
        String _host = "Host: ";
        _host += SERVER_API;
        // Make a HTTP request:
        clientSecure->println("GET https://api.isync.pro/data/" + key + "/set/" + msg + "?auth=" + auth + " HTTP/1.0");
        clientSecure->println(_host);
        clientSecure->println("Connection: close");
        clientSecure->println();

        while (clientSecure->connected())
        {
            String line = clientSecure->readStringUntil('\n');
            if (line == "\r")
                break;
        }
        
#if defined(K210)
        // wait response
        long _startMillis = millis();
        while (!clientSecure->available() and (millis() - _startMillis < 2000));
#endif

        // if there are incoming bytes available
        while (clientSecure->available())
        {
            char c = clientSecure->read();
            inMSG += c;
        }
        if (debug)
        {
            Serial.println("Send Data ::.");
            Serial.println(inMSG);
        }
        clientSecure->stop();
    }
    httpRuning = false;
    return inMSG;
}

String iSYNC::HTTP_GET(String key, String auth)
{
    StaticJsonDocument<200> obj;
    String json = HTTP_GET_RAW(key, auth);
    DeserializationError error = deserializeJson(obj, json);
    return obj[0];
}

String iSYNC::getPayload(String json)
{
    StaticJsonDocument<200> obj;
    DeserializationError error = deserializeJson(obj, json);
    if (debug && error)
        Serial.println("Error deserializeJson");
    String payload = obj["payload"];
    return payload;
}
bool iSYNC::mqConnected(){
    return MQTT->connected();
}
bool iSYNC::mqConnect()
{
    if (!mqConnected())
    {
        if(debug)Serial.print("MQTT connection...");
        String clientId = "";
        clientId = "ArduinoClient-" + username + "-" + String(random(0xffff), HEX);
        
        if (MQTT->connect(clientId.c_str(), username.c_str(), auth.c_str()))
        {
            return true;
        }else{
            return false;
        }
    }
}
void iSYNC::mqLoop()
{
    MQTT->loop();
}
void iSYNC::mqInit(String _username, String _auth){
    mqInit(_username,_auth,false);
}
void iSYNC::mqInit(String _username, String _auth,bool secure)
{
    if (debug)Serial.print("MQTT Init...");
    username = _username;
    auth = _auth;
    MQTT = new PubSubClient(*clientSecure);
    if(secure)MQTT->setServer(SERVER_MQTT, 8883);
    else MQTT->setServer(SERVER_MQTT, 1883);
    if (debug)Serial.println("Success");
}
iSYNC &iSYNC::mqCallback(MQTT_CALLBACK_SIGNATURE)
{
    MQTT->setCallback(callback);
    String clientId = "";
    clientId = "ArduinoClient-" + username + "-" + String(random(0xffff), HEX);
    MQTT->connect(clientId.c_str(), username.c_str(), auth.c_str());
}
bool iSYNC::mqPub(String key, String msg)
{
    if(debug)Serial.println("MQTT Publish -> "+msg);
    String _topic = "iSYNC/" + auth + "/" + key;
    return MQTT->publish(_topic.c_str(), msg.c_str());
}
bool iSYNC::mqSub(String key)
{
    if(debug)Serial.println("MQTT Subscribe key."+key);
    String _topic = "iSYNC/" + auth + "/" + key;
    return MQTT->subscribe(_topic.c_str());
}
bool iSYNC::mqSubProject()
{
    if(debug)Serial.println("MQTT Subscribe project.");
    String _topic = "iSYNC/" + auth + "/#";
    return MQTT->subscribe(_topic.c_str());
}