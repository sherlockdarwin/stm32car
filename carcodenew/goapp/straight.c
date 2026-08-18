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
    timerNVIC.NVIC_IRQChannelPreemptionPriority = 1;
    timerNVIC.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&timerNVIC);

    // 不在此处使能; 由 main 进入直行模式时 TIM_Cmd(TIM5, ENABLE)
}


void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) == SET) {
        if (car_run && straight_flag) {   // 直行模式且运行中才由 TIM5 控制
            now_yaw = Yaw;
            Yaw_Straight_Control(target_yaw, &left_pwm, &right_pwm);
        }
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }
}


void Yaw_Straight_Init(line_following* controller, int16_t base_speed, int16_t max_speed)
{
    controller->kp = PID_YAW_KP;
    controller->ki = PID_YAW_KI;
    controller->kd = PID_YAW_KD;

    controller->last_error = 0.0f;
    controller->integral = 0.0f;

    controller->base_speed = base_speed;
    controller->max_speed = max_speed;
}


void Yaw_Straight_Control(int16_t target_yaw, int16_t* left_speed, int16_t* right_speed)
{
    // int16 截断(now-target)已经自动环绕到 -180°~+180°
    int16_t err_raw = now_yaw - target_yaw;

    // 把原始角度(±32768=±180°)换算成度, 再给PID
    float error = (float)err_raw * 0.0054931640625f;   // 180/32768

    float pid_output = pid_control(&straight_controller, error);

    differential_speed_control(&straight_controller, pid_output, left_speed, right_speed);
}
