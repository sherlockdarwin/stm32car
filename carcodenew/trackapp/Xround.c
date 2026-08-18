#include "sys.h"

const float ARC_LENGTH = 3.1415926f * RADIUS / 2.0f;   // 1/4 圆弧总长度
const float CENTER_SPEED = ARC_LENGTH / TOTAL_TIME;    // 中心平均速度
const float DELTA_DIST = CENTER_SPEED * 0.02f;         // 20ms 内行驶距离

volatile float total_dist = 0.0f;   // 累计行驶距离


void Round(int16_t* left_speed, int16_t* right_speed)
{
    total_dist += DELTA_DIST;

    if (total_dist >= ARC_LENGTH) {
        *left_speed = 0;
        *right_speed = 0;
        return;   // 完成圆弧: 停车
    }

    *left_speed  = (int16_t)(PWM_BASE * (RADIUS - WHEEL_DISTANCE / 2.0f) / RADIUS);
    *right_speed = (int16_t)(PWM_BASE * (RADIUS + WHEEL_DISTANCE / 2.0f) / RADIUS);
}


void timer1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   // TIM1 在 APB2(修复原 APB1 笔误)
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

    // 不在此处使能; 由 main 进入走圆弧模式时 TIM_Cmd(TIM1, ENABLE)
}


void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET) {
        if (car_run && straight_flag) {   // 走圆弧模式且运行中
            if (waypoint_count % 2 == 0) Round(&left_pwm, &right_pwm);
            else                         Round(&right_pwm, &left_pwm);
        }
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
