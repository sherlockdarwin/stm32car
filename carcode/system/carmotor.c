#include "stm32f10x.h"                  // Device header


#define AIN1   GPIO_Pin_5
#define AIN2   GPIO_Pin_4
#define PWMA   GPIO_Pin_0

#define BIN1   GPIO_Pin_4                  //PB4
#define BIN2   GPIO_Pin_3                 //PB3  
#define PWMB   GPIO_Pin_1

#define STBY   GPIO_Pin_12

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//BIN PB3 PB4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB 
	//AIN PA4 PA5 STBY PA12
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能PA端口时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
    GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

}


void PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_initStructure;
	TIM_TimeBaseInitTypeDef TIM_timeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);	
    
	GPIO_initStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_initStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_initStructure);

	//定时器周期 t =( 72 / 7200 ) * 10^6 =  10khz    
	
	//配置时基
	TIM_timeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//不分频
	TIM_timeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_timeBaseInitStructure.TIM_Period = 7200-1;									//设置ARR值
	TIM_timeBaseInitStructure.TIM_Prescaler = 0;										//时钟预分频值
	TIM_TimeBaseInit(TIM2,&TIM_timeBaseInitStructure);
	
	//配置OC输出通道
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;					//采用PWM模式1输出波形
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//设置CH通道的有效电平
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		//设置CH通道的空闲状态的电平
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//使能CH通道
	TIM_OCInitStructure.TIM_Pulse = 0;									//设置TIM2的CCR值
	
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);                  
	
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);                                   

	//使能TIM的ARR和CRR，以及使能TIM定时器,开启pwm输出
	TIM_ARRPreloadConfig(TIM2,ENABLE);								    //预加载使能	
         
	TIM_Cmd(TIM2,ENABLE);
	
	TIM2->CCR1 = 0;  //PA0
	TIM2->CCR2 = 0;		//PA1
	
	GPIO_SetBits(GPIOA, STBY);
}


