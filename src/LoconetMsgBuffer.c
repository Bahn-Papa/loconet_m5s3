//##########################################################################
//#
//#		LoconetMsgBuffer.c
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

#include "LoconetMsgBuffer.h"


//==========================================================================
//
//		D E F I N I T I O N S
//
//==========================================================================

#define	LN_CHECKSUM_SEED		((uint8_t)0xFF)
#define LN_MSG_HEAD_SIZE		2


//==========================================================================
//
//		T Y P E   D E F I N I T I O N S
//
//==========================================================================


//==========================================================================
//
//		S T A T I C   F U N C T I O N   D E C L A R A T I O N S
//
//==========================================================================

//**********************************************************************
//	loconet_msg_buffer_get_msg
//----------------------------------------------------------------------
//
static lnMsg * loconet_msg_buffer_get_msg( loconet_msg_buffer_t *pBuffer )
{
	if( LN_MSG_HEAD_SIZE > pBuffer->index )
	{
		//----------------------------------------------------------
		//	we need 2 bytes to get the message length
		//
		return( NULL );
	}

	if( 0 == pBuffer->expLen )
	{
		//----------------------------------------------------------
		//	If it's a fixed length packet, compute the length from
		//	the OPC_Code, else get the length from byte 1
		//
		pBuffer->expLen = LOCONET_PACKET_SIZE(	pBuffer->buffer[ 0 ],
												pBuffer->buffer[ 1 ] );
	}

	if( pBuffer->index == pBuffer->expLen )
	{
		//----------------------------------------------------------
		//	okay, we read all bytes
		//	so if the check sum is okay
		//	then return a pointer to the message
		//
		if( 0 == pBuffer->checkSum )
		{
			return( (lnMsg *)pBuffer->buffer );
		}
	}

	return( NULL );
}


//==========================================================================
//
//		E X T E R N   F U N C T I O N S
//
//==========================================================================

//**********************************************************************
//	loconet_msg_buffer_init
//----------------------------------------------------------------------
//
void loconet_msg_buffer_init( loconet_msg_buffer_t *pBuffer )
{
	pBuffer->index		= 0;
	pBuffer->expLen		= 0;
	pBuffer->checkSum	= LN_CHECKSUM_SEED;

	for( uint8_t idx = 0 ; LN_BUF_SIZE > idx ; idx++ )
	{
		pBuffer->buffer[ idx ] = 0;
	}
}


//**********************************************************************
//	loconet_msg_buffer_add_byte
//----------------------------------------------------------------------
//
lnMsg *loconet_msg_buffer_add_byte( loconet_msg_buffer_t *pBuffer, uint8_t newByte )
{
	if( LN_BUF_SIZE > pBuffer->index )
	{
		//----------------------------------------------------------
		//	if the new byte is a loconet OP code then reset
		//	the buffer to empty
		//
		if( (newByte & LOCONET_OPC_MASK) != 0 )
		{
			loconet_msg_buffer_init( pBuffer );
		}

		pBuffer->buffer[ pBuffer->index++ ] = newByte;

		if(		(LN_MSG_HEAD_SIZE >= pBuffer->index)
			||	(pBuffer->expLen  >= pBuffer->index)	)
		{
			pBuffer->checkSum ^= newByte;
		}
	}

	return( loconet_msg_buffer_get_msg( pBuffer ) );
}
