// STM32   Project.                                            			GitLab
/****************************************************************************/
/*                                                                          */
/*  @file       : uart.c	                     	                        */
/*  @Copyright  : Powered by MULTIBEANS ORG rights reserved.                */
/*  @Revision   : Ver 1.0.                                                  */
/*  @Data       : 2017.09.25 Release.                                       */
/*  @Belong     : EDU class.                                                */
/*  @Git        :        */
/*  **code : (GB2312/GBK) in Windows 10 x64. Keil   Vision4 platform.	    */
/****************************************************************************/
/*  @Attention:                                                             */
/*  ---------------------------------------------------------------------   */
/*  |    Data    |  Behavior |     Offer      |          Content         |  */
/*  | 2017.09.25 |   create  |Carlos Lopez(M) | ceate the document.      |  */
/*  ---------------------------------------------------------------------   */
/*  Email: carlos@mltbns.top                                  MULTIBEANS.   */
/****************************************************************************/

#include "project.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕
    USART2->DR = (u8) ch;
    return ch;

}

BYTE uart_buffer[UART_RECV_BUFFER_MAX];
unsigned long uart_int_count = 0;
bool    uart_full_flag  =   false;

char  RxBuffer[200],RxCounter=0;
extern int bc35_receive;
extern char *strx,*extstrx;
//extern void BC35_RECData(void);


UART* uart_new_dev(enum uart_id_t uart_num,uint32_t baud)
{
    UART* self = (UART*)malloc(sizeof(UART)*2);

    self->init          =   &uart_init;
    self->pin_set       =   &uart_pin_set;
    self->write_byte    =   &uart_write_byte;
    self->write_string  =   &uart_write_string;
    self->clear_buffer  =   &uart_clear_buffer;
    self->fetch_data    =   &uart_fetch_data;
    switch(uart_num) {

    case UART_ID_1:
        self->hw.tx_pin                     =   GPIO_Pin_9;
        self->hw.rx_pin                     =   GPIO_Pin_10;
        self->hw.io_clk                     =   RCC_APB2Periph_USART1;
        self->hw.rx_port                    =   GPIOA;
        self->hw.tx_port                    =   GPIOA;
        self->uart_cfg.baud_rate            =   baud;
        self->uart_cfg.word_length          =   USART_WordLength_8b;
        self->uart_cfg.stop_bits            =   USART_StopBits_1;
        self->uart_cfg.parity               =   USART_Parity_No;
        self->uart_cfg.hardware_flow_ctrl   =   USART_HardwareFlowControl_None;
        self->uart_cfg.mode                 =   USART_Mode_Rx | USART_Mode_Tx;
        self->uart_cfg.usart_num            =   USART1;
        self->uart_cfg.uart_id              =   UART_ID_1;
        self->init( self );
        break;

    case UART_ID_2:
        self->hw.tx_pin                     =   GPIO_Pin_2;
        self->hw.rx_pin                     =   GPIO_Pin_3;
        self->hw.io_clk                     =   RCC_APB1Periph_USART2;
        self->hw.rx_port                    =   GPIOA;
        self->hw.tx_port                    =   GPIOA;
        self->uart_cfg.baud_rate            =   baud;
        self->uart_cfg.word_length          =   USART_WordLength_8b;
        self->uart_cfg.stop_bits            =   USART_StopBits_1;
        self->uart_cfg.parity               =   USART_Parity_No;
        self->uart_cfg.hardware_flow_ctrl   =   USART_HardwareFlowControl_None;
        self->uart_cfg.mode                 =   USART_Mode_Rx | USART_Mode_Tx;
        self->uart_cfg.usart_num            =   USART2;
        self->uart_cfg.uart_id              =   UART_ID_2;

        self->init( self );
        break;

    case UART_ID_3:

        self->hw.tx_pin                     =   GPIO_Pin_10;
        self->hw.rx_pin                     =   GPIO_Pin_11;
        self->hw.io_clk                     =   RCC_APB1Periph_USART3;
        self->hw.rx_port                    =   GPIOB;
        self->hw.tx_port                    =   GPIOB;
        self->uart_cfg.baud_rate            =   baud;
        self->uart_cfg.word_length          =   USART_WordLength_8b;
        self->uart_cfg.stop_bits            =   USART_StopBits_1;
        self->uart_cfg.parity               =   USART_Parity_No;
        self->uart_cfg.hardware_flow_ctrl   =   USART_HardwareFlowControl_None;
        self->uart_cfg.mode                 =   USART_Mode_Rx | USART_Mode_Tx;
        self->uart_cfg.usart_num            =   USART3;
        self->uart_cfg.uart_id              =   UART_ID_3;
        break;
    }

    return self;
}



