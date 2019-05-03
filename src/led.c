/*
 * led.c
 *
 *  Created on: 2019Äê3ÔÂ21ÈÕ
 *      Author: lifim
 */
#include "led.h"

int switch_time_green_to_red = 80;
int switch_time_red_to_green = 80;

void led_init_gpio()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef    line_gpio_cfg;
    line_gpio_cfg.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
    line_gpio_cfg.GPIO_Mode    =   GPIO_Mode_Out_OD;
    line_gpio_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &line_gpio_cfg );
    GREEN_OPEN();
    YELLOW_OPEN();
    RED_STOP();
}

void led_change_g_to_r_switch_time(int change_time)
{
    switch_time_green_to_red += change_time;
}

void led_change_r_to_g_switch_time(int change_time)
{
    switch_time_red_to_green += change_time;
}

