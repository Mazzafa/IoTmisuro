#include <ESP8266WiFi.h>

const char* ssid = "Siam-ReteAule";
const char* password = "Khimeya2018";

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.print("\nConnecting to "+String(ssid));

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
}
