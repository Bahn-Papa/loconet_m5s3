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

//#include <esp32-hal-uart.h>

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
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		G L O B A L   V A R I A B L E S
//
//==========================================================================

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

bool did_collision_happen_since_last_check( loconet_phy_uart_t *pUart )
{
	bool	isCollision;

	uart_get_collision_flag( pUart->uartNum, &isCollision );

	UART2.int_clr.rs485_clash = 1;		//	clear bit

	return( isCollision );
}


//**************************************************************************
//	loconet_phy_uart_rxtx_task
//--------------------------------------------------------------------------
//	description
//
void loconet_phy_uart_rxtx_task( void *pParameter )
{
	while( 1 )
	{
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
void loconet_phy_uart_init( loconet_phy_uart_t *pUart, loconet_bus_t *pBus )
{
	pUart->rxQueue	= xQueueCreateStatic( RX_QUEUE_LENGTH, sizeof( LnMsg ), rxQueueStorage, &rxQueueBuffer );
	pUart->txQueue	= xQueueCreateStatic( TX_QUEUE_LENGTH, sizeof( LnMsg ), txQueueStorage, &txQueueBuffer );

	loconet_msg_buffer_init( &(pUart->rxMsg) );
	loconet_bus_register_consumer( pBus, pUart, loconet_phy_uart_send );

	ESP_ERROR_CHECK( uart_param_config( UART_NUM_2, &uart_config ) );
	ESP_ERROR_CHECK( uart_set_pin( UART_NUM_2, pUart->txPin, pUart->rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE ) );
	ESP_ERROR_CHECK( uart_driver_install( UART_NUM_2, 0, 0, 0, NULL, 0 ) );

	uart_set_mode( UART_NUM_2, UART_MODE_RS485_APP_CTRL );
	UART2.rs485_conf.rx_busy_tx_en	= 0;	//	don't send while receiving => collision avoidance
	UART2.rs485_conf.tx_rx_en		= 1;	//	loopback (1), so collision detection works
}


//**************************************************************************
//	loconet_phy_uart_process
//--------------------------------------------------------------------------
//	this function will check if we got a new loconet message over
//	the physical lines. If so, then the message will be spread over
//	the bus to all other consumers, but not for us.
//
//	NOTE:
//	this function should be called in a periodical manner to get
//	loconet messages processed.
//
void loconet_phy_uart_process( loconet_phy_uart_t *pUart )
{
	LnMsg	aMsg;

	if( uxQueueMessageWaiting( pUart->rxQueue ) )
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
