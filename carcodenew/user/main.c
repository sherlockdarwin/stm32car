#include "sys.h"

uint16_t sensor_data[8];
volatile int16_t target_yaw;
int16_t now_yaw;
int16_t init_yaw;   // 初始航向(A→B方向), 出弧后算反方向用
float get_Lenconder;   //左编码器B
float get_Renconder;   //右编码器A
float set_encoder = 200;
volatile int16_t left_pwm, right_pwm;
volatile bool safe = false;
volatile bool straight_flag = 0;   // 0=循迹, 1=盲走(陀螺仪走直线)
volatile bool car_run = false;
volatile uint8_t waypoint_count = 0;

uint8_t current_task = 1;   // 当前选择的任务 1/2/3/4


/* 声光提示: 蜂鸣+LED亮, TIM6 1秒后自动关闭 */
static void sound_light(void)
{
	Beep_Sound();
	PA8_Flash();
	TIM_Cmd(TIM6, ENABLE);
}

/* 停车 */
static void car_stop(void)
{
	car_run = false;
	straight_flag = 0;
	left_pwm = 0;
	right_pwm = 0;
	TIM_Cmd(TIM5, DISABLE);
	sound_light();
}

/* 开始运行 */
static void car_start(void)
{
	car_run = true;
	straight_flag = 1;      // 从盲走开始
	init_yaw = Yaw;         // 记录初始航向(A→B方向)
	target_yaw = Yaw;       // A→B 走直线
	waypoint_count = 0;
	straight_controller.last_error = 0.0f;   // 清PID状态, 消除微分踢
	straight_controller.integral = 0.0f;
	TIM_Cmd(TIM5, ENABLE);  // 使能走直线控制(20ms一次)
	Timer_Start();          // 计时清零 + 声光
	TIM_Cmd(TIM6, ENABLE);  // 1秒后自动关声光
}

/* 原始角度(±32768=±180°)转度(整数) */
static int16_t yaw_deg(int16_t yaw)
{
	return (int16_t)((int32_t)yaw * 180 / 32768);
}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SWJ_Config();
	systick_init();

	PWM_Init();
	Motor_Init();
	System_Init();
	OLED_ShowString(1, 1, "TASK:1");

	JY61P_Full_Init_Sequence();                    // 陀螺仪初始化(USART1)
	Yaw_Straight_Init(&straight_controller, STRAIGHT_BASE_SPEED, STRAIGHT_MAX_SPEED);
	target_yaw = Yaw;
	line_following_init(&line_controller);

	while (1)
	{
		uint8_t key = Key_GetNum();

		// 按键: PB10=开始/停止(键1), PB11=选择任务(键2)
		if (key == 1)
		{
			if (car_run) car_stop();
			else         car_start();
		}
		else if (key == 2)
		{
			if (!car_run)   // 只有未运行时才能选任务
			{
				current_task++;
				if (current_task > 4) current_task = 1;
				OLED_ShowString(1, 1, "TASK:");
				OLED_ShowNum(1, 6, current_task, 1);
			}
		}

		// 读传感器、判线(带去抖)
		get_Renconder = Read_Encoder(ENCODER_A);
		get_Lenconder = Read_Encoder(ENCODER_B);
		Sensor_Read_All(sensor_data);
		safe = check_sensors_safe(&line_controller, sensor_data);

		// 运行状态机
		if (car_run)
		{
			if (current_task == 1)
			{
				// 任务1: A出发盲走 → 检测到黑线(B点) → 停车
				if (safe)
				{
					car_stop();
				}
			}
			else if (current_task == 2)
			{
				// 任务2: 盲走 ↔ 循迹, 回线(检测不到黑线)2次后停车
				if (straight_flag == 1)     // 盲走
				{
					if (safe)               // 检测到黑线 → 循迹
					{
						straight_flag = 0;
						sound_light();
					}
				}
				else                        // 循迹
				{
					if (!safe)              // 检测不到黑线 → 盲走
					{
						straight_flag = 1;
						target_yaw = Yaw;   // 锁车当前朝向(切向角度), 不依赖陀螺仪测准180°
						straight_controller.last_error = 0.0f;   // 清PID状态, 消除微分踢
						straight_controller.integral = 0.0f;
						waypoint_count++;
						sound_light();
						if (waypoint_count >= 2)
						{
							car_stop();
						}
					}
				}
			}
			// 任务3/4 待实现
		}

		// OLED 显示
		if (car_run)
		{
			volight();   // 第2行: 时间 MM:SS
			if (straight_flag == 1)     // 盲走: 显示当前角/目标角
			{
				OLED_ShowString(1, 1, "C:");
				OLED_ShowSignedNum(1, 3, yaw_deg((int16_t)Yaw), 4);
				OLED_ShowString(1, 9, "T:");
				OLED_ShowSignedNum(1, 11, yaw_deg(target_yaw), 4);
			}
			else                        // 循迹: 显示左右轮速
			{
				OLED_ShowString(1, 1, "L");
				OLED_ShowSignedNum(1, 2, left_pwm, 4);
				OLED_ShowString(1, 8, "R");
				OLED_ShowSignedNum(1, 9, right_pwm, 4);
			}
		}

		Set_Pwm(left_pwm, right_pwm);
	}
}
