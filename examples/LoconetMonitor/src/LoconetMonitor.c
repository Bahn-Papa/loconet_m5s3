//##########################################################################
//#
//#		LoconetMonitor.c
//#
//#-------------------------------------------------------------------------
//#
//#	The functions in this library are used to spread loconet messages.
//#
//#-------------------------------------------------------------------------
//#
//#		MIT License
//#
//#		Copyright (c) 2023	Michael Pfeil
//#							Am Kuckhof 8
//#							D - 52146 Würselen
//#							GERMANY
//#
//#-------------------------------------------------------------------------
//#
//#	File Version:	1		Date: 14.01.2024
//#
//#	Implementation:
//#		-	First implementation of the functions
//#
//##########################################################################


//==========================================================================
//
//		I N C L U D E S
//
//==========================================================================

#include "LoconetBus.h"
#include "LoconetPhyUART.h"
#include "LoconetConsumerSwitchSensor.h"


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

loconet_bus_t						theBus;
loconet_phy_uart_t					theUart;
loconet_consumer_switch_sensor_t	theSwitchSensorHandler;


//==========================================================================
//
//		F U N C T I O N S
//
//==========================================================================

//**************************************************************************
//	printRawData
//--------------------------------------------------------------------------
//	description
//
void printRawData( LnMsg *pMsg )
{
	uint8_t	length	= LOCONET_PACKET_SIZE( pMsg->data[ 0 ], pMsg->data[ 1 ] );

	printf( "  raw: [" );

	for( uint8_t idx = 0 ; idx < length ; idx++ )
	{
		printf( " %02X ", pMsg->data[ idx ] );
	}

	printf( "]\n" );
}

//**************************************************************************
//	app_main
//--------------------------------------------------------------------------
//	description
//
void app_main()
{
	//------------------------------------------------------------------
	//	initialize structures
	//
	theUart.pBus		= &theBus;
	theUart.uartNum		= 2;
	theUart.rxPin		= 1;
	theUart.txPin		= 3;
	theUart.invertRx	= false;
	theUart.invertTx	= false;

	loconet_bus_init( &theBus );
	loconet_phy_uart_init( &theUart );
	loconet_consumer_switch_sensor_init( &theSwitchSensorHandler, &theBus );

	//------------------------------------------------------------------
	//	register functions
	//
}
