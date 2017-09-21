#include <I2CSoilMoistureSensor.h>
#include <RFM69.h>         //get it here: https://github.com/lowpowerlab/rfm69
#include <RFM69_ATC.h>     //get it here: https://github.com/lowpowerlab/rfm69
#include <RFM69_OTA.h>     //get it here: https://github.com/lowpowerlab/rfm69
#include <SPIFlash.h>      //get it here: https://github.com/lowpowerlab/spiflash
#include <SPI.h>           //included in Arduino IDE (www.arduino.cc)
#include <Wire.h>          //included in Arduino IDE (www.arduino.cc)
#include <LowPower.h>      //get it here: https://github.com/lowpowerlab/lowpower
#include "config.h"

SPIFlash flash(8, 0xEF30); //WINDBOND 4MBIT flash chip on CS pin D8 (default for Moteino)
I2CSoilMoistureSensor sensor;
int soilMoistThresh = 250;
String senseDATA; // sensor data STRING
String ErrorLvl = "0"; // Error level. 0 = normal. 1 = soil moisture, 2 = Temperature , 3 = Humidity, 4 = Battery voltage
//*********************************************************************************************

void setup() {
  Wire.begin();
  Serial.begin(115200);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.enableAutoPower(ATC_RSSI);
  radio.encrypt(ENCRYPTKEY);
  sensor.begin(); // reset sensor
  delay(1000); // give some time to boot up
  SERIALFLUSH();
  Serial.println("Initializing...");
  Serial.print("I2C Soil Moisture Sensor Address: ");
  Serial.println(sensor.getAddress(),HEX);
  Serial.print("Sensor Firmware version: ");
  Serial.println(sensor.getVersion(),HEX);
  Serial.println();
}

byte sendLoops = 5;

void loop() {
  Serial.println(sendLoops);
  if (sendLoops-- <= 1)  //send readings every SEND_LOOPS
  {
    sendLoops = SEND_LOOPS - 1;
    while (sensor.isBusy()) delay(50); // available since FW 2.3
    Serial.print("Soil Moisture Capacitance: ");
    Serial.println(sensor.getCapacitance()); //read capacitance register
    delay(100);
    Serial.print("Temperature: ");
    Serial.println((((sensor.getTemperature()/(float)10)*1.8) + 32)); //temperature register
    delay(100);
    Serial.print("Light: ");
    Serial.println(sensor.getLight(true)); //request light measurement, wait and read light register
    sensor.sleep();
    Serial.println("Measuring successful!  Starting wake countdown...asdf");
  }
  SERIALFLUSH();
  flash.sleep();
  radio.sleep();
  LowPower.powerDown(sleepTime, ADC_OFF, BOD_OFF);
}

