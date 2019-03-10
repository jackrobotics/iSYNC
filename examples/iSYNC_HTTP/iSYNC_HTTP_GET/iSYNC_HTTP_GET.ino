#include <iSYNC.h>
iSYNC iSYNC;

String ssid = "HONEYLab";
String pass = "@HONEYLab";

String iSYNC_KEY = "5c7ecb8e89a6d26444d19991";

void setup() {
    Serial.begin(115200);
    Serial.println(iSYNC.getVersion());

    iSYNC.begin(ssid,pass);
}
long last = 0;
void loop() {
    if(millis() - last > 1000){
        last = millis();
        String json = iSYNC.HTTP_GET(iSYNC_KEY);
        String payload = iSYNC.getPayload(json);
        Serial.print("[iSYNC] Payload => ");
        Serial.println(payload);
    }
}