#ifndef _MANCHESTER_H
#define _MANCHESTER_H

#include "hardware.h"
#include "telemetry.h"

#define MANCHESTER_RX_BIT 	(byte)(RXD_TIMER_FREQ / TELEMETRY_BAUD_RATE)

typedef enum
{
	MANCHESTER_RX_STATUS_HANDLE = 0,
	MANCHESTER_RX_STATUS_READY,
	MANCHESTER_RX_STATUS_ERROR_LENGTH,
	MANCHESTER_RX_STATUS_ERROR_STRUCT,
} manchester_rx_status_type;

manchester_rx_status_type Manchester_RX_Parse(byte length);	// 1/2 period
u16 Manchester_RX_Get_Data();
bool Manchester_RX_Get_Type();

typedef enum
{
	MANCHESTER_TX_STATUS_WAIT,
	MANCHESTER_TX_STATUS_READY,
	MANCHESTER_TX_STATUS_LO,
	MANCHESTER_TX_STATUS_HI,
} manchester_tx_status_type;

manchester_tx_status_type Manchester_TX_Parse();
void Manchester_TX_Set_Data(u16 data);
//void Manchester_TX_Set_Type(bool type);

#endif
