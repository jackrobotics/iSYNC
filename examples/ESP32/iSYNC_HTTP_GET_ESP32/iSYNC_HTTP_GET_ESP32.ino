#include <WiFiClientSecure.h>
#include <iSYNC.h>

WiFiClientSecure clientSecure;
iSYNC iSYNC(clientSecure);

String ssid = "HONEYLab";
String pass = "@HONEYLab";

String iSYNC_KEY = "5c888df67f56637c67cac702";
String iSYNC_AUTH = "5c888c84d2d6ae7c58c2ba69"; //Auth project

void setup() {
    Serial.begin(115200);
    Serial.println(iSYNC.getVersion());

    iSYNC.begin(ssid,pass);
}
long last = 0;
void loop() {
    if(millis() - last > 1000){
        last = millis();
        String json = iSYNC.HTTP_GET(iSYNC_KEY,iSYNC_AUTH);
        String payload = iSYNC.getPayload(json);
        Serial.print("[iSYNC] Payload => ");
        Serial.println(payload);
    }
}