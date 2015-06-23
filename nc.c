/*
*
*  neo.cortec 2014
*  Descripse the api for communicating with the NC2400
*
*
*
*/


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "nc.h"


#define API_CMD_TX_UNACKNOWLEDGED 0x02
#define API_CMD_TX_ACKNOWLEDGED  0x03

#define RCV_ACK_DATA_PACKAGE 0x50
#define RCV_NACK_DATA_PACKAGE 0x51
#define RCV_DATA_PACKAGE 0x52
#define RCV_DATA_PACKAGE_HAPA 0x53



static ncFuntionReceivePackage_t gNcFuntionReceivePackage;
static ncFuntionTransmitPackageAckNak_t gNcFuntionTransmitPackageAckNak;
//static ncFunctionPutChar_t gNcFunctionPutChar;
//static ncFunctionGetChar_t gNcFunctionGetChar;
static ncFunctionPutData_t gNcFunctionPutData;


static ncEndian_t gNcEndian;
static char gNcTxBuffer[130];



//Local functions begin


// deserialisation  byte stream to a package stream
//
void byteStreamToPackageStream(unsigned char ncData)
{
	typedef enum {
		ncRcvWaitingForCmd,
		ncRcvWaitingForLength,
		ncRcvWaitingForNcIdLSB,
		ncRcvWaitingForNcIdMSB,
		ncRcvWaitingForPackageAgeLSB,
		ncRcvWaitingForPackageAgeMSB,
		ncRcvWaitingForHAPAMSB,
		ncRcvWaitingForHAPALSB,
		ncRcvWaitingForPort,
		ncRcvWaitingForData
	} ncRcvProtocolState_t;

	static ncRcvProtocolState_t ncRcvprotocolState = ncRcvWaitingForCmd;
	static unsigned char inputBuffer[250];
	static unsigned char payloadCounter;
	static ncId_t ncId = 0;
	// static char ncLSB = 0;
	static ncPackageAge_t ncPackageAge = 0;
	static ncPackageAge_t ncPackageAgeHAPA = 0;
	static char cmd = 0;
	static char length = 0;

	static ncPortId_t ncPortId = 0;
//        unsigned long temp_PA_shift;

       printf("%d\n:", ncData);

	switch (ncRcvprotocolState)
	{


	case ncRcvWaitingForCmd:
 printf("ncRcvWaitingForCmd");

		payloadCounter = 0;
		cmd = ncData;
		if ((cmd == RCV_ACK_DATA_PACKAGE)  ||
		    (cmd == RCV_NACK_DATA_PACKAGE) ||
			(cmd == RCV_DATA_PACKAGE)      ||
			(cmd == RCV_DATA_PACKAGE_HAPA))
		{
			ncRcvprotocolState = ncRcvWaitingForLength;
			break;
		}
		break;

	case ncRcvWaitingForLength:
printf("ncRcvWaitingForLength");
		length = ncData;
		ncRcvprotocolState = ncRcvWaitingForNcIdMSB;
		break;


	case ncRcvWaitingForNcIdMSB:

printf("ncRcvWaitingForNcIdMSB");
		ncId = ncData <<8;
		ncRcvprotocolState = ncRcvWaitingForNcIdLSB;
		break;

	case ncRcvWaitingForNcIdLSB:
 printf("ncRcvWaitingForNcIdLSB");
		ncId += ncData;
        if ((cmd == RCV_ACK_DATA_PACKAGE) || (cmd == RCV_NACK_DATA_PACKAGE))
		{
  			gNcFuntionTransmitPackageAckNak(ncId, cmd == RCV_ACK_DATA_PACKAGE ? 1 : 0);
			ncRcvprotocolState = ncRcvWaitingForCmd;
		}
        else
        {
		    ncRcvprotocolState = ncRcvWaitingForPackageAgeMSB;
        }
		break;



	case ncRcvWaitingForPackageAgeMSB:
		ncPackageAge = ncData;
                //printf("PA1:");
                //printf(ncPackageAge,HEX);
		ncRcvprotocolState = ncRcvWaitingForPackageAgeLSB;
		break;

	case ncRcvWaitingForPackageAgeLSB:
		ncPackageAge= ncData + (ncPackageAge<<8) ;
               // printf("PA:");
               // printf("%l",ncPackageAge);
                //printf("PA2:");
                //printf(ncPackageAge,HEX);
        if (cmd==RCV_DATA_PACKAGE_HAPA)
        {
        	ncRcvprotocolState=ncRcvWaitingForHAPAMSB;
        }
        else
        {
        	ncRcvprotocolState = ncRcvWaitingForPort;
        }
		break;

	case ncRcvWaitingForHAPAMSB:
                ncPackageAgeHAPA= ncData;
                //printf("PA3:");
                //printf(ncPackageAge,HEX);
                ncRcvprotocolState = ncRcvWaitingForHAPALSB;
                break;

        case ncRcvWaitingForHAPALSB:
                ncPackageAgeHAPA= ncData + (ncPackageAgeHAPA<<8) ;
                ncRcvprotocolState = ncRcvWaitingForPort;
 //               printf("PA:");
 //               printf(ncPackageAge,HEX);
 //               printf("PAHAPA:");
 //               printf(ncPackageAgeHAPA,HEX);
                break;

	case ncRcvWaitingForPort:
 printf("ncRcvWaitingForPort");
		ncPortId = ncData;
		ncRcvprotocolState = ncRcvWaitingForData;
		break;

	case ncRcvWaitingForData:
 printf("ncRcvWaitingForData");
		inputBuffer[payloadCounter++] = ncData;
		if (payloadCounter >= length - 7)
		{
 printf("gNcFuntionReceivePackage");
                   unsigned long HAPA = ((unsigned long) ((unsigned int)ncPackageAge) <<16) + (unsigned long) ((unsigned int)ncPackageAgeHAPA);

			gNcFuntionReceivePackage(ncId, HAPA, inputBuffer, payloadCounter);
			ncRcvprotocolState = ncRcvWaitingForCmd;
		}
		break;

	}
}














//implementation of ncApi functions





void ncInitialize(
	ncFuntionReceivePackage_t ncReceivePackage,
	ncFuntionTransmitPackageAckNak_t ncFuntionTransmitPackageAckNak,
        ncFunctionPutData_t ncFunctionPutData,
        ncEndian_t ncEndian)
{
  gNcFuntionReceivePackage = ncReceivePackage;
  gNcFuntionTransmitPackageAckNak = ncFuntionTransmitPackageAckNak;
  gNcFunctionPutData = ncFunctionPutData;
  gNcEndian = ncEndian;
}


void ncSendPackage(ncId_t ncDestId,
	ncPortId_t ncPortId,
	ncPackageDataPtr_t ncPackageDataPtr, 
	ncPackageLength_t ncPackageLength,
        char boAckPackageReceived
			     )   // functions pointer to responce
{
	gNcTxBuffer[0] = boAckPackageReceived ? API_CMD_TX_UNACKNOWLEDGED : API_CMD_TX_ACKNOWLEDGED;;
	gNcTxBuffer[1] = (char)(0xff & (ncPackageLength+3));
	gNcTxBuffer[2] = ncDestId>>8;
	gNcTxBuffer[3] = ncDestId;
        gNcTxBuffer[4] = ncPortId;
	memcpy(&gNcTxBuffer[2+3],ncPackageDataPtr,ncPackageLength);
        gNcFunctionPutData((ncPackageDataPtr_t)gNcTxBuffer, ncPackageLength + 5);

}


