#include <BH1750.h>
#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ClosedCube_HDC1080.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_SGP30 sgp;     
ClosedCube_HDC1080 hdc1080;
BH1750 lightMeter;

int touchbuttonPin = D6;
bool screen_on = false;
bool button_state = digitalRead(touchbuttonPin);

int lastDisplay = 0;                                //OLED
int DISPLAY_TIME = 3000;          
int displayItem = -1;



void printSerialNumber() {
  Serial.print("Device Serial Number=");
  HDC1080_SerialNumber sernum = hdc1080.readSerialNumber();
  char format[12];
  sprintf(format, "%02X-%04X-%04X", sernum.serialFirst, sernum.serialMid, sernum.serialLast);
  Serial.println(format);
}

void testdrawstyles() {

   unsigned long ora = millis();                      //imposto il tempo dei valori sul display
     if (ora - lastDisplay > DISPLAY_TIME) {
    lastDisplay = ora;
    displayItem++;
    if (displayItem > 5)displayItem;
  }



  if (screen_on == false ) {                             //stampo i valori
    switch(displayItem){
      case 0:
    display.clearDisplay();
    display.setTextSize(2);                      // text size
    display.setTextColor(SSD1306_WHITE);         // text color
    display.setCursor(10, 20);                    // text position
    display.println("Benvenuto");                  // messaggio
    display.display();                          //va messo a fine di ogni scritta
    display.clearDisplay(); 
    break;
 
    case 1:
  display.setTextSize(1);                           //temperatura
  display.setCursor(30, 10);
  display.setTextColor(SSD1306_WHITE);
  display.println("Temperatura: " ) ;
  display.setCursor(20, 30);
  display.setTextSize(3);
  display.println(hdc1080.readTemperature(), 1);
  display.display();
  display.clearDisplay();
  break;
  
    case 2:
  display.setTextSize(1);                           //umidità
  display.setCursor(30, 10);
  display.setTextColor(SSD1306_WHITE);
  display.println("Umidita': " ) ;
  display.setCursor(20, 30);
  display.setTextSize(3);
  display.print(hdc1080.readHumidity(), 1);
  display.display();
  display.clearDisplay();
    break;

  case 3:
  display.clearDisplay();
  display.setTextSize(1);                         //luce ambientale
  display.setCursor(30, 10);
  display.setTextColor(SSD1306_WHITE);
  display.println("Luce: " ) ;
  display.setCursor(20, 30);
  display.setTextSize(3);
  display.println(lightMeter.readLightLevel(), 1);
  display.display();
  display.clearDisplay();
  break;

case 4:
  display.clearDisplay();
  display.setTextSize(1);                           //TVOC
  display.setCursor(30, 10);
  display.setTextColor(SSD1306_WHITE);
  display.print("VOC: ");
  display.setTextSize(3);
  display.setCursor(20, 30);
  display.print(sgp.TVOC); //display.println(" ppb\t");
  display.display();
    display.clearDisplay();
  break;
  
case 5:
  display.clearDisplay();
  display.setTextSize(1);                             //CO2
  display.setCursor(30, 10);
  display.print("CO2 ");
  display.setCursor(20, 30);
  display.setTextSize(3);
  display.print(sgp.eCO2); //display.println(" ppm");
  display.display();
  display.clearDisplay();
  displayItem=1;
  break; 
    }
  }
}

void setup_sensori() {
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);                           //setup display oled
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever}
    display.clearDisplay();
    display.display();
    testdrawstyles();
    }
    
  hdc1080.begin(0x40);                                                      
  Serial.print("Manufacturer ID=0x");                                   //setup hdc1080 temperatura e umidità            
  Serial.println(hdc1080.readManufacturerId(), HEX); 
  Serial.print("Device ID=0x");
  Serial.println(hdc1080.readDeviceId(), HEX); 
  printSerialNumber();

  Wire.begin();                 
  lightMeter.begin();                                                      //setup bh1750, luce ambientale
  while (!Serial) {
    delay(10);
  }  
    if (! sgp.begin()) {                                                   //setup sgp30, voc e CO2
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);
}
