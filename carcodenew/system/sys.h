#ifndef __SYS_H
#define __SYS_H	  

#include "stm32f10x.h"

#include <string.h> 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

typedef unsigned char bool;
#define true  1
#define false 0


#include "carmotor.h"     //电机基础设置。设置速度等等
#include "trackusart.h"   //将灰度传感器数据读入数组sensor_data[8]
#include "track.h"        //循迹模块
#include "Xround.h"       //走圆弧模块(TIM1)
#include "straight.h"     //直走模块(TIM5)
#include "encoder.h"      //测量两个电机速度
#include "JY61Pusart1.h"  //读取陀螺仪数据，获取航向角Yaw。用的usart1
#include "Key.h"          //读取按键状态。用的GPIOB的PB10/PB11
#include "OLED.h"         //OLED显示模块
#include "LED.h"          //LED模块，用于显示当前状态
#include "Timer.h"        //定时器模块(TIM6计时/声光)
#include "volight.h"


extern uint8_t KeyNum;
extern volatile uint8_t waypoint_count;    // 经过的路点/回线次数

extern uint16_t sensor_data[8];

extern volatile int16_t Yaw;          // 解析出的航向角(原始值)
extern volatile int16_t target_yaw;
extern int16_t now_yaw;

extern volatile int16_t left_pwm, right_pwm;
extern volatile float dbg_line_error;   // 调试: 循迹偏差

extern line_following straight_controller;
extern line_following line_controller;
extern volatile bool safe;
extern volatile bool straight_flag;
extern volatile bool car_run;

extern float get_Lenconder;//左编码器B
extern float get_Renconder;//右编码器A
extern float set_encoder;    //初始化设定值

extern uint16_t Sec;			//定义在定时器中断里自增的变量
extern uint16_t Min;
extern uint16_t Timer_State;
extern uint16_t cnt;

/* 将大于一个字节的数据拆分成多个字节发送 */
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)   
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)   
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n) 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  


void SWJ_Config(void);
void System_Init(void);
void systick_init(void);
void delay_us(unsigned int n);
void delay_ms(unsigned int n);

#endif

