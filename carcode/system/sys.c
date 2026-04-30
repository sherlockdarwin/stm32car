#include "sys.h"                  // Device header


void SWJ_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	
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


