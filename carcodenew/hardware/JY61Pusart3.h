#ifndef __JY61PUSART1_H
#define __JY61PUSART1_H

// --- 全局变量声明 ---
extern volatile float Yaw; 

// --- 函数声明 ---
void usart1_init(uint32_t bound);
void JY61P_Send_Cmd(uint8_t *cmd, uint8_t len);
void JY61P_Full_Init_Sequence(void);

#endif


