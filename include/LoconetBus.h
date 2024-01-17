#pragma once

//##########################################################################
//#
//#		LoconetBus.h
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
//#	File Version:	1		Date: 05.01.2024
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

#include <inttypes.h>

#include "ln_opc.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	LOCONET_BUS_MAX_CONSUMERS		10


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//----------------------------------------------------------------------
//	a pointer to a bus consumer
//
typedef void *	loconet_bus_consumer;


//----------------------------------------------------------------------
//	a bus consumer function
//
typedef void (*loconet_bus_consumer_func)( loconet_bus_consumer pConsumer, LnMsg *pMsg );


//----------------------------------------------------------------------
//	the bus structure
//
typedef struct loconet_bus
{
	loconet_bus_consumer		consumerArray[ LOCONET_BUS_MAX_CONSUMERS ];
	loconet_bus_consumer_func	consumerFunctions[ LOCONET_BUS_MAX_CONSUMERS ];
	uint8_t						numConsumers;

} loconet_bus_t;


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

extern void loconet_bus_init( loconet_bus_t *pBus );

extern uint8_t loconet_bus_register_consumer( loconet_bus_t *pBus, loconet_bus_consumer pConsumer, loconet_bus_consumer_func pFunc );
extern uint8_t loconet_bus_unregister_consumer( loconet_bus_t *pBus, loconet_bus_consumer pConsumer, loconet_bus_consumer_func pFunc );

extern void loconet_bus_broadcast( loconet_bus_t *pBus, LnMsg *pMsg, loconet_bus_consumer_func pSender );
