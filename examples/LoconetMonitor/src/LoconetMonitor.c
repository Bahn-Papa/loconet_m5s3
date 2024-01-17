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
//	app_main
//--------------------------------------------------------------------------
//	description
//
void app_main()
{
	//------------------------------------------------------------------
	//	initialize structures
	//
	loconet_bus_init( &theBus );
	loconet_phy_uart_init( &theUart, &theBus );
	loconet_consumer_switch_sensor_init( &theSwitchSensorHandler, &theBus );

	//------------------------------------------------------------------
	//	register functions
	//
}
