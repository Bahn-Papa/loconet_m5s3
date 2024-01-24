//##########################################################################
//#
//#		LoconetConsumerSwitchSensor.c
//#
//#-------------------------------------------------------------------------
//#
//#	The functions in this part of the library are used react on switch
//#	and sensor messages.
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
#include "LoconetConsumerSwitchSensor.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#ifndef NULL
	#define NULL	((void *)0)
#endif


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

void loconet_consumer_switch_sensor_init( loconet_consumer_switch_sensor_t *pConsumer, loconet_bus_t *pBus )
{
	pConsumer->pBus					= pBus;
	pConsumer->pNotifySensor		= NULL;
	pConsumer->pNotifySwitchRequest	= NULL;
	pConsumer->pNotifySwitchReport	= NULL;
	pConsumer->pNotifySwitchOutputs	= NULL;
	pConsumer->pNotifySwitchState	= NULL;

	loconet_bus_register_consumer( pBus, pConsumer, loconet_consumer_switch_sensor_process );
}


uint8_t	loconet_consumer_register_notify_sensor( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_sensor pFunc )
{
	if( NULL == pConsumer->pNotifySensor )
	{
		pConsumer->pNotifySensor = pFunc;

		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


extern uint8_t	loconet_consumer_register_notify_switch_request( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc )
{
	if( NULL == pConsumer->pNotifySwitchRequest )
	{
		pConsumer->pNotifySwitchRequest = pFunc;

		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


extern uint8_t	loconet_consumer_register_notify_switch_report(  loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc )
{
	if( NULL == pConsumer->pNotifySwitchReport )
	{
		pConsumer->pNotifySwitchReport = pFunc;

		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


extern uint8_t	loconet_consumer_register_notify_switch_outputs( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc )
{
	if( NULL == pConsumer->pNotifySwitchOutputs )
	{
		pConsumer->pNotifySwitchOutputs = pFunc;

		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


extern uint8_t	loconet_consumer_register_notify_switch_state(   loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc )
{
	if( NULL == pConsumer->pNotifySwitchState )
	{
		pConsumer->pNotifySwitchState = pFunc;

		return( 0 );
	}
	else
	{
		return( 1 );
	}
}


void loconet_consumer_switch_sensor_process( loconet_bus_consumer pConsumer, LnMsg *pMsg )
{
	loconet_consumer_switch_sensor_t	*myConsumer	=	(loconet_consumer_switch_sensor_t *)pConsumer;
	uint16_t	Address;
	uint8_t		Direction;
	uint8_t		Output;


	Address = (pMsg->srq.sw1 | ((pMsg->srq.sw2 & 0x0F) << 7));

	if( pMsg->sr.command != OPC_INPUT_REP )
	{
		Address++;
	}

	switch( pMsg->sr.command )
	{
		case OPC_INPUT_REP:
			if( myConsumer->pNotifySensor )
			{
				Address <<= 1;
				Address += (pMsg->ir.in2 & OPC_INPUT_REP_SW) ? 2 : 1;
				Output	= pMsg->ir.in2 & OPC_INPUT_REP_HI;

				(*myConsumer->pNotifySensor)( Address, Output );
			}
			break;

		case OPC_SW_REQ:
			if( myConsumer->pNotifySwitchRequest )
			{
				Direction	= pMsg->srq.sw2 & OPC_SW_REQ_DIR;
				Output		= pMsg->srq.sw2 & OPC_SW_REQ_OUT;

				(*myConsumer->pNotifySwitchRequest)( Address, Output, Direction );
			}
			break;

		case OPC_SW_REP:
			if( pMsg->srp.sn2 & OPC_SW_REP_INPUTS )
			{
				if( myConsumer->pNotifySwitchReport )
				{
					Direction	= pMsg->srp.sn2 & OPC_SW_REP_SW;
					Output		= pMsg->srp.sn2 & OPC_SW_REP_HI;

					(*myConsumer->pNotifySwitchReport)( Address, Output, Direction );
				}
			}
			else
			{
				if( myConsumer->pNotifySwitchOutputs )
				{
					Direction	= pMsg->srp.sn2 & OPC_SW_REP_THROWN;
					Output		= pMsg->srp.sn2 & OPC_SW_REP_CLOSED;

					(*myConsumer->pNotifySwitchOutputs)( Address, Output, Direction );
				}
			}
			break;

		case OPC_SW_STATE:
			if( myConsumer->pNotifySwitchState )
			{
				Direction	= pMsg->srq.sw2 & OPC_SW_REQ_DIR;
				Output		= pMsg->srq.sw2 & OPC_SW_REQ_OUT;

				(*myConsumer->pNotifySwitchState)( Address, Output, Direction);
			}
			break;
	}
}
