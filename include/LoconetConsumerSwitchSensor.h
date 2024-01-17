#pragma once

//##########################################################################
//#
//#		LoconetConsumerSwitchSensor.h
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
#include <stdbool.h>

#include "ln_opc.h"
#include "LoconetBus.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define NOTIFY_FUNC_IDX_SENSOR				0
#define NOTIFY_FUNC_IDX_SWITCH_REQUEST		1
#define NOTIFY_FUNC_IDX_SWITCH_REPORT		2
#define NOTIFY_FUNC_IDX_SWITCH_OUTPUTS		3
#define NOTIFY_FUNC_IDX_SWITCH_STATE		4


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================

//----------------------------------------------------------------------
//	notify function definitions
//	a function of one of these types must be registered at
//	the "consumer" to be able to handle the spezific message.
//
typedef void (*loconet_consumer_func_notify_sensor)( uint16_t address, bool state );
typedef void (*loconet_consumer_func_notify_switch)( uint16_t address, bool output_closed, bool direction_thrown );


//----------------------------------------------------------------------
//	the bus structure
//
typedef struct loconet_consumer_switch_sensor
{
	loconet_bus_t							*pBus;
	loconet_consumer_func_notify_sensor		pNotifySensor;
	loconet_consumer_func_notify_switch		pNotifySwitchRequest;
	loconet_consumer_func_notify_switch		pNotifySwitchReport;
	loconet_consumer_func_notify_switch		pNotifySwitchOutputs;
	loconet_consumer_func_notify_switch		pNotifySwitchState;

} loconet_consumer_switch_sensor_t;


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

extern void loconet_consumer_switch_sensor_init( loconet_consumer_switch_sensor_t *pConsumer, loconet_bus_t *pBus );

extern uint8_t	loconet_consumer_register_notify_sensor( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_sensor pFunc );
extern uint8_t	loconet_consumer_register_notify_switch_request( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc );
extern uint8_t	loconet_consumer_register_notify_switch_report(  loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc );
extern uint8_t	loconet_consumer_register_notify_switch_outputs( loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc );
extern uint8_t	loconet_consumer_register_notify_switch_state(   loconet_consumer_switch_sensor_t *pConsumer, loconet_consumer_func_notify_switch pFunc );

//--------------------------------------------------------------------------
//	this is the function that must be registered at the "bus"
//	to be able to consume (handle) switch and sensor loconet messages
extern void loconet_consumer_switch_sensor_process( loconet_bus_consumer pConsumer, LnMsg *pMsg );
