#include "sys.h"

uint8_t KeyNum = 0;

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// 上拉输入: 按键一端接GND, 按下=低电平(0)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//PB10、PB11 上拉输入
}

/**
  * 函    数：按键获取键码(单次触发)
  * 参    数：无
  * 返 回 值：1=PB10(开始/停止), 2=PB11(选择任务), 0=无按键
  */
uint8_t Key_GetNum(void)
{
	uint8_t ret = 0;

	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)		//PB10 按下
	{
		delay_ms(10);											//消抖
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)		//确认按下
		{
			ret = 1;
			while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0);	//等松手
			delay_ms(10);
		}
	}

	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)		//PB11 按下
	{
		delay_ms(10);
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			ret = 2;
			while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
			delay_ms(10);
		}
	}

	return ret;
}
