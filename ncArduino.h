/*
 * ncArduino.h
 *
 *  Created on: 03/06/2015
 *      Author: NHO
 */

#ifndef NCARDUINO_H_
#define NCARDUINO_H_

#include <arduino.h>
#include <stdint.h>
#include "ncApi.h"

//this function sends data to the nc chip
void ncArduinoPutData(
	ncPackageDataPtr_t ncPackageDataPtr,
	ncPackageLength_t ncPackageLength); 


//this function poll the nc chip for data
void ncArduinoPollGetData();


//this function initialize the Arduino interface to the low layer
int ncArduinoInitialize( ncFuntionChipReadyToTransmit_t ncFuntionChipReadyToTransmit);



#endif /* NCARDUINO_H_ */
