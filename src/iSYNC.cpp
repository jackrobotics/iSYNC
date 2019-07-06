#include "iSYNC.h"
iSYNC::iSYNC(Client& netclient)
{
    clientSecure = &netclient;
    debug = false;
    httpRuning = false;
}
void iSYNC::debugMode(bool _debug)
{
    debug = _debug;
}
void iSYNC::begin(String ssid, String password)
{
#if defined(ESP8266) || defined(ESP32)
    if (debug){
        Serial.print("[iSYNC DEBUG]: Connect to SSID: ");
        Serial.println(ssid);
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED)
    {
        if (debug)Serial.print(".");
        // wait 0.1 second for re-trying
        delay(100);
    }
    if (debug){
        Serial.println();
        Serial.print("[iSYNC DEBUG]: Connected to ");
        Serial.println(ssid);
    }
#endif
}

String iSYNC::HTTP_GET_RAW(String key, String auth)
{
    String inMSG = "";
    if (httpRuning)
    {
        if (debug)Serial.println("\n[iSYNC DEBUG]: HTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
    if (debug)Serial.print("\n[iSYNC DEBUG]: Starting connection to server...");

#if defined(K210)
    if (!clientSecure->connectSSL(SERVER_API, 443))
#else
    if (!clientSecure->connect(SERVER_API, 443))
#endif
    // if (!clientSecure->connect(SERVER_API, 443))
    {
        if (debug)Serial.println("[iSYNC DEBUG]: Connection failed!");
    }else{
        if (debug)Serial.println("[iSYNC DEBUG]: Connected to server!");
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
            Serial.println("[iSYNC DEBUG]: Received Data ::.");
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
        if (debug)Serial.println("\n[iSYNC DEBUG]: HTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
    if (debug)Serial.print("\n[iSYNC DEBUG]: Starting connection to server...");

#if defined(K210)
    if (!clientSecure->connectSSL(SERVER_API, 443))
#else
    if (!clientSecure->connect(SERVER_API, 443))
#endif
    // if (!clientSecure->connect(SERVER_API, 443))
    {
        if (debug)Serial.println("[iSYNC DEBUG]: Connection failed!");
    }
    else
    {
        if (debug)Serial.println("[iSYNC DEBUG]: Connected to server!");
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
            Serial.println("[iSYNC DEBUG]: Send Data ::.");
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
    deserializeJson(obj, json);
    return obj[0];
}

String iSYNC::getPayload(String json)
{
    StaticJsonDocument<200> obj;
    deserializeJson(obj, json);
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
        if(debug)Serial.print("[iSYNC DEBUG]: MQTT connection...");
        String clientId = "";
        clientId = "ArduinoClient-" + username + "-" + String(random(0xffff), HEX);
        
        if (MQTT->connect(clientId.c_str(), username.c_str(), auth.c_str()))
        {
            return true;
        }else{
            return false;
        }
    }else return true;
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
    if (debug)Serial.print("[iSYNC DEBUG]: MQTT Init...");
    username = _username;
    auth = _auth;
    MQTT = new PubSubClient(*clientSecure);
    if(secure)MQTT->setServer(SERVER_MQTT, 8883);
    else MQTT->setServer(SERVER_MQTT, 1883);
    if (debug)Serial.println("Success");
}

bool iSYNC::mqPub(String key, String msg)
{
    if(debug)Serial.println("\n[iSYNC DEBUG]: MQTT Publish -> "+msg);
    String _topic = "iSYNC/" + auth + "/" + key;
    return MQTT->publish(_topic.c_str(), msg.c_str());
}
bool iSYNC::mqSub(String key)
{
    if(debug)Serial.println("\n[iSYNC DEBUG]: MQTT Subscribe key: "+key);
    String _topic = "iSYNC/" + auth + "/" + key;
    return MQTT->subscribe(_topic.c_str());
}
bool iSYNC::mqSubProject()
{
    if(debug)Serial.println("\n[iSYNC DEBUG]: MQTT Subscribe project.");
    String _topic = "iSYNC/" + auth + "/#";
    return MQTT->subscribe(_topic.c_str());
}