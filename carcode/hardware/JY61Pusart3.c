#include "sys.h"
uint8_t data_to_send[64];                  //发送数据缓存

#pragma import(__use_no_semihosting)
struct __FILE
{
	int a;
};
 
FILE __stdout;
void _sys_exit(int x)
{
	
}
int fputc(int ch,FILE *f)
{
    USART3->SR; 
    USART_SendData(USART3, (unsigned char) ch);
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
    return(ch);
} 
/* 串口3初始化设置 */
/* 入口参数：波特率 */
void usart3_init(uint32_t bound){

GPIO_InitTypeDef GPIO_InitStructure;

USART_InitTypeDef USART_InitStructure;

NVIC_InitTypeDef NVIC_InitStructure;



// 2. 配置 PB10 (USART3_TX)

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;

GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽

GPIO_Init(GPIOB, &GPIO_InitStructure);


// 3. 配置 PB11 (USART3_RX)

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入

GPIO_Init(GPIOB, &GPIO_InitStructure);


// 4. 串口参数设置

USART_InitStructure.USART_BaudRate = bound;

USART_InitStructure.USART_WordLength = USART_WordLength_8b;

USART_InitStructure.USART_StopBits = USART_StopBits_1;

USART_InitStructure.USART_Parity = USART_Parity_No;

USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

USART_Init(USART3, &USART_InitStructure);



// 5. 开启接收中断

USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);



// 6. 中断优先级设置 (NVIC)

NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;

NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 优先级根据你工程调整

NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

NVIC_Init(&NVIC_InitStructure);



USART_Cmd(USART3, ENABLE);

}
/**************************************************************************
串口DMA通道配置                
DMA_CHx:         DMA传输通道x   
peripheral_addr: 外设地址       
memory_addr:     内存地址        
data_length:     传输的数据长度  
**************************************************************************/
// 1. DMA 配置函数：USART3_TX 固定对应 DMA1_Channel2
void USART3_DMA_TX_Config(u32 memory_addr, u16 data_length)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel2); // 必须是 Channel2

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR; // 指向串口3数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = memory_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = data_length;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);
}

// 2. 发送函数
void USART3_DMA_SEND_DATA(u32 SendBuff, u16 len) 
{
    // 先配置，再使能
    USART3_DMA_TX_Config(SendBuff, len);
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel2, ENABLE);
}

/* 将大于一个字节的数据拆分成多个字节发送 */
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp)    ) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

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
	USART3_DMA_SEND_DATA((u32)(data_to_send),_cnt); //发送           
} 
uint8_t RxBuffer[11]; // 存放一帧数据
uint8_t count = 0;    // 计数器
volatile float Yaw;            // 解析出的航向角变量

void USART3_IRQHandler(void)
{
    uint8_t res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        res = USART_ReceiveData(USART3);
        
        // 状态机解析：JY61P 角度包开头是 0x55 0x53
        if(count == 0 && res == 0x55) count = 1;
        else if(count == 1 && res == 0x53) count = 2;
        else if(count >= 2 && count < 11)
        {
            RxBuffer[count++] = res;
            if(count == 11)
            {
                // 解析角度数据：第7,8字节是航向角
                short temp = (short)((RxBuffer[7] << 8) | RxBuffer[6]);
                Yaw = (float)temp / 32768.0f * 180.0f;
                count = 0; // 清零，等待下一包
            }
        }
        else count = 0; // 数据不对，重置计数
        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}
/* ====================================================== */
/*          JY61P 专用初始化与指令发送逻辑                */
/* ====================================================== */

// 指令包定义
uint8_t JY_UNLOCK[]    = {0xFF, 0xAA, 0x69, 0x88, 0xB5}; // 解锁
uint8_t JY_SAVE[]      = {0xFF, 0xAA, 0x00, 0x00, 0x00}; // 保存
uint8_t JY_BAUD_1152[] = {0xFF, 0xAA, 0x04, 0x06, 0x00}; // 设为 115200
uint8_t JY_RATE_100HZ[]= {0xFF, 0xAA, 0x03, 0x09, 0x00}; // 设为 100Hz
uint8_t JY_6AXIS[]     = {0xFF, 0xAA, 0x24, 0x01, 0x00}; // 6轴模式
uint8_t JY_CALI_ACC[]  = {0xFF, 0xAA, 0x01, 0x01, 0x00}; // 静态校准

/**
 * @brief  JY61P 专用指令发送函数
 * @note   这里不使用 DMA，因为初始化指令很短，直接用阻塞发送更可靠
 */
void JY61P_Send_Cmd(uint8_t *cmd, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        USART_SendData(USART3, cmd[i]);
        // 等待发送完成 (TC位)
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    }
}

/**
 * @brief  JY61P 全套配置流程
 * @note   Leader提示：此函数在 main 中调用，执行时请保持小车静止
 */
void JY61P_Full_Init_Sequence(void) {
    // 1. 尝试以默认 9600 波特率握手
    usart3_init(9600); 
    delay_ms(500); 

    // 2. 解锁并尝试修改波特率到 115200 (对应微信图片逻辑)
    JY61P_Send_Cmd(JY_UNLOCK, 5);     // 解锁
    delay_ms(200);                    
    JY61P_Send_Cmd(JY_BAUD_1152, 5);  // 改波特率指令
    delay_ms(200);

    // 3. 核心：将 STM32 串口也切到 115200
    usart3_init(115200); 
    delay_ms(100);

    // 4. 在新波特率下重新解锁并配置性能参数
    JY61P_Send_Cmd(JY_UNLOCK, 5);     // 再次解锁
    delay_ms(200);
    
    JY61P_Send_Cmd(JY_RATE_100HZ, 5); // 设置为100Hz回传
    delay_ms(100);
    
    JY61P_Send_Cmd(JY_6AXIS, 5);      // 切换为6轴算法
    delay_ms(100);

    // 5. 静态校准指令
    // 发送后，传感器会采集当前加速度计数据作为“平地”参考
    JY61P_Send_Cmd(JY_CALI_ACC, 5); 
    delay_ms(100);

    // 6. 保存配置并退出
    JY61P_Send_Cmd(JY_SAVE, 5);       
    delay_ms(500); // 留出时间写入Flash
}

