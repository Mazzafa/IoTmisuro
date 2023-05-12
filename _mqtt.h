#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* mqtt_server = "172.16.206.91";
const char* mqtt_user = "mosquitto";
const char* mqtt_password = "dietpi";

String thingId = "DMO_" + String(WiFi.macAddress().c_str());

String mqttTopicSubscibe_sensor = "iort2023/" + myName + "/sensor";
String mqttTopicSubscibe_event = "iort2023/" + myName + "/event";

String mqttTopicPubish_sensor = "iort2023/" + myName + "/sensor";
String mqttTopicPubish_event = "iort2023/" + myName + "/event";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
unsigned long lastMsg = 0;

void mqttReceive(char* topic, byte* payload, unsigned int length) {
  payload[length] = 0;
  String Payload = String((char*)payload);
  String Topic = String(topic);

  Serial.println("Message arrived " + Topic + " " + Payload );
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      mqttClient.subscribe(mqttTopicSubscibe_sensor.c_str());
      mqttClient.subscribe(mqttTopicSubscibe_event.c_str());

    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
