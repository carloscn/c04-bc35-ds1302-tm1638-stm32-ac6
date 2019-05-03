/*
 * led.c
 *
 *  Created on: 2019Äê3ÔÂ21ÈÕ
 *      Author: lifim
 */

#ifndef LED_H_
#define LED_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "stm32f10x_rcc.h"

#define             RED_STOP()                     ( GPIO_SetBits(GPIOA, GPIO_Pin_7) )
#define             RED_OPEN()                      ( GPIO_ResetBits(GPIOA, GPIO_Pin_7) )
#define             GREEN_STOP()                      ( GPIO_SetBits(GPIOA, GPIO_Pin_5) )
#define             GREEN_OPEN()                       ( GPIO_ResetBits(GPIOA, GPIO_Pin_5) )
#define             YELLOW_STOP()                     ( GPIO_SetBits(GPIOA, GPIO_Pin_6) )
#define             YELLOW_OPEN()                      ( GPIO_ResetBits(GPIOA, GPIO_Pin_6) )

#endif /* LED_C_ */
