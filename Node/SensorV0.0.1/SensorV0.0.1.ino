#include <I2CSoilMoistureSensor.h>
#include <RFM69.h>         //get it here: https://github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://github.com/lowpowerlab/rfm69
#include <RFM69_OTA.h>     //get it here: https://github.com/lowpowerlab/rfm69
#include <SPIFlash.h>      //get it here: https://github.com/lowpowerlab/spiflash
#include <SPI.h>
#include <Wire.h>
#include <LowPower.h>      //get it here: https://github.com/lowpowerlab/lowpower
#include "config.h"        //Node config

SPIFlash flash(8, 0xEF30); //WINDBOND 4MBIT flash chip on CS pin D8 (default for Moteino)
I2CSoilMoistureSensor sensor; //The fucking sensor
int soilMoistThresh = 250; //Moisture threshold obviously
String senseDATA; // sensor data STRING
String ErrorLvl = "0"; // Error level. 0 = normal. 1 = soil moisture, 2 = Temperature , 3 = Humidity, 4 = Battery voltage

char Mstr[10]; //Strings for storing tranmission data
char Tstr[10];
char Lstr[10];
char buffer[50]; //Buffer for storing local strings

void setup() {
  Wire.begin();
  Serial.begin(115200); //115200 BAUD can change this to SERIAL_BAUD if I want all in config.h
  Serial.flush(); //Unsure if needed, but fuck it
  Serial.println("\n"); //Buffer line to keep random initial serial monitor char's away
  
  //Initialize radio and Automatic Transmission Control
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.enableAutoPower(ATC_RSSI); //Transmission control to save power
  radio.encrypt("Dreamplace2.0!!!"); //PASSWORD

  //Info print
  sprintf(buffer, "Soil Node 1 transmitting at: %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buffer); //Print stored buffer
  sensor.begin(); // reset sensor
  delay(1000); // give some time to boot up

  //Sensor info
  Serial.println("Initializing...");
  Serial.print("I2C Soil Moisture Sensor Address: ");
  Serial.println(sensor.getAddress(),HEX);
  Serial.print("Sensor Firmware version: ");
  Serial.println(sensor.getVersion(),HEX);
  Serial.println();

  radio.sendWithRetry(GATEWAYID, "START", 6); // Test packet
  Blink(LED, 100);Blink(LED, 100);Blink(LED, 100); //BLINK
  
}

byte sendLoops = 5; //Sleep timer
double M,T,L; //Storing vars
byte sendLen;

void loop() {
  Serial.print(sendLoops * 8); //TESTING
  Serial.println(" seconds until reading."); //TESTING
  if (sendLoops-- <= 1)  //send readings every SEND_LOOPS
  {
    sendLoops = SEND_LOOPS - 1;
    while (sensor.isBusy()) delay(50);
      M = sensor.getCapacitance();
      Serial.print("Soil Moisture Capacitance: ");
      Serial.println(M); //read capacitance register
      delay(50);
      T = (((sensor.getTemperature()/(float)10)*1.8) + 32);
      Serial.print("Temperature: ");
      Serial.println(T); //temperature register
      delay(50); 
      L = sensor.getLight(true);
      Serial.print("Light: ");
      Serial.println(L); //request light measurement, wait and read light register
      sensor.sleep();
      delay(10); //TESTING
      Serial.println("Measuring successful!  Starting wake countdown..."); //TESTING
      Serial.println(); //TESTING
    //data to strings
    dtostrf(M, 3,2, Mstr);
    dtostrf(T, 3,2, Tstr);
    dtostrf(L, 3,2, Lstr);
    sprintf(buffer, "M:%s T:%s L:%s", Mstr, Tstr, Lstr);

    sendLen = strlen(buffer);
    radio.sendWithRetry(GATEWAYID, buffer, sendLen, 1); //retry one time
    //Confirm readings
    Serial.print(buffer); Serial.print(" (packet length:"); Serial.print(sendLen); Serial.println(")");
    Blink(LED, 5);
  }
  SERIALFLUSH();
  flash.sleep(); //I don't initialize, here for when I do
  radio.sleep(); //Comment out if I want to program wirelessly
  delay(10); //Fixed random characters printing in console
  LowPower.powerDown(sleepTime, ADC_OFF, BOD_OFF);
}
