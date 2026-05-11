#include "sys.h"

line_following line_controller;

// 巡线参数初始化 Initialize line following parameters
void line_following_init(line_following* controller)
{
    controller->kp = 270.0f;    // 比例系数 | proportional gain
    controller->ki = 0.5f;      // 积分系数 | integral gain
    controller->kd = 50.0f;     // 微分系数 | derivative gain

    controller->last_error = 0.0f;  // 上次偏差 | last error
    controller->integral = 0.0f;    // 积分累积 | integral accumulation

    controller->base_speed = 450;   // 基础速度 | base speed
    controller->max_speed = 800;    // 最大速度 | maximum speed

    controller->sensor_weights[0] = -5.0f;
    controller->sensor_weights[1] = -4.0f;
    controller->sensor_weights[2] = -2.0f;
    controller->sensor_weights[3] = -1.0f;
    controller->sensor_weights[4] = 1.0f;
    controller->sensor_weights[5] = 2.0f;
    controller->sensor_weights[6] = 4.0f;
    controller->sensor_weights[7] = 5.0f;

    controller->motor_locked = true;  // 上电默认锁定电机 | lock motors on power-up
}


bool check_sensors_safe(line_following* controller, uint16_t* sensor_values) {
// 上电安全锁检查，当传感器全亮或全灭时不启动小车，防止乱跑
    uint16_t first_value = sensor_values[0];

    for (int i = 1; i < 8; i++) {
        if (sensor_values[i] != first_value)return true;  // 传感器值不全相同，安全
    }

    return false;  // 传感器全亮或全灭，不安全
}

float calculate_error(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value)
{
//计算偏差值
    float weighted_sum = 0.0f;
    int active_sensors = 0;

    for (int i = 0; i < 8; i++) {
        if (sensor_values[i] == line_raw_value) {
            weighted_sum += controller->sensor_weights[i];
            active_sensors++;
        }
    }

    // 如果没有检测到线，返回上次偏差（丢线处理）
    // If no line is detected, return the last deviation (line loss handling)
    if (active_sensors == 0) {
        return controller->last_error;
    }

    // 计算加权平均偏差
    // Calculate weighted average deviation
    float error = weighted_sum / active_sensors;
    return error;
}

float pid_control(line_following* controller, float error)
{

    if (fabsf(error) < 0.6f) {
        error = 0.0f;
    }

    if ((controller->last_error > 0 && error < 0) ||(controller->last_error < 0 && error > 0)) {
        controller->integral = 0.0f;  // 偏差过零，清空积分/ Clear integral when deviation crosses zero
    }

    // 动态调整积分限幅 / Dynamically adjust integral limit
    float integral_limit;
    if (fabsf(error) > 3.0f) {          // 大偏差时 / Large deviation
        integral_limit = 80.0f;
    } else if (fabsf(error) > 1.5f) {   // 中等偏差时 / Medium deviation
        integral_limit = 50.0f;
    } else {                            // 小偏差时 / Small deviation
        integral_limit = 20.0f;
    }

    // 积分项，使用动态限幅 / Integral term with dynamic limit
    controller->integral += error;
    controller->integral = PWM_Limit(controller->integral, -integral_limit, integral_limit);

    // 微分项 / Derivative term
    float derivative = error - controller->last_error;

    // PID计算 / PID calculation
    float output = (controller->kp * error +
                   controller->ki * controller->integral +
                   controller->kd * derivative);

    // 更新上次偏差 / Update last error
    controller->last_error = error;

    return output;
}

void differential_speed_control(line_following* controller, float pid_output, uint16_t* left_speed, uint16_t* right_speed)
{

    float left = controller->base_speed + pid_output;
    float right = controller->base_speed - pid_output;

    // 速度限制 / Speed limit
    *left_speed = PWM_Limit(left, -controller->max_speed, controller->max_speed);
    *right_speed = PWM_Limit(right, -controller->max_speed, controller->max_speed);
}

void follow_line(line_following* controller, uint16_t* sensor_values, uint16_t line_raw_value)
{
//巡线主函数 Main line following function

    // 检查安全锁 / Check safety lock
    if (controller->motor_locked) {
        if (safe&&straight_flag) 
		{
			straight_flag = 0;
			count ++;
            controller->motor_locked = false;  // 解锁 / Unlock
			TIM_Cmd(TIM5, DISABLE);
			TIM_Cmd(TIM6, ENABLE);
			Beep_Sound();
			PA8_Flash();
        } 
		/*
		else {
            Set_Pwm(0, 0);// 确保电机停止 / Ensure motors are stopped
            return;
        }
		*/
    }

    // 计算偏差 / Calculate deviation
    float error = calculate_error(controller, sensor_values, line_raw_value);

    // PID控制计算 / PID control calculation
    float pid_output = pid_control(controller, error);

    // 差速控制 / Differential speed control
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
	
	//TIM_Cmd(TIM7, ENABLE);
}


void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		follow_line(&line_controller, sensor_data, 1);
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
		
	}
}



