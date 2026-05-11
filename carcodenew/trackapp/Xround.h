#ifndef __XROUND_H
#define __XROUND_H


#define RADIUS         1.0f    // 圆弧半径：固定1米
#define WHEEL_DISTANCE 0.15f    // 小车左右轮中心距(米)
#define TOTAL_TIME     8.0f    // 总行驶时间：8秒（≤10秒，可调整）
#define PWM_BASE        3600   // 基准PWM值（0~7199，TIM2自动重装载7199）
#define PWM_MAX         7199   // PWM最大值（硬件限制）
#define PWM_MIN         0      // PWM最小值


void Round(uint16_t* left_speed, uint16_t* right_speed);
void timer1_Init(void);


#endif

