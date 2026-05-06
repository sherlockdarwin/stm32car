#include "sys.h"

static uint16_t Sec=0;			//定义在定时器中断里自增的变量
uint16_t Min=0;
static uint16_t Timer_State=0;
static uint16_t cnt=0;

void volight(void)
{
	OLED_ShowNum(2, 5, Min, 2);		//不断刷新显示Num变量
	OLED_ShowString(2, 7, ":");
	OLED_ShowNum(2, 8, Sec, 2);
	if(Min==1)
	{		
		Timer_Stop();
	}
}

void PA8_Init(void)         //PA8初始化
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_8); // 默认熄灭
}

void PA8_Flash(void)     //PA8闪烁
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);   // 亮
	delay_ms(200);
	GPIO_SetBits(GPIOA, GPIO_Pin_8); // 灭
}



void Beep_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}
void Beep_Sound(void)
{
  GPIOB->BRR  = GPIO_Pin_5;  
  delay_ms(200);
  GPIOB->BSRR = GPIO_Pin_5;  
}


void Timer_Start(void)        //计时开始函数，可以直接调用
	{
		Sec=0;
		Min=0;
		Beep_Sound();
		//TIM_Cmd(TIM6, ENABLE);
		Timer_State=1;
  // 开启内核定时器
		PA8_Flash();
	}
	
	void Timer_Stop(void)       //计时结束函数
	{
		if(Timer_State==1)        //确认计时状态
		{
		//TIM_Cmd(TIM6, DISABLE);
		Beep_Sound();
		Timer_State=0;      // 关闭定时器
		PA8_Flash();
		}
    }

void SysTick_Handler(void)
{
	if(Timer_State == 1)
	{
		cnt++;
		if(cnt >= 200)  // 1秒
		{
			cnt=0;
			Sec=Sec+1;
			if(Sec >= 60)
			{
				Min++;
				Sec=0;
			}
		}
	}
}

