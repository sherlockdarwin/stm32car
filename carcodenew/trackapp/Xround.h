#ifndef __XROUND_H
#define __XROUND_H


#define RADIUS         1.0f    // 圆弧半径(米)
#define WHEEL_DISTANCE 0.15f   // 左右轮中心距(米)
#define TOTAL_TIME     8.0f    // 总行驶时间(秒)
#define PWM_BASE        3600   // 基准PWM(0~7199)
#define PWM_MAX         7199
#define PWM_MIN         0


extern volatile float total_dist;   // 累计行驶距离(走圆弧用)

void Round(int16_t* left_speed, int16_t* right_speed);
void timer1_Init(void);


#endif
