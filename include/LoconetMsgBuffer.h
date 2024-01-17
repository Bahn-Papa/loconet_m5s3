#pragma once

//##########################################################################
//#
//#		LoconetMsgBuffer.h
//#
//#-------------------------------------------------------------------------
//#
//#	A buffer for one loconet message.
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
//#	File Version:	1		Date: 07.01.2024
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

#ifndef LN_BUF_SIZE
	#define LN_BUF_SIZE		(sizeof( lnMsg ))
#endif


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//----------------------------------------------------------------------
//	the message buffer tructure
//
typedef struct loconet_msg_buffer
{
	uint8_t		buffer[ LN_BUF_SIZE ];
	uint8_t		index;
	uint8_t		checkSum;
	uint8_t		expLen;

} loconet_msg_buffer_t;


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

extern void loconet_msg_buffer_init( loconet_msg_buffer_t *pBuffer );

extern lnMsg *loconet_msg_buffer_add_byte( loconet_msg_buffer_t *pBuffer, uint8_t newByte );
