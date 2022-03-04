//#include "common.h"
#include "hardware.h"
#include "options.h"

MainVars options;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Options_Init()
{
	Options_Set_Serial(Options_Get_Serial());	// для проверки
	Options_Set_Telemetry_Baud_Rate(Options_Get_Telemetry_Baud_Rate());	// для проверки
	Options_Set_Level_B(Options_Get_Level_B());
	Options_Set_Level_M(Options_Get_Level_M());
	SetGenFreq(options.gen_freq);
	SetWindowCount(options.win_count);
	SetWindowTime(options.win_time);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool Options_Set_Telemetry_Baud_Rate(u16 baud_rate)
{
	if((baud_rate >= TELEMETRY_TX_BAUD_RATE_X1) && (baud_rate <= TELEMETRY_TX_BAUD_RATE_X4))
	{
//		PORTB.OUTSET = PIN2_bm;

		options.baud_rate = baud_rate;
		return true;
	}
	else
	{
		options.baud_rate = TELEMETRY_TX_BAUD_RATE_X1;
		return false;
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Options_Set_Level_B(u16 level)
{
	static u16 pl = 0;

	if(level > OPTIONS_LEVEL_M_MAX)
	{	
		level = OPTIONS_LEVEL_M_MAX;
	};

	if (pl != level)
	{
		AD5312_Set(AD5312_CHANNEL_LEVEL_B, options.level_b = pl = level);
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void Options_Set_Level_M(u16 level)
{
	static u16 pl = 0;

	if(level > OPTIONS_LEVEL_M_MAX)
	{	
		level = OPTIONS_LEVEL_M_MAX;
	};

	if (pl != level)
	{
		AD5312_Set(AD5312_CHANNEL_LEVEL_M, options.level_m = pl = level);
	};
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
