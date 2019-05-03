/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "project.h"
#include "stm32f10x.h"

void RCC_Configuration(void);
// TM1638  b GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
// 实时时钟： b 0 1 2
#define     SET_TIME_MODE           0

/*TCP服务器配置 */
#define NBIOT_SERVICE_IP        "47.100.224.170"
#define NBIOT_SERVICE_TCP_PORT  (8888)
#define NBIOT_SERVICE_UDP_PORT  (9999)

/*COAP服务器配置 */
#define NBIOT_COAP_SERVICE_IP "49.4.85.232"
const char  hexStrTable[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46 };
UART* uart1;
UART* uart2;
UART* uart3;
DS1302* ds1302;
unsigned char num[8];       //各个数码管显示的值

struct time_t {
    unsigned char hours;
    unsigned char min;
    unsigned char sec;
};
uint16_t irq_time_sec = 0;
uint16_t irq_time_k = 0;
//--- control mode
#define     CTRL_MODE_HIGH   0xdf
#define     CTRL_MODE_LOW  0xcf
uint16_t ctrl_mode;
//---
uint16_t normal_mode = 1;
uint16_t high_peak_mode = 0;
bool cloud_ctrl = 0;
bool one_time = false;
void send_info_to_cloud(unsigned char* state, uint16 time);
int main(void)
{
    uint16_t i;

    SystemInit();
    RCC_Configuration();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    led_init_gpio();
    tm1638_init();

    uart1 = uart_new_dev(UART_ID_1, 115200);
    uart2 = uart_new_dev(UART_ID_2, 9600);
    uart3 = uart_new_dev(UART_ID_3, 9600);
    ds1302 = ds1302_new_dev();
    uart1->write_string(uart1, "*Set up real time clock.\n", -1);
#if SET_TIME_MODE
    ds1302->time.year     =   0x19;
    ds1302->time.month    =   0x03;
    ds1302->time.day      =   0x25;
    ds1302->time.hour     =   0x14;
    ds1302->time.min      =   0x40;
    ds1302->time.sec      =   0x00;
    ds1302->time.week     =   0x05;
    ds1302->write_time( ds1302 );
#endif
    ds1302->read_time(ds1302);
    uart1->write_string(uart1, ds1302->time.all_time, -1);
//    printf("%d-%d-%d %dh:%d:%d\n",
//           ds1302->time.year,\
//           ds1302->time.month, \
//           ds1302->time.day, \
//           ds1302->time.hour, \
//           ds1302->time.min, \
//           ds1302->time.sec );

    //NBBCxx_init(uart2);

    //Init BC35
    BC35_Init();
    BC35_ConUDP();
    uart1->write_string(uart1, "*Init tube to zero.\n", -1);
    Delay(200);
    for(i=0;i<8;i++)
        tm1638_write_data(i<<1,code_tab[0]);                   //初始化寄存
    //send_info_to_cloud("100", 64);
//    BC35_Senddata("4","AABBCCDD");
    //BC35_Senddata("4", "AABBCCDD");
    uart1->write_string(uart1, "*Start time loop.\n", -1);
    TIM_SetInterval(1,1000000000);
//    BC35_Senddata(4, "AABBCCDD");
    for(;;) {
        BC35_RECData();
        // 红绿灯控制流程
        if (normal_mode == 1) {
            led_do_normal_mode(irq_time_sec);
        }else{
            led_do_high_peak_mode(irq_time_sec);
        }
        i = tm1638_read_key();                          //读按键值

        if(i < 8) {
            while( tm1638_read_key() == i );              //等待按键释放
            if (i == 0) {
                led_change_g_to_r_switch_time(10);
            }else if (i == 1) {
                led_change_g_to_r_switch_time(-10);
            }else if (i == 2) {
                led_change_r_to_g_switch_time(10);
            }else if (i == 3) {
                led_change_r_to_g_switch_time(-10);
            }else if (i == 4) {
                normal_mode = 0;
                high_peak_mode = 0;
                RED_OPEN();
                YELLOW_STOP();
                GREEN_STOP();
            // 按键控制高峰期模式------------------>
            }else if (i == 5) {
                normal_mode = 0;
                high_peak_mode = 0;
                RED_STOP();
                YELLOW_STOP();
                GREEN_OPEN();
            }else if (i == 6) {
                normal_mode = 1;
                high_peak_mode = 0;
            }else if (i == 7) {
                normal_mode = 0;
                high_peak_mode = 1;
            }
        }

        // 读取时间信息,设定高峰期模式------------------>
        ds1302->read_time(ds1302);
        if ( (ds1302->time.hour > 0x7 && ds1302->time.hour < 0x9) || \
             (ds1302->time.hour > 0x17 && ds1302->time.hour < 0x20) ) {
            normal_mode = 0;
            high_peak_mode = 1;
        }else{
            normal_mode = 1;
            high_peak_mode = 0;
        }

        // 云控制高峰期模式------------------>
        //
    }
}

void send_info_to_cloud(unsigned char* state, uint16 time)
{
    uint8_t buffer[10];
    unsigned char header[2];
    unsigned char states[3];
    unsigned char times[3];
    uint16_t i;

    times[0] = (time / 100) % 10 + 0x30;
    times[1] = (time / 10) % 10 + 0x30;
    times[2] = (time / 1) % 10 + 0x30;


    buffer[0] = state[0];
    buffer[1] = state[1];
    buffer[2] = state[2];
    buffer[3] = times[0];
    buffer[4] = times[1];
    buffer[5] = times[2];
    buffer[6] = '\0';
    BC35_Senddata(3, buffer);
    uart1->write_string(uart1, buffer, -1);

}

