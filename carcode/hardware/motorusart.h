#ifndef __MOTORUSART_H
#define __MOTORUSART_H

#include "stm32f10x.h"
#include <stdio.h>
void usart1_init(uint32_t bound);
void vofa_send_vel(float v1,float v2);

#endif

