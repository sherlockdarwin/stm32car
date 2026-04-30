#ifndef __CARMOTOR_H
#define __CARMOTOR_H	

void Motor_Init(void);
void PWM_Init(void);
void Set_Pwm(int motor_left,int motor_right);
int PWM_Limit(int IN,int max,int min);
int myabs(int a);



#endif
