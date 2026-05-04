#ifndef __CARMOTOR_H
#define __CARMOTOR_H	

#define AIN1   GPIO_Pin_5
#define AIN2   GPIO_Pin_4
#define PWMA   GPIO_Pin_0

#define BIN1   GPIO_Pin_4                  //PB4
#define BIN2   GPIO_Pin_3                 //PB3  
#define PWMB   GPIO_Pin_1

#define STBY   GPIO_Pin_12

void Motor_Init(void);
void PWM_Init(void);
void Set_Pwm(int motor_left,int motor_right);
int PWM_Limit(int IN,int max,int min);
int myabs(int a);



#endif
