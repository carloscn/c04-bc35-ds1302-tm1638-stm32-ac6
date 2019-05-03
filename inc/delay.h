#ifndef __DELAY_H
#define __DELAY_H 			   

#include "stdint.h"
#define SYSTEM_SUPPORT_OS       0       //定义系统文件夹是否支持UCOS

extern void delay_init(void);
extern void delay_ms(uint16_t nms);
extern void delay_us(uint32_t nus);
#define          DELAY_US(num)            delay_us(num)
#define          DELAY_MS(num)            delay_ms(num)
#endif





























