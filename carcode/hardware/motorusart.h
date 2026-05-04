#ifndef __MOTORUSART_H
#define __MOTORUSART_H

#include "sys.h"

void usart2_init(uint32_t bound);
void vofa_send_vel(float v1,float v2);

#endif

