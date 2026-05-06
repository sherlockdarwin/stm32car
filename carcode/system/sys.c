#include "sys.h"              // Device header

#pragma import(__use_no_semihosting)
struct __FILE
{
	int a;
};
 
FILE __stdout;

void _sys_exit(int x)
{
	
}



void SWJ_Config(void)
{
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);	
}

void System_Init(void)
{


}


void systick_init(void)
{
	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 200))	        //1000 -> 1ms
	{ 
		/* Capture error */ 
		while (1);
	}
}


void delay_us(unsigned int n)
{
	unsigned char j;
	while(n--)
    for(j=0;j<10;j++);
}

void delay_ms(unsigned int n)
{
	while(n--)
    delay_us(1000);
}


