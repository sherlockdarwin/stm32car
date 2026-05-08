#include "sys.h"

line_following straight_controller;

void timer5_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    TIM_InternalClockConfig(TIM5);

	TIM_TimeBaseInitTypeDef timerTIM;
	timerTIM.TIM_ClockDivision = TIM_CKD_DIV1;
	timerTIM.TIM_CounterMode = TIM_CounterMode_Up;
	timerTIM.TIM_Period = 200 - 1;
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
		now_yaw = Yaw;
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}


void Yaw_Straight_Init(line_following* controller, int16_t base_speed, int16_t max_speed)
{
    // 赋值PID参数（航向专用）
    controller->kp = PID_YAW_KP;
    controller->ki = PID_YAW_KI;
    controller->kd = PID_YAW_KD;

    // 初始化PID缓存
    controller->last_error = 0.0f;
    controller->integral = 0.0f;

    // 电机速度参数
    controller->base_speed = base_speed;
    controller->max_speed = max_speed;

    // 解锁电机
    controller->motor_locked = false;
}


void Yaw_Straight_Control(float target_yaw, uint16_t* left_speed, uint16_t* right_speed)
{
    float error = now_yaw - target_yaw;

    if (error > 180.0f)  error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    float pid_output = pid_control(&straight_controller, error);

    differential_speed_control(&straight_controller, pid_output, left_speed, right_speed);
}


