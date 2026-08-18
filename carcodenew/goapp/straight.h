#ifndef __STRAIGHT_H
#define __STRAIGHT_H


#define PID_YAW_KP      (50.0f)   // 走直线比例(误差按度算, 越大纠偏越猛)
#define PID_YAW_KI      (0.0f)    // 积分(走直线一般不用)
#define PID_YAW_KD      (20.0f)   // 微分(阻尼)

#define STRAIGHT_BASE_SPEED  1200   // 走直线基础速度
#define STRAIGHT_MAX_SPEED   2000  // 走直线单侧轮最大速度


void timer5_Init(void);
void Yaw_Straight_Init(line_following* controller, int16_t base_speed, int16_t max_speed);
void Yaw_Straight_Control(int16_t target_yaw, int16_t* left_speed, int16_t* right_speed);


#endif
