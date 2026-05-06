#include "sys.h"

uint8_t data_to_send[64];   

int fputc(int ch,FILE*f)
{
    USART1->SR; 
    USART_SendData(USART1, (unsigned char) ch);
    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
    return(ch);
} 
/* 串口1初始化设置 */
/* 入口参数：波特率 */
void usart1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//USART1 Tx(PA.9) 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	//USART1 Rx(PA.10) 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); 

	USART_Cmd(USART1, ENABLE); 


	NVIC_InitTypeDef NVIC_InitStructure;    
   
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;               //通道设置为串口1中断    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;       //中断占先等级    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;              //中断响应优先级    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //打开中断    
    NVIC_Init(&NVIC_InitStructure);   
}
/**************************************************************************
串口DMA通道配置                
DMA_CHx:         DMA传输通道x   
peripheral_addr: 外设地址       
memory_addr:     内存地址        
data_length:     传输的数据长度  
**************************************************************************/
void USARTx_DMA_TX_Config(DMA_Channel_TypeDef* DMA_CHx,u32 peripheral_addr,u32 memory_addr,u16 data_length)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	                        //时钟使能                                                                              
    DMA_DeInit(DMA_CHx);                                                        //复位
                                                                                
    DMA_InitStructure.DMA_PeripheralBaseAddr = peripheral_addr;                 //外设地址     
    DMA_InitStructure.DMA_MemoryBaseAddr =memory_addr;                          //内存地址  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                          //外设作为传输的目的地
    DMA_InitStructure.DMA_BufferSize = data_length;                             //数据缓存大小                       
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //内存地址自增   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //外设数据宽度8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //内存数据宽度8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                //无内存到内存传输                    
    DMA_Init(DMA_CHx, &DMA_InitStructure);                                 
}
/**************************************************************************
串口DMA数据发送 
**************************************************************************/
void USARTx_DMA_SEND_DATA(u32 SendBuff,u16 len) 
{
	USARTx_DMA_TX_Config(DMA1_Channel4,(u32)&USART1->DR,(u32)SendBuff,len);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);                              //使能串口DMA发送
	DMA_Cmd(DMA1_Channel4, ENABLE);                                             //使能DMA传输
}


/* 向VOFA+上位机速度 */
void vofa_send_vel(float v1,float v2)
{
	unsigned char _cnt = 0;
	data_to_send[_cnt++] = BYTE0(v1);
	data_to_send[_cnt++] = BYTE1(v1);
	data_to_send[_cnt++] = BYTE2(v1);
	data_to_send[_cnt++] = BYTE3(v1);
	data_to_send[_cnt++] = BYTE0(v2);
	data_to_send[_cnt++] = BYTE1(v2);
	data_to_send[_cnt++] = BYTE2(v2);
	data_to_send[_cnt++] = BYTE3(v2);
	
	data_to_send[_cnt++]=00;
	data_to_send[_cnt++]=00;
	data_to_send[_cnt++]=0X80;
	data_to_send[_cnt++]=0X7F;
	USARTx_DMA_SEND_DATA((u32)(data_to_send),_cnt); //发送           
} 
uint8_t ch;

void USART1_IRQHandler(void)                                 
{      
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
    {  
        ch= USART_ReceiveData(USART1);

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);                             //清除空闲中断标志         
    }   
}   



