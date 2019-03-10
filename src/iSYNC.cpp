#include "iSYNC.h"
iSYNC::iSYNC(){
    debug = false;
    httpRuning = false;
}
void iSYNC::debugMode(bool _debug = true){
    debug = _debug;
}
void iSYNC::begin(String ssid,String password){
    if(debug)Serial.print("Connect to SSID: ");
    if(debug)Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        if(debug)Serial.print(".");
        // wait 1 second for re-trying
        delay(100);
    }
    if(debug)Serial.print("Connected to ");
    if(debug)Serial.println(ssid);
}

String iSYNC::HTTP_GET_RAW(String key){
    String inMSG = "";
    if(httpRuning){
        if(debug)Serial.println("\nHTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
#if defined(ESP8266)
    const char FINGERPRINT[] = "fc 72 ae 00 0d ba cd b0 c8 00 97 74 39 79 21 36 64 e9 79 9f";
    clientSecure.setFingerprint(FINGERPRINT);
#endif
    if(debug)Serial.print("\nStarting connection to server...");
    
    if (!clientSecure.connect(SERVER_API, 443)){
        if(debug)Serial.println("Connection failed!");
    }else {
        if(debug)Serial.println("Connected to server!");
        String _host = "Host: ";_host+=SERVER_API;
        // Make a HTTP request:
        clientSecure.println("GET https://www.isync.pro/data/"+key+" HTTP/1.0");
        clientSecure.println(_host);
        clientSecure.println("Connection: close");
        clientSecure.println();

        while (clientSecure.connected()) {
            String line = clientSecure.readStringUntil('\n');
            if (line == "\r") break;
        }

        // if there are incoming bytes available
        while (clientSecure.available()) {
            char c = clientSecure.read();
            inMSG += c;
        }
        if(debug){
            Serial.println("Received Data ::.");
            Serial.println(inMSG);
        }
        clientSecure.stop();
    }
    httpRuning = false;
    return inMSG;
}

String iSYNC::HTTP_POST(String key,String msg){
    String inMSG = "";
    if(httpRuning){
        if(debug)Serial.println("\nHTTP Runing... please wait!");
        return "";
    }
    httpRuning = true;
#if defined(ESP8266)
    const char fingerprint[] = "fc 72 ae 00 0d ba cd b0 c8 00 97 74 39 79 21 36 64 e9 79 9f";
    clientSecure.setFingerprint(fingerprint);
#endif
    if(debug)Serial.print("\nStarting connection to server...");
    
    if (!clientSecure.connect(SERVER_API, 443)){
        if(debug)Serial.println("Connection failed!");
    }else {
        if(debug)Serial.println("Connected to server!");
        String _host = "Host: ";_host+=SERVER_API;
        // Make a HTTP request:
        clientSecure.println("GET https://www.isync.pro/data/"+key+"/set/"+msg+" HTTP/1.0");
        clientSecure.println(_host);
        clientSecure.println("Connection: close");
        clientSecure.println();

        while (clientSecure.connected()) {
            String line = clientSecure.readStringUntil('\n');
            if (line == "\r") break;
        }

        // if there are incoming bytes available
        while (clientSecure.available()) {
            char c = clientSecure.read();
            inMSG += c;
        }
        if(debug){
            Serial.println("Send Data ::.");
            Serial.println(inMSG);
        }
        clientSecure.stop();
    }
    httpRuning = false;
    return inMSG;
}

String iSYNC::HTTP_GET(String key){
    StaticJsonDocument<200> obj;
    String json = HTTP_GET_RAW(key);
    DeserializationError error = deserializeJson(obj, json);
    return obj[0];
}

String iSYNC::getPayload(String json){
    StaticJsonDocument<200> obj;
    DeserializationError error = deserializeJson(obj, json);
    if(debug && error)Serial.println("Error deserializeJson");
    String payload = obj["payload"];
    return payload;
}