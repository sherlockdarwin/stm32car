#include "sys.h"

const float ARC_LENGTH = 3.1415926f * RADIUS / 2.0f;  // 1/4圆弧总长度
const float CENTER_SPEED = ARC_LENGTH / TOTAL_TIME;   // 小车中心平均速度
const float DELTA_DIST = CENTER_SPEED * 0.02f;        // 20ms中断行驶的微小距离

// 全局变量（volatile防止编译器优化，中断专用）
volatile float total_dist = 0.0f;  // 累计行驶距离


void Round(uint16_t* left_speed, uint16_t* right_speed)//右转
{
	total_dist += DELTA_DIST;
		
		//是否完成1/4圆弧
	if(total_dist >= ARC_LENGTH)
	{
			// 到达终点：电机停转
		*left_speed = 0;
		*right_speed = 0;
	}
		
	*left_speed = (int)(PWM_BASE * (RADIUS - WHEEL_DISTANCE/2.0f) / RADIUS);
	*right_speed = (int)(PWM_BASE * (RADIUS + WHEEL_DISTANCE/2.0f) / RADIUS);
}


void timer1_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_InternalClockConfig(TIM1);

	TIM_TimeBaseInitTypeDef timerTIM;
	timerTIM.TIM_ClockDivision = TIM_CKD_DIV1;
	timerTIM.TIM_CounterMode = TIM_CounterMode_Up;
	timerTIM.TIM_Period = 200 - 1;
	timerTIM.TIM_Prescaler = 7200 - 1;
	timerTIM.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &timerTIM);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	
	NVIC_InitTypeDef timerNVIC;
	timerNVIC.NVIC_IRQChannel = TIM1_UP_IRQn;
	timerNVIC.NVIC_IRQChannelCmd = ENABLE;
	timerNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	timerNVIC.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&timerNVIC);
	
	TIM_Cmd(TIM1, ENABLE);
}


void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		if(count%2==0)
		{
			Round(&left_pwm, &right_pwm);
		}
		else if(count%2==1)
		{
			Round(&right_pwm, &left_pwm);
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
	}
}


