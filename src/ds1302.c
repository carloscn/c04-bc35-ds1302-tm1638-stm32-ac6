/*
 * ds1302.c
 *
 *  Created on: 2018年4月23日
 *      Author: weihaochen
 */
#include "ds1302.h"

//初始时间定义

BYTE    time_buf[8] = {0x20,0x10,0x09,0x14,0x23,0x59,0x50,0x02};//初始时间
BYTE    readtime[14];//当前时间
uint16  sec_buf=0;  //秒缓存
bool    sec_flag=0; //秒标志位

DS1302* ds1302_new_dev(void)
{
    DS1302* self = (DS1302*) malloc(sizeof(DS1302)*2);
    // step1 : set connect with function pointer;
    self->init          =   &ds1302_init;
    self->pin_set       =   &ds1302_pin_set;
    self->reset         =   &ds1302_reset;
    self->write_byte    =   &ds1302_write_byte;
    self->read_byte     =   &ds1302_read_byte;
    self->read_time     =   &ds1302_read_time;
    self->write_time    =   &ds1302_write_time;

    // step2 : init hardware para;
    self->hw.rst_pin    =   GPIO_Pin_0;
    self->hw.rst_port   =   GPIOB;
    self->hw.sck_pin    =   GPIO_Pin_2;
    self->hw.sck_port   =   GPIOB;
    self->hw.sda_pin    =   GPIO_Pin_1;
    self->hw.sda_port   =   GPIOB;

    self->init( self );

    return self;
}

void    ds1302_init( DS1302 *self )
{
    self->pin_set( self );

    DS1302_RST_LOW(self);
    DS1302_SCLK_LOW(self);
}

void    ds1302_pin_set( DS1302 *self )
{

    GPIO_InitTypeDef io_cfg;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

    io_cfg.GPIO_Pin     =  self->hw.sck_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_Out_PP;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.sck_port, &io_cfg );

    io_cfg.GPIO_Pin     =  self->hw.rst_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_Out_PP;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.rst_port, &io_cfg );

    io_cfg.GPIO_Pin     =  self->hw.sda_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_Out_PP;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.sda_port, &io_cfg );

}


void    ds1302_reset( DS1302 *self )
{
    DS1302_RST_LOW(self);
}


void    ds1302_write_byte(DS1302 *self, uint8 addr, uint8 data )
{
    unsigned char   i;
    DS1302_RST_HIGH(self);

    addr    =   addr    &   0xFE;
    // Write address;
    for( i = 0; i < 8; i++ ) {
        if( addr & 0x01 ) {
            DS1302_SDA_HIGH(self);
        }else {
            DS1302_SDA_LOW(self);
        }
        DS1302_SCLK_HIGH(self);
        DS1302_SCLK_LOW(self);
        addr = addr >> 1;
    }
    // Write data;
    for( i = 0; i < 8; i++ ) {
        if( data & 0x01 ) {
            DS1302_SDA_HIGH(self);
        }else {
            DS1302_SDA_LOW(self);
        }
        DS1302_SCLK_HIGH(self);
        DS1302_SCLK_LOW(self);
        data = data >> 1;
    }
    DS1302_RST_LOW(self);

}

