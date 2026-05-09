#include "sys.h"
//TIM3  PA6 PA7
void Encoder_A_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;      

    //PA6 ch1, PA7 ch2 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);                           

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = MAX_VALUE;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              

   
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling ,TIM_ICPolarity_Falling);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6;              
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    //Reset counter
    TIM3->CNT = 0;

    TIM_Cmd(TIM3, ENABLE);
}



//TIM4  PB6 PB7
void Encoder_B_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;      

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                           

    TIM_DeInit(TIM4);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = MAX_VALUE;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              

   
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling ,TIM_ICPolarity_Falling);
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 6;              
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    //Reset counter
    TIM4->CNT = 0;

    TIM_Cmd(TIM4, ENABLE);
  
}


int Read_Encoder(uint8_t encoder)
{
   int Encoder_TIM;    
   switch(encoder)
	 {
		 case 1:  
				Encoder_TIM= (short)TIM3 -> CNT;  TIM3 -> CNT=0; break;	
		 case 2:  
				Encoder_TIM= (short)TIM4 -> CNT;  TIM4 -> CNT=0; break;	
		 default: 
			    Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

int Read_Position(uint8_t encoder)
{
   int Encoder_TIM;    
   switch(encoder)
	 {
		 case 1:  Encoder_TIM= (short)TIM3 -> CNT; break;	
		 case 2:  Encoder_TIM= (short)TIM4 -> CNT; break;	
		 default: Encoder_TIM=0;
	 }
		return Encoder_TIM;
}


