#include <RFM69.h>
#include <RFM69_ATC.h>
#include <LowPower.h>

//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE ************
//*********************************************************************************************
#define NODEID        2                   //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
#define NETWORKID     100                 //the same on all nodes that talk to each other (range up to 255)
#define GATEWAYID     1
#define FREQUENCY   RF69_433MHZ
#define ENCRYPTKEY    "Password!!" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW                     //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ENABLE_ATC                        //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI -75
#define SERIAL_EN                         //comment out if you don't want any serial output
#define SEND_LOOPS   8                    //send data this many sleep loops
#define SLEEP_LONGEST SLEEP_8S 
period_t sleepTime = SLEEP_LONGEST;       //period_t is an enum type defined in the LowPower library (LowPower.h)
#define SERIALFLUSH() {Serial.flush();}
#define SERIALFLUSH();
//*********************************************************************************************
#define LED           9 // Moteinos have LEDs on D9
#define FLASH_SS      8 // and FLASH SS on D8

#ifdef SERIAL_EN
  #define SERIAL_BAUD   115200
  #define DEBUG(input)   {Serial.print(input);}
  #define DEBUGln(input) {Serial.println(input);Serial.flush();}
#else
  #define DEBUG(input);
  #define DEBUGln(input);
#endif

#ifdef ENABLE_ATC //Automatic Transmission Control
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void Blink(byte PIN, byte DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS/2);
  digitalWrite(PIN,LOW);
  delay(DELAY_MS/2);  
}
