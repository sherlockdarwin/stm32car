#include "sys.h"

uint16_t sensor_data[8];

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SWJ_Config();
	systick_init();
	while(1)
	{
		System_Init();
	}
}
