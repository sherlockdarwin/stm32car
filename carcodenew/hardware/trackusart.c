#include "sys.h"


//选择传感器通道 select sensor channel
static void select_channel(uint8_t channel)
{
    SENSOR_AD0_WRITE((channel >> 0) & 0x01);  // bit0 -> AD0
    SENSOR_AD1_WRITE((channel >> 1) & 0x01);  // bit1 -> AD1
    SENSOR_AD2_WRITE((channel >> 2) & 0x01);  // bit2 -> AD2
}


static uint16_t Read_OUT_value(void)
{
    return SENSOR_OUT_READ();
}

void Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 配置 AD0 (PA15) 为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 AD1 (PB1)、AD2 (PB0) 为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置 OUT (PA11) 为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}

void Sensor_Read_All(uint16_t* sensor_values)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        select_channel(i);
        delay_us(50);
        sensor_values[i] = Read_OUT_value();
    }
}

uint16_t Sensor_Read_Single(uint8_t channel)
{
    if (channel >= 8)
    {
        return 0; // 无效通道 // Invalid channel
    }
    select_channel(channel);
    delay_us(50);
    return Read_OUT_value();
}


