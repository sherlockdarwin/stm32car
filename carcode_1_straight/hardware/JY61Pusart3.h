#ifndef __JY61PUSART3_H
#define __JY61PUSART3_H


// --- 全局变量声明 ---
// 使用 extern 关键字，让其他文件（如 main.c）能访问解析出的角度
extern volatile float Yaw; 

// --- 函数声明 ---

/**
 * @brief USART3 初始化函数
 * @param bound: 波特率 (JY61P 建议设为 9600 或 115200)
 */
void usart3_init(uint32_t bound);

/**
 * @brief 串口3 DMA 发送配置
 * @param memory_addr: 待发送数据的内存起始地址
 * @param data_length: 发送字节长度
 */
void USART3_DMA_TX_Config(uint32_t memory_addr, uint16_t data_length);

/**
 * @brief 串口3 DMA 数据发送触发函数
 * @param SendBuff: 数据首地址
 * @param len: 发送长度
 */
void USART3_DMA_SEND_DATA(uint32_t SendBuff, uint16_t len);

/**
 * @brief 将数据打包并发送至 VOFA+ 上位机进行波形显示
 * @param v1: 通道1数据 (例如当前角度)
 * @param v2: 通道2数据 (例如目标角度)
 */
void usart3_vofa_send_vel(float v1, float v2); 
void JY61P_Send_Cmd(uint8_t *cmd, uint8_t len);
void JY61P_Full_Init_Sequence(void);//初始化jy61p的全套流程，包含波特率切换和性能配置
#endif

