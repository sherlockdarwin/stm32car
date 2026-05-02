#ifndef __TRACKUSART_H
#define __TRACKUSART_H

#include "sys.h"

#define AD0_PIN          GPIO_Pin_15
#define AD1_PIN          GPIO_Pin_14
#define AD2_PIN          GPIO_Pin_13
#define OUT_PIN          GPIO_Pin_11

#define SENSOR_AD0_WRITE(state)  GPIO_WriteBit(GPIOA, AD0_PIN, (state) ? 1 : 0)
#define SENSOR_AD1_WRITE(state)  GPIO_WriteBit(GPIOA, AD1_PIN, (state) ? 1 : 0)
#define SENSOR_AD2_WRITE(state)  GPIO_WriteBit(GPIOA, AD2_PIN, (state) ? 1 : 0)

#define SENSOR_OUT_READ()        GPIO_ReadInputDataBit(GPIOA, OUT_PIN)

void Sensor_Init(void);
void Sensor_Read_All(uint16_t* sensor_values);
uint16_t Sensor_Read_Single(uint8_t channel);

#endif

