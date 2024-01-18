#pragma once

//##########################################################################
//#
//#		LoconetPhyUART.h
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"

#include "ln_opc.h"
#include "LoconetBus.h"
#include "LoconetMsgBuffer.h"


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


typedef enum
{
	IDLE			= 0,
	CD_BACKOFF,
	TX_COLLISION,
	TX,
	RX

} ln_tx_rx_status_t;


//----------------------------------------------------------------------
//	the loconet physical handler structure
//
typedef struct loconet_phy_uart
{
	TaskHandle_t			rxtxTask;
	QueueHandle_t			rxQueue;
	QueueHandle_t			txQueue;

	loconet_bus_t			*pBus;
	uart_port_t				uartNum;
	uint8_t					rxPin;
	uint8_t					txPin;
	bool					invertRx;
	bool					invertTx;

	loconet_msg_buffer_t	rxMsg;
	LnMsg 					txMsg;
	ln_tx_rx_status_t		state;
	uint64_t				cdBackoffStart;
	uint64_t				cdBackoffTimeout;
	uint64_t				collisionTimeout;

} loconet_phy_uart_t;


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

extern void loconet_phy_uart_init( loconet_phy_uart_t *pUart );

extern void loconet_phy_uart_send( loconet_bus_consumer pConsumer, LnMsg *pMsg );