void led_disp_time(int b)
{
    int s = 0,g = 0;
    s = (int)(b/10);
    g = b%10;
    tm1638_tube_dip(2, s);
    tm1638_tube_dip(3, g);
}
void led_do_normal_mode(int time)
{
    unsigned char s[3];
    if (time == 0) {
        RED_OPEN();
        YELLOW_STOP();
        GREEN_STOP();
        s[0] = '1'; s[1] = '0'; s[2] = '0';
    }

    if (time < switch_time_red_to_green) {
        led_disp_time(switch_time_red_to_green - time);
        if (one_time == false) {
            send_info_to_cloud(s, switch_time_red_to_green - time);
            one_time = true;
        }
        return;
    }

    if (time <= switch_time_red_to_green + 5) {
        if ( (switch_time_red_to_green + time + 5) %2 == 1) {
            RED_STOP();
            YELLOW_OPEN();
            GREEN_STOP();
            s[0] = '0'; s[1] = '1'; s[2] = '0';
        }else{
            RED_STOP();
            YELLOW_STOP();
            GREEN_STOP();
            s[0] = '0'; s[1] = '0'; s[2] = '0';
        }
    }
    if (time <= switch_time_red_to_green + 5 ) {
        led_disp_time(switch_time_red_to_green + 5 - time);
        if (one_time == true) {
            send_info_to_cloud(s, switch_time_red_to_green + 5 - time);
            one_time = false;
        }
        return;
    }

    if (time > switch_time_red_to_green + 5 && time <= switch_time_red_to_green + switch_time_green_to_red + 5) {
        RED_STOP();
        YELLOW_STOP();
        GREEN_OPEN();
        s[0] = '0'; s[1] = '0'; s[2] = '1';
    }
    if (time <= switch_time_red_to_green + switch_time_green_to_red + 5 ) {
        led_disp_time(switch_time_red_to_green + switch_time_green_to_red + 5 - time);
        if (one_time == false) {
            send_info_to_cloud(s, switch_time_red_to_green + switch_time_green_to_red + 5 - time);
            one_time = true;
        }
        return;
    }
    if ( (time > switch_time_red_to_green + switch_time_green_to_red + 5) && (time <= switch_time_red_to_green + switch_time_green_to_red + 10)) {
        if ( (switch_time_red_to_green + switch_time_green_to_red + time + 10) % 2 == 1) {
            RED_STOP();
            YELLOW_OPEN();
            GREEN_STOP();
            s[0] = '0'; s[1] = '1'; s[2] = '0';
        }else{
            RED_STOP();
            YELLOW_STOP();
            GREEN_STOP();
            s[0] = '0'; s[1] = '0'; s[2] = '0';
        }
    }
    if (time <= switch_time_red_to_green + switch_time_green_to_red + 10 ) {
        led_disp_time(switch_time_red_to_green + switch_time_green_to_red + 10 - time);
        if (one_time == true) {
            send_info_to_cloud(s, switch_time_red_to_green + switch_time_green_to_red + 10 - time);
            one_time = false;
        }
        return;
    }

    if ( time > switch_time_red_to_green + switch_time_green_to_red + 10) {
        irq_time_sec = 0;
    }
}

void led_do_high_peak_mode(uint16_t time)
{
    led_do_normal_mode(time - 5);
}

void TIM1_IRQHandler(void)   //TIM中断
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET){  //检查TIM更新中断发生与否
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除TIMx更新中断标志
        irq_time_k ++;
        if (irq_time_k >= 10) {
            irq_time_sec ++;
            irq_time_k = 0;
        }
    }
}

void RCC_Configuration(void)
{
    /* Enable system clocks ------------------------------------------------*/
    ErrorStatus HSEStartUpStatus;
    RCC_DeInit();                                                                                   // System clock reset.
    RCC_HSEConfig( RCC_HSE_ON );                                                                    // Open the HSE clock.
    HSEStartUpStatus = RCC_WaitForHSEStartUp();                                                     // Wait for HSE clock.
#if 1
    if( HSEStartUpStatus == SUCCESS ) {                                                             // If the HSE time consuming normal.

        FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable );                                     // Enable flash reader buffer.
        FLASH_SetLatency( FLASH_Latency_2 );                                                        // Flash wait state.
        RCC_HCLKConfig( RCC_SYSCLK_Div1 );                                                          // HCLK = SYSCLK = 72.0MHz
        RCC_PCLK2Config( RCC_HCLK_Div1 );                                                           // PCLK2 = HCLK = 72.0MHz
        RCC_PCLK1Config( RCC_HCLK_Div2 );                                                           // PCLK1 = HCLK/2 = 36.0MHz

        RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
        RCC_PLLCmd(ENABLE);                                                                         // Enable PLL

        while( RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET );                                       // Wait till PLL is ready.
        RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );                                                // Select PLL as system clock source.
        while( RCC_GetSYSCLKSource() != 0x08 );                                                     // Wait till PLL is used as system clock source.
    }
    // notice :
    // If there is as "RCC_ADCCLKConfig( RCC_PCLK2_Div6 )" peripheral clock.
    // So, the ADCLK = PCLK2 / 6 = 12MHz.
#endif
    /* Enable peripheral clocks ------------------------------------------------*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);
    /* Enable DMA1 and DMA2 clocks */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);
    /* Enable ADC1, ADC2, ADC3 and GPIOC clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
                           RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1   , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOD ,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}
