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
    if(millis() - last > 5000){
        last = millis();
        String Temperature=String(random(0,100));
        String Humidity=String(random(0,100));
        String payload="{\"Temperature\":"+Temperature+",\"Humidity\":"+Humidity+"}";
        
        iSYNC.HTTP_POST(iSYNC_KEY,payload);
        Serial.print("[iSYNC] HTTP_POST => ");
        Serial.println(payload);
    }
}