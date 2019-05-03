/*
 * tm1638.h
 *
 *  Created on: 2019Äê3ÔÂ21ÈÕ
 *      Author: lifim
 */

#ifndef TM1638_H_
#define TM1638_H_


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x_rcc.h"

#define DATA_COMMAND    0X40
#define DISP_COMMAND    0x80
#define ADDR_COMMAND    0XC0

void tm1638_init_gpio();
void tm1638_write_byte(unsigned char data);
unsigned char tm1638_read_byte(void);
void tm1638_write_com(unsigned char cmd);
unsigned char tm1638_read_key(void);
void tm1638_write_data(unsigned char add, unsigned char data);
void tm1638_write_led(unsigned char led_flag);
extern void tm1638_init(void);
extern void tm1638_tube_dip(uint16_t pos, uint16_t data);
extern unsigned char code_tab[];
#endif /* TM1638_H_ */
