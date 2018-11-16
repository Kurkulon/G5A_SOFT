#include "common.h"
#include "main.h"
#include "hardware.h"
#include "options.h"
#include "telemetry.h"
#include "control.h"
#include "measure.h"

/******************************************************************************************/

extern const byte fusedata[] __attribute__ ((section (".fuse"))) = {0xFF, 0xFF, 0xFE, 0xF7, 0xE2};

/******************************************************************************************/

int main(void) 
{
	Hardware_Init();
	Options_Init();
	Telemetry_Init();
//	Control_Init();
	Measure_Init();

//	TM16 tm;

//	tm.Reset();

  	for(;;)
	{
		PORTB.OUTSET = PIN0_bm;

		UpdateHardware();

		//if (tm.Check(MS2RT(3000)))
		//{
		//	EnableGen();
		//	PORTB.OUTTGL = PIN0_bm;
		//};

		Telemetry_Idle();

		PORTB.OUTCLR = PIN0_bm;

//		Control_Idle();
		Measure_Idle();
	};

	return 0;
}
