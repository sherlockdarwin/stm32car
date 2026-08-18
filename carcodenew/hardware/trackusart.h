#ifndef __TRACKUSART_H
#define __TRACKUSART_H


#define AD0_PIN          GPIO_Pin_15   // PA15
#define AD1_PIN          GPIO_Pin_1    // PB1
#define AD2_PIN          GPIO_Pin_0    // PB0
#define OUT_PIN          GPIO_Pin_11   // PA11

#define SENSOR_AD0_WRITE(state)  GPIO_WriteBit(GPIOA, AD0_PIN, (state) ? Bit_SET : Bit_RESET)   // PA15
#define SENSOR_AD1_WRITE(state)  GPIO_WriteBit(GPIOB, AD1_PIN, (state) ? Bit_SET : Bit_RESET)   // PB1
#define SENSOR_AD2_WRITE(state)  GPIO_WriteBit(GPIOB, AD2_PIN, (state) ? Bit_SET : Bit_RESET)   // PB0

#define SENSOR_OUT_READ()        GPIO_ReadInputDataBit(GPIOA, OUT_PIN)

void Sensor_Init(void);
void Sensor_Read_All(uint16_t* sensor_values);
uint16_t Sensor_Read_Single(uint8_t channel);

#endif

