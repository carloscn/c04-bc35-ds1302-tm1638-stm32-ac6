/*
 * tm1638.c
 *
 *  Created on: 2019年3月21日
 *      Author: lifim
 */

#include "tm1638.h"
// STB - PB7 / CLK - PB6 / DIO - PB5

#define             DIO_HIGH()                     ( GPIO_SetBits(GPIOB, GPIO_Pin_5) )
#define             DIO_LOW()                      ( GPIO_ResetBits(GPIOB, GPIO_Pin_5) )
#define             DIO_DATA()                     ( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) )
#define             STB_HIGH()                     ( GPIO_SetBits(GPIOB, GPIO_Pin_7) )
#define             STB_LOW()                      ( GPIO_ResetBits(GPIOB, GPIO_Pin_7) )
#define             CLK_HIGH()                     ( GPIO_SetBits(GPIOB, GPIO_Pin_6) )
#define             CLK_LOW()                      ( GPIO_ResetBits(GPIOB, GPIO_Pin_6) )


unsigned char code_tab[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
                           0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

void tm1638_init_gpio()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef    line_gpio_cfg;
    line_gpio_cfg.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    line_gpio_cfg.GPIO_Mode    =   GPIO_Mode_Out_PP;
    line_gpio_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &line_gpio_cfg );
}

void tm1638_write_byte(unsigned char data)
{
    unsigned char i;
    for( i=0; i<8; i++ ) {
        CLK_LOW();
        if(data & 0x01)
            DIO_HIGH();
        else
            DIO_LOW();
        data >>= 1;
        CLK_HIGH();
    }
}

unsigned char tm1638_read_byte(void) {

    unsigned char i;
    unsigned char temp=0;
    DIO_HIGH();
    for( i=0; i<8; i++ ) {
        temp>>=1;
        CLK_LOW();
        if( DIO_DATA() )
            temp|=0x80;
        CLK_HIGH();
    }
    return temp;
}

void tm1638_write_com(unsigned char cmd)
{
    STB_LOW();
    tm1638_write_byte(cmd);
    STB_HIGH();
}

unsigned char tm1638_read_key(void)
{
    unsigned char c[4],i,key_value=0;
    STB_LOW();
    tm1638_write_byte(0x42);                //读键扫数据 命令
    for(i=0;i<4;i++)
        c[i]=tm1638_read_byte();
    STB_HIGH();                           //4个字节数据合成一个字节
    for(i=0;i<4;i++)
        key_value|=c[i]<<i;
    for(i=0;i<8;i++)
        if((0x01<<i)==key_value)
            break;
    return i;
}

void tm1638_write_data(unsigned char add, unsigned char data)
{
    tm1638_write_com(0x44);
    STB_LOW();
    tm1638_write_byte(0xc0|add);
    tm1638_write_byte(data);
    STB_HIGH();
}

void tm1638_write_led(unsigned char led_flag)                   //控制全部LED函数，LED_flag表示各个LED状态
{
    unsigned char i;
    for(i=0;i<8;i++) {
        if(led_flag&(1<<i))
            tm1638_write_data(2*i+1,1);
        else
            tm1638_write_data(2*i+1,0);
    }
}

void tm1638_init(void)
{
    unsigned char i;
    tm1638_init_gpio();
    tm1638_write_com(0x8b);       //亮度 (0x88-0x8f)8级亮度可调
    tm1638_write_com(0x40);       //采用地址自动加1
    STB_LOW();              //
    tm1638_write_byte(0xc0);    //设置起始地址
    for(i=0;i<16;i++)      //传送16个字节的数据
        tm1638_write_byte(0x00);
    STB_HIGH();
}
extern unsigned char num[8];
void tm1638_tube_dip(uint16_t pos, uint16_t data)
{
    tm1638_write_data(pos*2, code_tab[data]);
    tm1638_write_led(1<<data);
}
