#include "sys.h"

void timer5_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_InternalClockConfig(TIM5);

	TIM_TimeBaseInitTypeDef timerTIM;
	timerTIM.TIM_ClockDivision = TIM_CKD_DIV1;
	timerTIM.TIM_CounterMode = TIM_CounterMode_Up;
	timerTIM.TIM_Period = 100 - 1;
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


void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}


