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

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出 Push-pull output
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入 Floating input
	
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