BYTE    ds1302_read_byte( DS1302 *self, uint8 addr )
{
    BYTE    i,temp;

    DS1302_RST_HIGH(self);
    addr = addr | 0x01;
    for( i = 0; i < 8; i++ ) {
        if( addr & 0x01 ) {
            DS1302_SDA_HIGH(self);
        }else {
            DS1302_SDA_LOW(self);
        }
        DS1302_SCLK_HIGH(self);
        DS1302_SCLK_LOW(self);
        addr >>= 1;
    }
    ds1302_hw_set_in( self );
    for( i = 0; i < 8; i++ ) {
        temp  >>= 1;
        if( DS1302_SDA_READ(self) ) {
            temp    |=  0x80;
        }else {
            temp    &=  0x7F;
        }
        DS1302_SCLK_HIGH(self);
        DS1302_SCLK_LOW(self);
    }
    ds1302_hw_set_out( self );
    DS1302_RST_LOW(self);
    return temp;

}
void    ds1302_write_time( DS1302 *self )
{


    self->write_byte( self,  ds1302_control_add,0x00);         //关闭写保护
    self->write_byte( self,  ds1302_sec_add,0x80);             //暂停时钟
    self->write_byte( self,  ds1302_year_add,self->time.year);     //年
    self->write_byte( self,  ds1302_month_add,self->time.month);    //月
    self->write_byte( self,  ds1302_date_add,self->time.day);     //日
    self->write_byte( self,  ds1302_hr_add,self->time.hour);       //时
    self->write_byte( self,  ds1302_min_add,self->time.min);      //分
    self->write_byte( self,  ds1302_sec_add,self->time.sec);      //秒
    self->write_byte( self,  ds1302_day_add,self->time.week);      //周
    self->write_byte( self,  ds1302_control_add,0x80);         //打开写保护
}

void    ds1302_read_time( DS1302 *self )
{

    time_buf[1] = self->read_byte( self, ds1302_year_add);      //年

    time_buf[2] = self->read_byte( self, ds1302_month_add);     //月

    time_buf[3] = self->read_byte( self, ds1302_date_add);      //日

    time_buf[4] = self->read_byte( self, ds1302_hr_add);        //时

    time_buf[5] = self->read_byte( self, ds1302_min_add);       //分

    time_buf[6] = (self->read_byte( self, ds1302_sec_add))&0x7f;//秒，屏蔽秒的第7位，避免超出59

    time_buf[7] = self->read_byte( self, ds1302_day_add);       //周

    // Deal year;
    self->time.year     =   (time_buf[0] >> 4)  * 1000 + \
                            (time_buf[0] & 0xF) * 100 + \
                            (time_buf[1] >> 4)  * 10 + \
                            (time_buf[1] & 0xF) * 1;
    itoa( self->time.year, self->time.year_str, 4 );


    self->time.month    =   (time_buf[2] >> 4)  * 10 + \
                            (time_buf[2] & 0xF) *1;
    itoa( self->time.month, self->time.month_str,2 );


    self->time.day    =     (time_buf[3] >> 4)  * 10 + \
                            (time_buf[3] & 0xF) *1;
    itoa( self->time.day, self->time.day_str, 2 );


    self->time.hour    =    (time_buf[4] >> 4)  * 10 + \
                            (time_buf[4] & 0xF) *1;
    itoa( self->time.hour, self->time.hour_str, 2 );

    self->time.min    =     (time_buf[5] >> 4)  * 10 + \
                            (time_buf[5] & 0xF) *1;
    itoa( self->time.min, self->time.min_str, 2 );

    self->time.sec    =     (time_buf[6] >> 4)  * 10 + \
                            (time_buf[6] & 0xF) *1;
    itoa( self->time.sec, self->time.sec_str, 2 );


    self->time.week   =     (time_buf[7] >> 4)  * 10 + \
                            (time_buf[7] & 0xF) *1;
    itoa( self->time.week, self->time.week_str, 2 );


    sprintf( self->time.all_time,    \
             "time: %d-%d-%d %d:%d:%d week:%d \n" , \
             self->time.year,   \
             self->time.month,  \
             self->time.day,    \
             self->time.hour,   \
             self->time.min,    \
             self->time.sec,    \
             self->time.week   );

}
/*
* --------------------------------
* 1. SCK   -    PC13
* 2. DAT   -    PE06
* 3. RST   -    PE05
* */


void    ds1302_hw_set_in( DS1302 *self )
{
    GPIO_InitTypeDef io_cfg;
    io_cfg.GPIO_Pin     =  self->hw.sda_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_IN_FLOATING;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.sda_port, &io_cfg );
}

void    ds1302_hw_set_out( DS1302 *self )
{
    GPIO_InitTypeDef io_cfg;
    io_cfg.GPIO_Pin     =  self->hw.sda_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_Out_PP;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.sda_port, &io_cfg );
}
