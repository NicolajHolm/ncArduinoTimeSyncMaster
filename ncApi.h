#ifndef _NC_API_H
#define _NC_API_H
/*
ncApi.h -
neo.cortec 2015
Description of Api for communicating with the NC2400 chip

*/

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int16_t ncId_t;
typedef int16_t ncPackageAge_t;
typedef int8_t ncPortId_t;
typedef unsigned char* ncPackageDataPtr_t;
typedef int8_t ncPackageLength_t;
typedef char ncMessageType_t;


//This function is the interface to the HW abstraction layer. The task of this function is to send the package to the nc chip as a stream.
typedef void (*ncFunctionPutData_t) (ncPackageDataPtr_t ncPackageDataPtr, ncPackageLength_t ncPackageLength);

//This function notifies about an incomming package
typedef void(*ncFuntionReceivePackage_t)(ncId_t ncId, ncPackageAge_t ncPackageAge, ncPackageDataPtr_t ncPackagePtr, ncPackageLength_t Count);


//This function notifies if a package has been delivered with or without succes
typedef void(*ncFuntionTransmitPackageAckNak_t)(ncId_t ncId, char boAckPackageReceived);


//This function notifies when nc chip is ready to trannit a package
typedef void(*ncFuntionChipReadyToTransmit_t)(void);


//packing of bytes
typedef enum {
	ncLittleEndian,
	ncBigEndian
} ncEndian_t;


//This function initializes the nc layer.
void ncInitialize(
	ncFuntionReceivePackage_t ncReceivePackage,
	ncFuntionTransmitPackageAckNak_t ncFuntionTransmitPackageAckNak,
	ncFunctionPutData_t ncFunctionPutData,
	ncEndian_t ncEndian); 


//This function sends a message to a nc node
void ncSendPackage(ncId_t ncDestId,       // address on the destination node
	ncPortId_t ncPort,
	ncPackageDataPtr_t ncPackageDataPtr,    // payload to send 
	ncPackageLength_t ncPackageLength,       // length of payload
	char boAckPackageReceived		     );   // 

#ifdef __cplusplus
  }
#endif

#endif //_NC_API_H

