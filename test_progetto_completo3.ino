String myName = "IOTmisuro";

#include "_wifi.h"
#include "_mqtt.h"
#include "Sensori.h"

const int PIRpin = D5;
int lastPIRstate = 0;

void setup() {
  Serial.begin(115200);

  pinMode(touchbuttonPin, INPUT);                            //touch button
  pinMode(PIRpin, INPUT);                                   //setup pir sensor (presenza persone)
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  setup_wifi();                                                 //richiamo il setup del WIFI
  setup_sensori();                                              //richiamo il setup dei sensori
  randomSeed(micros());                                          //richiamo il setup del MQTT
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttReceive);
}
void loop() {

  int p = digitalRead(PIRpin);                                  //PIR
  if (p != lastPIRstate) {                                      //se l'ultimo stato del PIR è diverso dallo stato attuale
    lastPIRstate = p;                                           //cambia il valore dell'ultimo stato
    if (p == 1)
      //digitalWrite(BUILTIN_LED, LOW);                            //led acceso
      Serial.println("Movimento rilevato");                     // se rileva movimento
    if (p == 0)
      // digitalWrite(BUILTIN_LED, HIGH);                         //led spento
      Serial.println("Nessun movimento");                       //se non lo rileva più

    String pl = String(p);                                                    //per mqtt, string payload = string p
 
    Serial.println("Publish message: " + mqttTopicPubish_event + pl);         //mqtt event del pir,
    mqttClient.publish(mqttTopicPubish_event.c_str(), pl.c_str());


  }

  Serial.print("T:");
  Serial.print(hdc1080.readTemperature());                                     //print temperatura
  Serial.println("C ");

  Serial.print("RH:");                                                         //print umidità
  Serial.print(hdc1080.readHumidity());
  Serial.println("%");

  float lux = lightMeter.readLightLevel();                                    //print luce ambientale
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  int counter = 0;                                                             //inizio parte TVOC Co2 H2 & Ethanol
  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  Serial.print("TVOC: "); Serial.print(sgp.TVOC); Serial.println(" ppb\t");         //print TVOC
  Serial.print("eCO2: "); Serial.print(sgp.eCO2); Serial.println(" ppm");           //print CO2

  if (! sgp.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }

  Serial.print("Raw H2: "); Serial.print(sgp.rawH2); Serial.println(" \t");         //print H2
  Serial.print("Raw Ethanol: "); Serial.print(sgp.rawEthanol); Serial.println("");  //print Ethanol

  delay(1000);
  counter++;
  if (counter == 30) {
    counter = 0;
    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  }

  button_state = digitalRead(touchbuttonPin);             //TOUCH BUTTON
  if (button_state == HIGH) {                            // se il pulsante è premuto
    if (screen_on == false) {                            //e lo schermo è acceso, lo spegne
      display.display();
      display.clearDisplay();
      screen_on = true;
    }
    else {                                              //se lo schermo è spento, lo accende
      display.display();
      screen_on = false;
    }
  }
  if (screen_on == false) {                            //se lo schermo è acceso
    testdrawstyles();                                 // richiamo il setup del display
  }

  if (!mqttClient.connected()) {                             //MQTT
    reconnect();
  }
  mqttClient.loop();

  unsigned long now = millis();
  if (now - lastMsg > SENSOR_TIME) {
    lastMsg = now;

    DynamicJsonDocument doc(1024);
    doc["temperatura"] = hdc1080.readTemperature();
    doc["umidità"] = hdc1080.readHumidity();
    doc["luce"] = lightMeter.readLightLevel();
    doc["TVoc"] = sgp.TVOC;
    doc["CO2"] = sgp.eCO2;
    doc["batteria"] = analogRead (A0) * 4.5 / 1024.0;    // valori inviati in Json su mqtt
 
    String mqttPayload = "";
    serializeJson(doc, mqttPayload);

    Serial.println("Publish message: " + mqttTopicPubish_sensor + " " + mqttPayload);
    mqttClient.publish(mqttTopicPubish_sensor.c_str(), mqttPayload.c_str());

  }

}
