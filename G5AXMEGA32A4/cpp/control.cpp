#include "common.h"
#include "control.h"
#include "hardware.h"
#include "options.h"

u16 control_level_m;
u16 control_level_b;


/************************************************************************/
void Control_Init()
{
         control_level_m = 0xFFFF;
         control_level_b = 0xFFFF;
}

void Control_Idle()
{
	if(control_level_m != Options_Get_Level_M())
	{
		control_level_m = Options_Get_Level_M();
		AD5312_Set(AD5312_CHANNEL_LEVEL_M, control_level_m);
	}
	if(control_level_b != Options_Get_Level_B())
	{
		control_level_b = Options_Get_Level_B();
		AD5312_Set(AD5312_CHANNEL_LEVEL_B, control_level_b);
	}
}

