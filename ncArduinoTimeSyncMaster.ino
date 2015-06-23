 // include the library code:
#include <Arduino.h>
#include <stdint.h>
#include <SoftwareSerial.h>
#include "ncApi.h"
#include "ncArduino.h"



unsigned long previousMillis = 0;  
unsigned long interval = 10000;           // interval at which to blink (milliseconds)
volatile char Send = 1;
 
void ncArduinoReceivePackage(ncId_t ncId, ncPackageAge_t ncPackageAge, ncPackageDataPtr_t ncPackagePtr, ncPackageLength_t Count){
  
  ncPackageAge_t ncPackageAgeCount125mSek = ncPackageAge;
  unsigned long SlaveTime = *((unsigned long *) ncPackagePtr);
  SlaveTime = SlaveTime +  (ncPackageAgeCount125mSek * 125);
  unsigned long MasterTimeMillis = millis();
  signed long timeDif = MasterTimeMillis - SlaveTime;
  Serial.println("ncArduinoReceivePackage");
  Serial.print("MasterTim=");
  Serial.println(MasterTimeMillis, DEC);
  Serial.print("SlaveTime=");
  Serial.println(SlaveTime, DEC);
  Serial.print("timeDif=");
    Serial.println(timeDif, DEC);
     Serial.print("ncId=");
    Serial.println(ncId, DEC);
    Serial.print("ncPackageAge=");
    Serial.println(ncPackageAge, DEC);
    Serial.print("Count=");
    Serial.println(Count, DEC);
 }


void ncArduinoTransmitPackageAckNak(ncId_t ncId, char boPackageReceived)
{
Serial.println("ncArduinoReceivePackageAckNak");
     Serial.print("ncId=");
    Serial.println(ncId, DEC);
    Serial.print("boPackageReceived=");
    Serial.println(boPackageReceived, DEC);

  
}



void ncFuntionChipReadyToTransmit(void){
  Send = 1;
}  




void setup(){
  int rc;
  ncInitialize(ncArduinoReceivePackage,ncArduinoTransmitPackageAckNak, ncArduinoPutData , ncLittleEndian );
  if ((rc = ncArduinoInitialize(ncFuntionChipReadyToTransmit)) != 0)
    return;
  
 pinMode(13, OUTPUT);
 pinMode(3, OUTPUT);
 previousMillis = millis();
//  delay(10000);
// Serial.println("Master setup");
}


void loop()
{
  
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    Send = 0;
    while (!Send) {
      delay(1);
    }
    Serial.println("Master millis");
    unsigned long MasterTimeMillis = millis();
    Serial.println("Master before ncSendMsg");
    ncSendPackage(0x2a, 1, (ncPackageDataPtr_t)&MasterTimeMillis, sizeof(MasterTimeMillis),1); 
    Serial.println("Master ncSendMsg");
    Serial.print("MasterTim=");
    Serial.println(MasterTimeMillis, DEC);
    previousMillis = MasterTimeMillis;
  }
  ncArduinoPollGetData();
}



