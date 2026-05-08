#include "sys.h"

char Serial_RxPacket[100];
uint8_t Serial_RxFlag;

void sendblue(void)
{
	if (Serial_RxFlag == 1)		//如果接收到数据包
	{
		float kp = line_controller.kp;
		float ki = line_controller.ki;
		float kd = line_controller.kd;
		
		char * Tag = strtok(Serial_RxPacket,",");  //判断是否是按键数据包
		if(strcmp(Tag,"key") == 0)
		{
			char * Name = strtok(NULL,",");     //判断内容和动作
			char * Action = strtok(NULL,",");
				
			if(strcmp(Name,"kp++")==0&&strcmp(Action,"up") == 0)
			{
				kp=kp+5;
				line_controller.kp = kp;
				printf("%f\r\n",kp);
			}
			else if(strcmp(Name,"kp--")==0&&strcmp(Action,"up") == 0)
			{
				kp=kp-5;
				line_controller.kp = kp;
				printf("%f\r\n",kp);
			}
			else if(strcmp(Name,"ki++")==0&&strcmp(Action,"up") == 0)
			{
				ki=ki+5;
				line_controller.ki = ki;
				printf("%f\r\n",ki);
			}
			else if(strcmp(Name,"ki--")==0&&strcmp(Action,"up") == 0)
			{
				ki=ki-5;
				line_controller.ki = ki;
				printf("%f\r\n",ki);
			}
			else if(strcmp(Name,"kd--")==0&&strcmp(Action,"up") == 0)
			{
				kd=kd-5;
				line_controller.kd = kd;
				printf("%f\r\n",kd);
			}
			else if(strcmp(Name,"kd++")==0&&strcmp(Action,"up") == 0)
			{
				kd=kd+5;
				line_controller.kd = kd;
				printf("%f\r\n",kd);
			}
			printf("%s\r\n",Tag);
			printf("%s\r\n",Name);
			printf("%s\r\n",Action);
			
		}
			
		Serial_RxFlag = 0;			//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
	}
}

