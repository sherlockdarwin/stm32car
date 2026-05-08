#ifndef __STRAIGHT_H
#define __STRAIGHT_H


#define PID_YAW_KP      (2.0f)
#define PID_YAW_KI      (0.1f)
#define PID_YAW_KD      (0.8f)


void timer5_Init(void);
void Yaw_Straight_Init(line_following* controller, int16_t base_speed, int16_t max_speed);
void Yaw_Straight_Control(float target_yaw, uint16_t* left_speed, uint16_t* right_speed);


#endif

