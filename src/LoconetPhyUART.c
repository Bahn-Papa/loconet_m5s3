//##########################################################################
//#
//#		LoconetPhyUART.c
//#
//#-------------------------------------------------------------------------
//#
//#	The functions in this library are used to spread loconet messages.
//#	This part will handle the sending and receiving of the loconet messages
//#	over a "real, physical" loconet.
//#	
//#	used resources: UART
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
//#	File Version:	1		Date: 13.01.2024
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

#include <hal/uart_hal.h>

#include "LoconetPhyUART.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define RX_QUEUE_LENGTH					64
#define TX_QUEUE_LENGTH					32


#define LOCONET_TICK_TIME				60
#define LOCONET_CARRIER_TICKS			20
#define LOCONET_COLLISION_TICKS			15

#define COLLISION_TIMEOUT_INCREMENT		(LOCONET_COLLISION_TICKS * LOCONET_TICK_TIME)
#define CD_BACKOFF_TIMEOUT_INCREMENT	(LOCONET_CARRIER_TICKS   * LOCONET_TICK_TIME)


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

StaticTask_t	xTaskBuffer;
StackType_t		xStack[ 200 ];

StaticQueue_t	rxQueueBuffer;
StaticQueue_t	txQueueBuffer;

uint8_t			rxQueueStorage[ RX_QUEUE_LENGTH * sizeof( LnMsg ) ];
uint8_t			txQueueStorage[ TX_QUEUE_LENGTH * sizeof( LnMsg ) ];

uart_config_t uart_config =
{
	.baud_rate	= 16667,
	.data_bits	= UART_DATA_8_BITS,
	.parity		= UART_PARITY_DISABLE,
	.stop_bits	= UART_STOP_BITS_1,
	.flow_ctrl	= UART_HW_FLOWCTRL_DISABLE,
};


//==========================================================================
//
//		I N T E R N A L   F U N C T I O N S
//
//==========================================================================

//**************************************************************************
//	did_collision_happen_since_last_check
//--------------------------------------------------------------------------
//	description
//
bool did_collision_happen_since_last_check( loconet_phy_uart_t *pUart )
{
	bool	isCollision;

	uart_get_collision_flag( pUart->uartNum, &isCollision );

	UART2.int_clr.rs485_clash_int_clr = 1;		//	clear bit

	return( isCollision );
}


//**************************************************************************
//	startCollisionTimer
//--------------------------------------------------------------------------
//	description
//
void startCollisionTimer( loconet_phy_uart_t *pUart )
{
	pUart->state			= TX_COLLISION;
	pUart->collisionTimeout	= (uint64_t)esp_timer_get_time() + LOCONET_COLLISION_TICKS;
}


//**************************************************************************
//	isCollisionTimerElapsed
//--------------------------------------------------------------------------
//	description
//
bool isCollisionTimerElapsed( loconet_phy_uart_t *pUart )
{
	return( (uint64_t)esp_timer_get_time() > pUart->collisionTimeout );
}


//**************************************************************************
//	startCDBackoffTimer
//--------------------------------------------------------------------------
//	description
//
void startCDBackoffTimer( loconet_phy_uart_t *pUart )
{
	pUart->state			= CD_BACKOFF;
	pUart->cdBackoffStart	= (uint64_t)esp_timer_get_time();
	pUart->cdBackoffTimeout	= pUart->cdBackoffStart + LOCONET_TICK_TIME;
}


//**************************************************************************
//	isCDBackoffTimerElapsed
//--------------------------------------------------------------------------
//	description
//
bool isCDBackoffTimerElapsed( loconet_phy_uart_t *pUart )
{
	return( (uint64_t)esp_timer_get_time() > pUart->cdBackoffTimeout );
}


