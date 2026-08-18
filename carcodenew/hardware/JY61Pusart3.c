#include "sys.h"

/* ====================================================== */
/*          USART1 驱动（用于 JY61P 陀螺仪）               */
/*          引脚：PA9(TX)  PA10(RX)                        */
/* ====================================================== */

uint8_t RxBuffer[11]; // 存放一帧数据
uint8_t count = 0;    // 计数器
volatile float Yaw;            // 解析出的航向角变量

/* 串口1初始化设置 */
/* 入口参数：波特率 */
void usart1_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 使能 GPIOA 和 USART1 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

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
    
	USART_InitStructure.USART_BaudRate = bound; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USART1, &USART_InitStructure);

	// 开启接收中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

	// 中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE); 
}

/* JY61P 数据接收中断 — 解析 0x55 0x53 角度包 */
void USART1_IRQHandler(void)                                 
{      
    uint8_t res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
    {  
        res = USART_ReceiveData(USART1);
        
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

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);                         
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
        USART_SendData(USART1, cmd[i]);
        // 等待发送完成 (TC位)
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
}

/**
 * @brief  JY61P 初始化
 * @note   JY61P 已配置好，直接以 115200 初始化串口
 */
void JY61P_Full_Init_Sequence(void) {
    usart1_init(115200); 
    delay_ms(100);
}
