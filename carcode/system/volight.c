#include "sys.h"

static uint16_t Sec=0;			//定义在定时器中断里自增的变量
uint16_t Min=0;
static uint16_t Timer_State=0;
static uint16_t cnt=0;

void volight(void)
{
    SysTick_Configuration();
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


void timer5_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_InternalClockConfig(TIM5);

	TIM_TimeBaseInitTypeDef timerTIM;
	timerTIM.TIM_ClockDivision = TIM_CKD_DIV1;
	timerTIM.TIM_CounterMode = TIM_CounterMode_Up;
	timerTIM.TIM_Period = 10000 - 1;
	timerTIM.TIM_Prescaler = 7200 - 1;
	timerTIM.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &timerTIM);
	
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	
	
	NVIC_InitTypeDef timerNVIC;
	timerNVIC.NVIC_IRQChannel = TIM5_IRQn;
	timerNVIC.NVIC_IRQChannelCmd = ENABLE;
	timerNVIC.NVIC_IRQChannelPreemptionPriority = 2;
	timerNVIC.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&timerNVIC);
	
	TIM_Cmd(TIM5, ENABLE);
}



void SysTick_Configuration(void)
{
	// 配置内核中断优先级
	NVIC_SetPriority(SysTick_IRQn, 0x00);
	// 初始化1ms中断（72MHz）
	SysTick_Config(72000);
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
		if(cnt >= 1000)  // 1秒
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