//**************************************************************************
//	loconet_phy_uart_rxtx_task
//--------------------------------------------------------------------------
//	description
//
void loconet_phy_uart_rxtx_task( void *pParameter )
{
	loconet_phy_uart_t	*pUart;
	LnMsg				*pMsg;
	uint8_t				dataByte;

	pUart = (loconet_phy_uart_t *)pParameter;

	while( 1 )
	{
		if( 0 < uart_read_bytes( pUart->uartNum, &dataByte, (uint32_t)1, 0 ) )
		{
			pUart->state = RX;

			do
			{
				pMsg = loconet_msg_buffer_add_byte( &(pUart->rxMsg), dataByte );

				if( NULL != pMsg )
				{
					xQueueSendToBack( pUart->rxQueue, (void *)pMsg, 0 );
				}

			} while ( 0 < uart_read_bytes( pUart->uartNum, &dataByte, (uint32_t)1, 0 ) );

			startCDBackoffTimer( pUart );
		}
		else if( (RX == pUart->state) && isCDBackoffTimerElapsed( pUart ) )
		{
			pUart->state = IDLE;
		}
		else if( IDLE == pUart->state )
		{
		}
		else if( (TX_COLLISION == pUart->state) && isCollisionTimerElapsed( pUart ) )
		{
			startCDBackoffTimer( pUart );
		}
	}
}


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

//**************************************************************************
//	loconet_phy_uart_init
//--------------------------------------------------------------------------
//	description
//
void loconet_phy_uart_init( loconet_phy_uart_t *pUart )
{
	pUart->rxQueue	= xQueueCreateStatic( RX_QUEUE_LENGTH, sizeof( LnMsg ), rxQueueStorage, &rxQueueBuffer );
	pUart->txQueue	= xQueueCreateStatic( TX_QUEUE_LENGTH, sizeof( LnMsg ), txQueueStorage, &txQueueBuffer );

	loconet_msg_buffer_init( &(pUart->rxMsg) );
	loconet_bus_register_consumer( pUart->pBus, pUart, loconet_phy_uart_send );

	ESP_ERROR_CHECK( uart_param_config( pUart->uartNum, &uart_config ) );
	ESP_ERROR_CHECK( uart_set_pin( pUart->uartNum, pUart->txPin, pUart->rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) );
	ESP_ERROR_CHECK( uart_driver_install( pUart->uartNum, 0, 0, 0, NULL, 0 ) );

	uart_set_mode( pUart->uartNum, UART_MODE_RS485_APP_CTRL );
	UART2.rs485_conf.rs485rxby_tx_en	= 0;	//	don't send while receiving => collision avoidance
	UART2.rs485_conf.rs485tx_rx_en		= 1;	//	loopback (1), so collision detection works
}


//**************************************************************************
//	loconet_phy_uart_process
//--------------------------------------------------------------------------
//	this function will check if we got a new loconet message over
//	the physical lines. If so, the message will be spread over the bus
//	to all other consumers, but not to us.
//
//	NOTE:
//	this function should be called in a periodical manner to get
//	loconet messages processed.
//
void loconet_phy_uart_process( loconet_phy_uart_t *pUart )
{
	LnMsg	aMsg;

	if( uxQueueMessagesWaiting( pUart->rxQueue ) )
	{
		//--------------------------------------------------------------
		//	we received a loconet msg
		//
		xQueueReceive( pUart->rxQueue, &aMsg, 0 );

		//--------------------------------------------------------------
		//	now spread this msg over the bus to all other consumers,
		//	but not to ourself
		//
		loconet_bus_broadcast( pUart->pBus, &aMsg, loconet_phy_uart_send );
	}
}


//**************************************************************************
//	loconet_phy_uart_send
//--------------------------------------------------------------------------
//	this function will send the given loconet message over the physical
//	lines to the loconet.
//	Normaly this function will be called automaticly if there is a new
//	loconet message spread on the bus.
//	But it can be called directly, also.
//
void loconet_phy_uart_send( loconet_bus_consumer pConsumer, LnMsg *pMsg )
{
	loconet_phy_uart_t	*pUart	= (loconet_phy_uart_t *)pConsumer;

	xQueueSendToBack( pUart->txQueue, (void *)pMsg, 0 );
}
