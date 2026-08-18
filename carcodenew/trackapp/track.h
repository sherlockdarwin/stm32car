#ifndef __TRACK_H
#define __TRACK_H


/* ===== 循迹(巡线) PID 与速度参数 —— 按需调整 ===== */
#define LINE_KP   250.0f   // 比例(转向力度, 循迹建议 80~200)
#define LINE_KI   0.0f     // 积分(循迹通常不用)
#define LINE_KD   40.0f    // 微分(阻尼, 防抖)

#define LINE_BASE_SPEED  1200   // 基础速度(电机死区之上)
#define LINE_MAX_SPEED   2000  // 单侧轮最大速度上限
#define LINE_MAX_STEER   600   // 最大转向量(≤基础速度, 防止内侧轮倒转飞线)

/* 积分限幅(误差单位)。之前偏小(20), 圆弧上积分压不住, 已调大 */
#define LINE_INTEGRAL_LIMIT_LARGE   250.0f
#define LINE_INTEGRAL_LIMIT_MEDIUM  150.0f
#define LINE_INTEGRAL_LIMIT_SMALL   80.0f

/* 黑线去抖阈值: 连续 N 次全白才判"出线", 连续 M 次检测到线才判"回线" */
#define OFF_LINE_CONFIRM_CNT  15
#define ON_LINE_CONFIRM_CNT   3      


typedef struct {
    // PID参数
    float kp;
    float ki;
    float kd;

    float last_error;       // 上次偏差
    float integral;         // 积分累加

    // 控制参数
    int16_t base_speed;     // 基础速度
    int16_t max_speed;      // 最大速度

    // 传感器权重 (8个传感器的位置权重)
    float sensor_weights[8];

} line_following;


void line_following_init(line_following* controller);
bool check_sensors_safe(line_following* controller, uint16_t* sensor_values);
float calculate_error(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value);
float pid_control(line_following* controller, float error);
void differential_speed_control(line_following* controller, float pid_output, int16_t* left_speed, int16_t* right_speed);
void follow_line(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value);

void timer7_Init(void);

#endif
