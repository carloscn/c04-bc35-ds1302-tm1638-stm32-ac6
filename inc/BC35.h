#include "uart.h"
//#include <stm32l1xx.h>
void Clear_Buffer(void);//��ջ���	
void BC35_Init(void);
void BC35_PDPACT(void);
void BC35_ConUDP(void);
void BC35_RECData(void);
extern void BC35_Senddata(uint8_t len,uint8_t *data);
typedef struct
{
     uint8_t CSQ;
	 uint8_t Socketnum;   //���
	 uint8_t reclen[2];   //��ȡ�����ݵĳ���
     uint8_t res[2];
     uint8_t recdatalen[10];
     uint8_t recdata[100];
	 uint8_t uart1len[10];
	 uint8_t senddata[100];
} BC35;
