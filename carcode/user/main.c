#include "sys.h"

uint16_t sensor_data[8];
float target_yaw;
float now_yaw;
float get_Lenconder;//左编码器B
float get_Renconder;//右编码器A
float set_encoder=66;    //初始化设定值
uint16_t left_pwm, right_pwm;
bool safe;

int main(void)
{
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SWJ_Config();
	systick_init();
	System_Init();
	//Timer_Start();
	OLED_ShowString(2, 1, "Num:");	//2行1列显示字符串Num:
	
	JY61P_Full_Init_Sequence();
	Yaw_Straight_Init(&straight_controller, 200, 255);
	
	target_yaw = Yaw; //确定初始航向角
	
	line_following_init(&line_controller);
	
	while(1)
	{
		get_Renconder = Read_Encoder(ENCODER_A);
		get_Lenconder = Read_Encoder(ENCODER_B);
		
		Sensor_Read_All(sensor_data);
		safe = check_sensors_safe(&line_controller, sensor_data);
		
		printf("[plot,%f,%f]",get_Lenconder,set_encoder);  //显示左轮波形图
		volight();
		
		if (Serial_RxFlag == 1)		//如果接收到数据包
		{
			float kp;
			float ki;
			float kd;
		
			if(KeyNum == 2)
			{
				kp = line_controller.kp;
				ki = line_controller.ki;
				kd = line_controller.kd;			
			}
			if(KeyNum == 1)
			{
				kp = straight_controller.kp;
				ki = straight_controller.ki;
				kd = straight_controller.kd;			
			}
			
			char * Tag = strtok(Serial_RxPacket,",");  //判断是否是按键数据包
			if(strcmp(Tag,"key") == 0)
			{
				char * Name = strtok(NULL,",");     //判断内容和动作
				char * Action = strtok(NULL,",");
				
			if(strcmp(Name,"kp++")==0&&strcmp(Action,"up") == 0)
			{
				kp=kp+5;
				printf("%f\r\n",kp);
			}
			else if(strcmp(Name,"kp--")==0&&strcmp(Action,"up") == 0)
			{
				kp=kp-5;
				printf("%f\r\n",kp);
			}
			else if(strcmp(Name,"ki++")==0&&strcmp(Action,"up") == 0)
			{
				ki=ki+5;
				printf("%f\r\n",ki);
			}
			else if(strcmp(Name,"ki--")==0&&strcmp(Action,"up") == 0)
			{
				ki=ki-5;
				printf("%f\r\n",ki);
			}
			else if(strcmp(Name,"kd--")==0&&strcmp(Action,"up") == 0)
			{
				kd=kd-5;
				printf("%f\r\n",kd);
			}
			else if(strcmp(Name,"kd++")==0&&strcmp(Action,"up") == 0)
			{
				kd=kd+5;
				printf("%f\r\n",kd);
			}
			
			if(KeyNum == 2)
			{
				line_controller.kp = kp;
				line_controller.ki = ki;
				line_controller.kd = kd;			
			}
			if(KeyNum == 1)
			{
				straight_controller.kp = kp;
				straight_controller.ki = ki;
				straight_controller.kd = kd;			
			}
			
				printf("%s\r\n",Tag);
				printf("%s\r\n",Name);
				printf("%s\r\n",Action);
			
			}
			
			Serial_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}
		
		if(KeyNum == 1)
		{
			if(safe == 1)
			{
				left_pwm = 0;
				right_pwm = 0;
				KeyNum = 0;
				TIM_Cmd(TIM5, DISABLE);
				TIM_Cmd(TIM6, ENABLE);
				Beep_Sound();
				PA8_Flash();
			}
			Set_Pwm(left_pwm, right_pwm);
		}
		
		if(KeyNum == 2)
		{
			if(safe == 0)
			{
				TIM_Cmd(TIM5, ENABLE);
				TIM_Cmd(TIM6, ENABLE);
				Beep_Sound();
				PA8_Flash();
			}
			follow_line(&line_controller, sensor_data, 1);
		}
		
	}
}


