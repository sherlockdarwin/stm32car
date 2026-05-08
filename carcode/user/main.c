#include "sys.h"

uint16_t sensor_data[8];
float target_yaw;
float now_yaw;

int main(void)
{
	uint16_t left_pwm, right_pwm;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SWJ_Config();
	systick_init();
	System_Init();
	
	JY61P_Full_Init_Sequence();
	Yaw_Straight_Init(&straight_controller, 200, 255);
	
	target_yaw = Yaw; //确定初始航向角
	
	line_following_init(&line_controller);
	
	while(1)
	{
		if(KeyNum == 1)
		{
			Yaw_Straight_Control(target_yaw, &left_pwm, &right_pwm);
			Set_Pwm(left_pwm, right_pwm);
			
		}
		
		if(KeyNum == 2)
		{
			Sensor_Read_All(sensor_data);
			follow_line(&line_controller, sensor_data, 1);
			
		}
		
	}
}