void    uart_init( struct uart_t *self )
{
    uint16_t  irq;
    USART_InitTypeDef   uart_cfg;
    NVIC_InitTypeDef NVIC_InitStructure;

    self->pin_set(self);


    uart_cfg.USART_BaudRate             =   self->uart_cfg.baud_rate;
    uart_cfg.USART_WordLength           =   self->uart_cfg.word_length;
    uart_cfg.USART_HardwareFlowControl  =   self->uart_cfg.hardware_flow_ctrl;
    uart_cfg.USART_StopBits             =   self->uart_cfg.stop_bits;
    uart_cfg.USART_Mode                 =   self->uart_cfg.mode;
    uart_cfg.USART_Parity               =   self->uart_cfg.parity;
    USART_Init( self->uart_cfg.usart_num, &uart_cfg );
    USART_ITConfig( self->uart_cfg.usart_num, USART_IT_RXNE, ENABLE );
    //USART_ITConfig( self->uart_cfg.usart_num, USART_IT_TXE, ENABLE );
    USART_ClearFlag( self->uart_cfg.usart_num, USART_FLAG_TXE  );
    USART_Cmd(self->uart_cfg.usart_num, ENABLE);
    if( self->uart_cfg.uart_id  ==  UART_ID_1 ) {
        irq = USART1_IRQn;
    }else if( self->uart_cfg.uart_id == UART_ID_2 ) {
        irq = USART2_IRQn;
    }else if( self->uart_cfg.uart_id == UART_ID_3 ){
        irq = USART3_IRQn;
    }
    NVIC_InitStructure.NVIC_IRQChannel = irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}
void    uart_pin_set( struct uart_t *self )
{
    GPIO_InitTypeDef io_cfg;

    if ( self->uart_cfg.uart_id  ==  UART_ID_1 ) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
        RCC_APB2PeriphClockCmd(self->hw.io_clk,ENABLE);
    }else if ( self->uart_cfg.uart_id == UART_ID_2 ) {
        RCC_APB1PeriphClockCmd(self->hw.io_clk,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    }else if ( self->uart_cfg.uart_id == UART_ID_3 ) {
        RCC_APB1PeriphClockCmd(self->hw.io_clk,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    }
    io_cfg.GPIO_Pin     =   self->hw.rx_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_IN_FLOATING;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.rx_port, &io_cfg );

    io_cfg.GPIO_Pin     =   self->hw.tx_pin;
    io_cfg.GPIO_Mode    =   GPIO_Mode_AF_PP;
    io_cfg.GPIO_Speed   =   GPIO_Speed_50MHz;
    GPIO_Init(self->hw.tx_port, &io_cfg );
}
void    uart_write_byte( struct uart_t *self, BYTE byte )
{
    while(!USART_GetFlagStatus( self->uart_cfg.usart_num ,USART_FLAG_TXE));
    USART_SendData( self->uart_cfg.usart_num, (uint16_t)byte );

}

void    uart_write_string( struct uart_t *self, BYTE *bytes, int length )
{
    uint16_t i = 0;
    if ( length < 0 ) {
        while( *(bytes + i) != '\0' ) {
            while(!USART_GetFlagStatus( self->uart_cfg.usart_num ,USART_FLAG_TXE));
            USART_SendData( self->uart_cfg.usart_num, *(bytes + i) );

            i ++;
        }
    } else {

        for ( i = 0; i < length; i++ ) {
            while(!USART_GetFlagStatus( self->uart_cfg.usart_num ,USART_FLAG_TXE));
            USART_SendData( self->uart_cfg.usart_num, *(bytes + i) );
        }
    }
}

void    uart_clear_buffer( struct uart_t *self )
{
    uint16_t i;
    for ( i = 0;    i < UART_RECV_BUFFER_MAX; i++ ) {
        *( self->uart_buffer + i) = '\0';
    }
    self->uart_rx_count =   0;

}

void     uart_fetch_data( struct uart_t *self, int length )
{

    uint16_t cpy_length = 0;

    if ( length < 0 ) {
        cpy_length = UART_RECV_BUFFER_MAX;
    }else {
        cpy_length = length;
    }
    memcpy( self->uart_buffer, uart_buffer, cpy_length );

}


void USART1_IRQHandler( void )
{

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        //bluetooth_message_deal( USART_ReceiveData(USART1) );
    }

    USART_ClearITPendingBit(USART1,USART_IT_ORE);
}

void USART2_IRQHandler( void )
{

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收模块返回的数据
    {
        RxBuffer[RxCounter++]=USART_ReceiveData(USART2);//接收模块的数据;
    }
}

void USART3_IRQHandler(void)                    //串口3中断服务程序
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收模块返回的数据
    {
    }

}

/*::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
End:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D:-D
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/

