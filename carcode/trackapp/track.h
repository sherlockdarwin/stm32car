#ifndef __TRACK_H
#define __TRACK_H


#define PID_MOTOR_KP  (1.5f)
#define PID_MOTOR_KI  (0.08f)
#define PID_MOTOR_KD  (0.5f)

typedef struct {
    // PID参数 PID Parameters
    float kp;               // 比例系数 Proportional coefficient
    float ki;               // 积分系数 Integral coefficient
    float kd;               // 微分系数 Derivative coefficient

    float last_error;       // 上次偏差 Last error
    float integral;         // 积分累加 Integral accumulation

    // 控制参数 Control Parameters
    int16_t base_speed;     // 基础速度 Base speed
    int16_t max_speed;      // 最大速度 Max speed

    // 传感器权重 Sensor weights (8个传感器的位置权重)
    float sensor_weights[8];

    bool motor_locked;      // 初始锁定电机，防止乱跑 Initially lock motors to prevent random movement

} line_following;

extern line_following line_controller;

void line_following_init(line_following* controller);
bool check_sensors_safe(line_following* controller, uint16_t* sensor_values);
float calculate_error(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value);    
float pid_control(line_following* controller, float error);
void differential_speed_control(line_following* controller, float pid_output, int16_t* left_speed, int16_t* right_speed);
void follow_line(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value);


#endif

