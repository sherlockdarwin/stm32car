#include "sys.h"

line_following line_controller;
volatile float dbg_line_error = 0.0f;   // 调试: 循迹偏差(error), 供OLED显示

/* 黑线"出线/回线"检测去抖状态 */
static uint16_t off_line_cnt = 0;    // 连续全白计数
static uint16_t on_line_cnt  = 0;    // 连续检测到线计数
static bool     line_state    = false; // false=丢线(上电在盲区), true=在线上(有黑线)


void line_following_init(line_following* controller)
{
    controller->kp = LINE_KP;
    controller->ki = LINE_KI;
    controller->kd = LINE_KD;

    controller->last_error = 0.0f;
    controller->integral = 0.0f;

    controller->base_speed = LINE_BASE_SPEED;
    controller->max_speed = LINE_MAX_SPEED;

    controller->sensor_weights[0] = -5.0f;
    controller->sensor_weights[1] = -4.0f;
    controller->sensor_weights[2] = -2.0f;
    controller->sensor_weights[3] = -1.0f;
    controller->sensor_weights[4] = 1.0f;
    controller->sensor_weights[5] = 2.0f;
    controller->sensor_weights[6] = 4.0f;
    controller->sensor_weights[7] = 5.0f;
}


/*
 * 黑线"出线/回线"判定(带去抖)
 * 数字量传感器: 读到 0 = 白, 1 = 黑(压线)
 * 返回 true = 在线上(循迹); false = 全白丢线(转入直行盲走)
 * 关键: 全白必须连续 OFF_LINE_CONFIRM_CNT 次才判"出线", 避免单个采样点/缝隙/尖角造成误判
 */
bool check_sensors_safe(line_following* controller, uint16_t* sensor_values)
{
    (void)controller;

    bool all_white = true;
    for (int i = 0; i < 8; i++) {
        if (sensor_values[i] != 0) { all_white = false; break; }
    }

    if (all_white) {
        off_line_cnt++;
        if (off_line_cnt > OFF_LINE_CONFIRM_CNT) off_line_cnt = OFF_LINE_CONFIRM_CNT;
        if (off_line_cnt >= OFF_LINE_CONFIRM_CNT) line_state = false;
        on_line_cnt = 0;
    } else {
        on_line_cnt++;
        if (on_line_cnt > ON_LINE_CONFIRM_CNT) on_line_cnt = ON_LINE_CONFIRM_CNT;
        if (on_line_cnt >= ON_LINE_CONFIRM_CNT) line_state = true;
        off_line_cnt = 0;
    }
    return line_state;
}


float calculate_error(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value)
{
    float weighted_sum = 0.0f;
    int active_sensors = 0;

    for (int i = 0; i < 8; i++) {
        if (sensor_values[i] == line_raw_value) {
            weighted_sum += controller->sensor_weights[i];
            active_sensors++;
        }
    }

    if (active_sensors == 0) {
        return controller->last_error;   // 丢线: 沿用上次偏差
    }
    // 用倒数表+乘法代替浮点除法, 避免中断里 __aeabi_fdiv 导致的 HardFault
    static const float inv[9] = {0.0f, 1.0f, 0.5f, 0.33333334f, 0.25f, 0.2f, 0.16666667f, 0.14285714f, 0.125f};
    return weighted_sum * inv[active_sensors];
}


float pid_control(line_following* controller, float error)
{
    if (fabsf(error) < 0.6f) {
        error = 0.0f;
    }

    if ((controller->last_error > 0 && error < 0) || (controller->last_error < 0 && error > 0)) {
        controller->integral = 0.0f;   // 偏差过零, 清积分
    }

    // 动态积分限幅
    float integral_limit;
    if (fabsf(error) > 3.0f)      integral_limit = LINE_INTEGRAL_LIMIT_LARGE;
    else if (fabsf(error) > 1.5f) integral_limit = LINE_INTEGRAL_LIMIT_MEDIUM;
    else                          integral_limit = LINE_INTEGRAL_LIMIT_SMALL;

    controller->integral += error;
    controller->integral = (float)PWM_Limit((int)controller->integral,
                                            (int)(-integral_limit),
                                            (int)integral_limit);

    float derivative = error - controller->last_error;

    float output = controller->kp * error
                 + controller->ki * controller->integral
                 + controller->kd * derivative;

    controller->last_error = error;
    return output;
}


void differential_speed_control(line_following* controller, float pid_output, int16_t* left_speed, int16_t* right_speed)
{
    // 关键: 限制转向量, 防止 PID 输出过大导致差速失控、内侧轮倒转飞线
    float max_steer = (float)LINE_MAX_STEER;
    if (pid_output >  max_steer) pid_output =  max_steer;
    if (pid_output < -max_steer) pid_output = -max_steer;

    float left  = controller->base_speed + pid_output;
    float right = controller->base_speed - pid_output;

    // 限幅到 [0, max_speed], 内侧轮最多停转、不倒转
    *left_speed  = (int16_t)PWM_Limit((int)left,  0, controller->max_speed);
    *right_speed = (int16_t)PWM_Limit((int)right, 0, controller->max_speed);
}


void follow_line(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value)
{
    float error = calculate_error(controller, sensor_values, line_raw_value);
    dbg_line_error = error;   // 调试: 存偏差供OLED显示
    float pid_output = pid_control(controller, error);
    differential_speed_control(controller, pid_output, &left_pwm, &right_pwm);
}


void timer7_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    TIM_InternalClockConfig(TIM7);

    TIM_TimeBaseInitTypeDef timerTIM;
    timerTIM.TIM_ClockDivision = TIM_CKD_DIV1;
    timerTIM.TIM_CounterMode = TIM_CounterMode_Up;
    timerTIM.TIM_Period = 200 - 1;
    timerTIM.TIM_Prescaler = 7200 - 1;
    timerTIM.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM7, &timerTIM);

    TIM_ClearFlag(TIM7, TIM_FLAG_Update);
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef timerNVIC;
    timerNVIC.NVIC_IRQChannel = TIM7_IRQn;
    timerNVIC.NVIC_IRQChannelCmd = ENABLE;
    timerNVIC.NVIC_IRQChannelPreemptionPriority = 1;
    timerNVIC.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&timerNVIC);

    TIM_Cmd(TIM7, ENABLE);   // 上电即开始循迹(20ms一次)
}


void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {
        if (car_run && !straight_flag) {   // 循迹模式且运行中才由 TIM7 控制
            follow_line(&line_controller, sensor_data, 1);   // 先改回1, 用第3行传感器原始值确认极性
        }
        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}
