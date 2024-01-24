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
	uint8_t	length	= LOCONET_PACKET_SIZE( pMsg->sz.command, pMsg->sz.mesg_size );

	printf( "  raw: [" );

	for( uint8_t idx = 0 ; idx < length ; idx++ )
	{
		printf( " %02X ", pMsg->data[ idx ] );
	}

	printf( "]\n" );
}


//**************************************************************************
//	printSwitchRequest
//--------------------------------------------------------------------------
//	description
//
void printSwitchRequest( uint16_t address, bool output_closed, bool direction_thrown )
{
	printf( "CMD: Switch Request" );
}


//**************************************************************************
//	printSwitchReport
//--------------------------------------------------------------------------
//	description
//
void printSwitchReport( uint16_t address, bool output_closed, bool direction_thrown )
{
	printf( "CMD: Switch Report " );
}


//**************************************************************************
//	printOutputReport
//--------------------------------------------------------------------------
//	description
//
void printOutputReport( uint16_t address, bool output_closed, bool direction_thrown )
{
	printf( "CMD: Output Report " );
}


//**************************************************************************
//	printSwitchState
//--------------------------------------------------------------------------
//	description
//
void printSwitchState( uint16_t address, bool output_closed, bool direction_thrown )
{
	printf( "CMD: Switch State  " );
}


//**************************************************************************
//	printSensorReport
//--------------------------------------------------------------------------
//	description
//
void printSensorReport( uint16_t address, bool state )
{
	printf( "CMD: Sensor Report " );
}


//**************************************************************************
//	printLoconetMsg
//--------------------------------------------------------------------------
//	description
//
void printLoconetMsg( loconet_bus_consumer pConsumer, LnMsg *pMsg )
{
	switch( pMsg->sz.command )
	{
		case OPC_SW_REP:
		case OPC_SW_REQ:
		case OPC_SW_STATE:
		case OPC_INPUT_REP:
			break;

		default:
			printf( "CMD: 0x%02X unknown  ", pMsg->sz.command );
			break;
	}

	printRawData( pMsg );
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
	theUart.rxPin		= 3;
	theUart.txPin		= 1;
	theUart.invertRx	= true;
	theUart.invertTx	= true;

	loconet_bus_init( &theBus );
	loconet_phy_uart_init( &theUart );
	loconet_consumer_switch_sensor_init( &theSwitchSensorHandler, &theBus );

	//------------------------------------------------------------------
	//	register functions
	//
	loconet_consumer_register_notify_switch_request( &theSwitchSensorHandler, printSwitchRequest );
	loconet_consumer_register_notify_switch_report( &theSwitchSensorHandler, printSwitchReport );
	loconet_consumer_register_notify_switch_state( &theSwitchSensorHandler, printSwitchState );
	loconet_consumer_register_notify_switch_outputs( &theSwitchSensorHandler, printOutputReport );
	loconet_consumer_register_notify_sensor( &theSwitchSensorHandler, printSensorReport );

	loconet_bus_register_consumer( &theBus, NULL, printLoconetMsg );

	vTaskDelay( 5000 / portTICK_PERIOD_MS );

	printf( "Loconet Monitor\n" );

	while( 1 )
	{
		loconet_phy_uart_process( &theUart );

		vTaskDelay( 10 / portTICK_PERIOD_MS );
	}
}
