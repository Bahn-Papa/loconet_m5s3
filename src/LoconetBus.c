//##########################################################################
//#
//#		LoconetBus.c
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

#include "LoconetBus.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		F U N C T I O N S
//
//==========================================================================

void loconet_bus_init( loconet_bus_t *pBus )
{
	pBus->numConsumers	= 0;

	for( uint8_t idx = 0 ; LOCONET_BUS_MAX_CONSUMERS > idx ; idx++ )
	{
		pBus->consumerArray[ idx ]		= NULL;
		pBus->consumerFunctions[ idx ]	= NULL;
	}
}


uint8_t loconet_bus_register_consumer( loconet_bus_t *pBus, loconet_bus_consumer pConsumer, loconet_bus_consumer_func pFunc )
{
	uint8_t	error = 1;

	if( LOCONET_BUS_MAX_CONSUMERS > pBus->numConsumers )
	{
		pBus->consumerArray[ pBus->numConsumers ]		= pConsumer;
		pBus->consumerFunctions[ pBus->numConsumers ]	= pFunc;
		pBus->numConsumers++;

		error = 0;
	}

	return( error );
}


uint8_t loconet_bus_unregister_consumer( loconet_bus_t *pBus, loconet_bus_consumer pConsumer, loconet_bus_consumer_func pFunc )
{
	uint8_t error		= 1;	//	consumer not found
	uint8_t foundIdx	= LOCONET_BUS_MAX_CONSUMERS;
	uint8_t	idx;


	//-----------------------------------------------------------------
	//	first check if there are consumers in the array
	//
	if( 0 == pBus->numConsumers )
	{
		error =  2;		//	no consumers in array
	}
	else
	{
		//-----------------------------------------------------------------
		//	then search for the 'consumer'
		//
		for( idx = 0 ; (idx < pBus->numConsumers) && (LOCONET_BUS_MAX_CONSUMERS == foundIdx) ; idx++ )
		{
			if( pBus->consumerFunctions[ idx ] == pFunc )
			{
				foundIdx = idx;
			}
		}

		//-----------------------------------------------------------------
		//	if foundIdx < LOCONET_BUS_MAX_CONSUMERS, then we found
		//	the consumer, so remove it from the consumers array
		//
		if( LOCONET_BUS_MAX_CONSUMERS > foundIdx )
		{
			error = 0;

			for( idx = foundIdx + 1 ; idx < pBus->numConsumers ; idx++, foundIdx++ )
			{
				pBus->consumerArray[ foundIdx ]		= pBus->consumerArray[ idx ];
				pBus->consumerFunctions[ foundIdx ]	= pBus->consumerFunctions[ idx ];
			}

			pBus->numConsumers--;
			pBus->consumerArray[ pBus->numConsumers ]		= NULL;
			pBus->consumerFunctions[ pBus->numConsumers ]	= NULL;
		}
	}

	return( error );
}


void loconet_bus_broadcast( loconet_bus_t *pBus, LnMsg *pMsg, loconet_bus_consumer_func pSender )
{
	loconet_bus_consumer_func	pFunc;

	for( uint8_t idx = 0 ; idx < pBus->numConsumers ; idx++ )
	{
		pFunc = pBus->consumerFunctions[ idx ];

		if( pSender != pFunc )
		{
			(*pFunc)( pBus->consumerArray[ idx ], pMsg );
		}
	}
}
