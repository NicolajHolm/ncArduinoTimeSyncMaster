

#include <arduino.h>
#include <stdint.h>
#include <string.h>
#include <SoftwareSerial.h>
#include "ncArduino.h"
#include "ncApi.h"
#include "nc.h"


SoftwareSerial mySerial(5,3); // RX, TX

void ncArduinoPutData(
	ncPackageDataPtr_t ncPackageDataPtr, 
	ncPackageLength_t ncPackageLength) 
{
   mySerial.write(ncPackageDataPtr, ncPackageLength);
}


void ncArduinoPollGetData()
{
  int c;
  while (mySerial.available() > 0)
  {
    c = mySerial.read();
    byteStreamToPackageStream((unsigned char) c);
  }
}


void ncArduinoSerialEvent(){
  ncArduinoPollGetData();
}




int ncArduinoInitialize( ncFuntionChipReadyToTransmit_t ncFuntionChipReadyToTransmit)
{
   attachInterrupt(0, ncFuntionChipReadyToTransmit, FALLING);
   attachInterrupt(1, ncArduinoSerialEvent, FALLING);
   mySerial.begin(57600);

  //debug �window
  Serial.begin(115200);
  return 0;
}

