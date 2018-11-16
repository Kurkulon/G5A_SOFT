#ifndef _TELEMETRY_H
#define _TELEMETRY_H


#define TELEMETRY_BAUD_RATE	20833ul
#define TELEMETRY_RX_BAUD_RATE	TELEMETRY_BAUD_RATE

#define TELEMETRY_RX_BUFFER	8
#define TELEMETRY_TX_BUFFER	(128 + 512 + 16)

#define TELEMETRY_PAUSE_MKS	(4 * (1000000ul / TELEMETRY_BAUD_RATE))
#define TELEMETRY_TIMEOUT_MKS	(200 * (1000000ul / TELEMETRY_BAUD_RATE))

typedef enum
{
	TELEMETRY_MODE_OFF = 0,
	TELEMETRY_MODE_RX,
	TELEMETRY_MODE_TX,
} telemetry_mode_type;

extern void Telemetry_Init();
extern void Telemetry_Idle();
//extern void Telemetry_Interrupt_Check();


/**************************************************************/
typedef struct __attribute__ ((packed))
{
	union
	{
		struct
		{
			byte command;	
			byte adress;	
		};
		u16 request;
	};
} telemetry_rx_type;

typedef struct __attribute__ ((packed))
{
	union
	{
		struct
		{
			byte command;	
			byte adress;	
		};
		u16 response;
	};
} telemetry_tx_type;
//-------------------------------------
#define TELEMETRY_ADRESS				(0x70)
#define TELEMETRY_COMMAND_GET_MAIN		(0x00)
#define TELEMETRY_COMMAND_GET_SPECIAL	(0x10)
#define TELEMETRY_COMMAND_GET_MEASURE	(0x20)
#define TELEMETRY_COMMAND_SET_MAIN		(0x80)
#define TELEMETRY_COMMAND_SET_SPECIAL	(0x90)
#define TELEMETRY_COMMAND_SAVE			(0xF0)
#define TELEMETRY_PARAMETR_SERIAL		(0x01)
#define TELEMETRY_PARAMETR_BAUD_RATE	(0x02)
#define TELEMETRY_PARAMETR_LEVEL_B		(0x01)
#define TELEMETRY_PARAMETR_LEVEL_M		(0x02)
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
} telemetry_rx_get_main_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
	u16 serial;
	u16 version;
} telemetry_tx_get_main_type;
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
} telemetry_rx_get_special_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
	u16 level_b;
	u16 level_m;
} telemetry_tx_get_special_type;
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
} telemetry_rx_get_measure_type;

typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
	u16 offset;
	u16 size;
} telemetry_rx_get_extended_measure_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
	u16 gk1;
	u16 gk2;
	u16 nnkb;
        u16 nnkm;
	u16 temperature_in;
} telemetry_tx_get_measure_type;
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
	u16 parametr;
	u16 data;
} telemetry_rx_set_main_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
} telemetry_tx_set_main_type;
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
	u16 parametr;
	u16 data;
} telemetry_rx_set_special_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
} telemetry_tx_set_special_type;
//-------------------------------------
typedef struct __attribute__ ((packed))
{
	telemetry_rx_type request;
} telemetry_rx_save_type;

typedef struct __attribute__ ((packed))
{
	telemetry_tx_type response;
} telemetry_tx_save_type;

#endif
