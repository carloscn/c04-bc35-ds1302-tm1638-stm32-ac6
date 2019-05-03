///AT+NSOCL=0
#include "string.h"
#include "stdio.h"
#include "project.h"
char *strx,*extstrx;
char *command_str1,*command_str2;
extern char  RxBuffer[200],RxCounter;
BC35 BC35_Status;
extern UART* uart1;
extern UART* uart2;

char socket_number;
int bc35_init_flag;
int bc35_receive;
int bc35_receivedata;
int display;
char *bc35_socket_num;
char *bc35_socket_len;

void Delay(int i)
{
    int j,k;
    for ( j = 0; j < 7000; j ++) {
        for ( k = 0; k < i; k ++);
    }
}

void Clear_Buffer(void)//��ջ���
{
    if(display) {
         uart1->write_string(uart1,RxBuffer,-1);
    }
    memset(RxBuffer,'\0',sizeof(RxBuffer));
    RxCounter=0;
}
void BC35_Init(void)
{
    bc35_init_flag = 0;
    display = 1;//�򿪴������
    bc35_receive = 0;
    bc35_receivedata = 0;

    uart1->write_string(uart1,"\nBC35 �ϵ��Լ�...\n",-1);
    Delay(2000);
    Clear_Buffer();
    display = 1;//�򿪴������
    printf("AT\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }
    display = 0;//�رմ������
    Clear_Buffer();
    display = 1;
    uart1->write_string(uart1,"\nBoot BC35...\n",-1);
    printf("AT+NRB\r\n");//����BC35
//    Delay(8000);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    //    Clear_Buffer();
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"ERROR");//����OK
        if(strx != NULL ){
            BC35_Init();
        }
        //        Clear_Buffer();
        //        printf("AT+NRB\r\n");
        Delay(20);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
//        bc35_init_flag ++;
//        if (bc35_init_flag == 10) {
//            BC35_Init();
//        }
    }
    uart1->write_string(uart1,"Reboot init BC35 success...\n",-1);
    display = 1;
    Clear_Buffer();
    display = 1;

    printf("AT\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }


    //printf("AT+CMEE=1\r\n"); //�������ֵ
    uart2->write_string(uart2, "AT+CMEE=1\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    Clear_Buffer();
    uart2->write_string(uart2,"AT+NBAND?\r\n", -1 );
    //printf();//��ȡƵ�κ�
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+NBAND:5");//����5  ����  �ƶ��Ƿ���8 ���Է���:+NBAND:5,8,3
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        //printf("AT+NBAND?\r\n");//��ȡƵ�κ�
        uart2->write_string(uart2,"AT+NBAND?\r\n", -1 );
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+NBAND:5");//����OK
    }
    //printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
    uart2->write_string(uart2,"AT+CIMI\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"46004");//��46004
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        //printf("AT+CIMI\r\n");//��ȡ���ţ������Ƿ���ڿ�����˼���Ƚ���Ҫ��
        uart2->write_string(uart2,"AT+CIMI\r\n", -1 );
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"46004");//����OK,˵�����Ǵ��ڵ�
    }

    //printf("AT+CGATT=1\r\n");//�������磬PDP
    uart2->write_string(uart2,"AT+CGATT=1\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//��OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        //printf("AT+CGATT=1\r\n");//
        uart2->write_string(uart2,"AT+CGATT=1\r\n", -1 );
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//
        bc35_init_flag ++;
        if (bc35_init_flag == 10) {
            BC35_Init();
        }
    }
#if 1
    printf("AT+CGATT?\r\n");//��ѯ����״̬
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//��1 ��������ɹ� ��ȡ��IP��ַ��
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT?\r\n");//��ȡ����״̬
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//����1,����ע���ɹ�
        bc35_init_flag ++;
        if (bc35_init_flag == 10) {
            BC35_Init();
        }
    }
#endif
    printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CSQ");//����CSQ
    if(strx)
    {
        BC35_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//��ȡCSQ
        if(BC35_Status.CSQ==99)//˵��ɨ��ʧ��
        {
            while(1)
            {
                uart1->write_string(uart1,"�ź�����ʧ�ܣ���鿴ԭ��!\r\n",-1);
                Delay(800);
            }
        }
    }
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CSQ\r\n");//�鿴��ȡCSQֵ
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CSQ");//
        if(strx)
        {
            BC35_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//��ȡCSQ
            if(BC35_Status.CSQ==99)//˵��ɨ��ʧ��
            {
                while(1)
                {
                    uart1->write_string(uart1,"�ź�����ʧ�ܣ���鿴ԭ��!\r\n",-1);
                    Delay(800);
                }
            }
        }
    }
    Clear_Buffer();
    printf("AT+CEREG?\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CEREG:0,1");//����ע��״̬
    extstrx=strstr((const char*)RxBuffer,(const char*)"+CEREG:1,1");//����ע��״̬
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG?\r\n");//�ж���Ӫ��
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CEREG:0,1");//����ע��״̬
        extstrx=strstr((const char*)RxBuffer,(const char*)"+CEREG:1,1");//����ע��״̬
    }
    printf("AT+CEREG=1\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG=1\r\n");//�ж���Ӫ��
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }
    /*   printf("AT+COPS?\r\n");//�ж���Ӫ��
Delay(800);
strx=strstr((const char*)RxBuffer,(const char*)"46011");//���ص�����Ӫ��
Clear_Buffer();
while(strx==NULL)
{
Clear_Buffer();
printf("AT+COPS?\r\n");//�ж���Ӫ��
Delay(800);
strx=strstr((const char*)RxBuffer,(const char*)"46011");//���ص�����Ӫ��
}
     *///ż������������ ����ע���������ģ�����COPS 2,2,""�����Դ˴������ε�
    uart1->write_string(uart1,"Init BC35 success...\n",-1);
}

void BC35_ConUDP(void)
{
    uint8_t i;
    //    printf("AT+NSOCL=1\r\n");//�ر�socekt����
    Delay(800);
    printf("AT+NSOCR=DGRAM,17,3568,1\r\n");//����һ��Socket UDP
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");
    socket_number = RxBuffer[2];
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+NSOCR=DGRAM,17,3568,1\r\n");//����һ��Socket UDP
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//���ص�����Ӫ��
    }
    uart1->write_string(uart1,"����Socket���ӳɹ�...\n",-1);
    Delay(800);
    //BC35_Senddata("4", "aabbccdd");
    uart1->write_string(uart1,"\n׼��ʹ��BC35��������...\n",-1);
}
void BC35_Senddata(uint8_t len,uint8_t *data)
{
    Clear_Buffer();
    switch (socket_number) {
    // ������IP   ,�˿ں�
    case '0':
        printf("AT+NSOST=0,212.64.24.85,12345,%d,%s\r\n",len,data);
        break;
    case '1':
        printf("AT+NSOST=1,212.64.24.85,12345,%d,%s\r\n",len,data);
        break;
    case '2':
        printf("AT+NSOST=2,212.64.24.85,12345,%d,%s\r\n",len,data);
        break;
    default:break;
    }
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//����OK
    }
    //    Clear_Buffer();//��ֹ�������ݱ�����

    //    BC35_RECData();
}

void BC35_RECData(void)
{
    strx=strstr((const char*)RxBuffer,(const char*)"+NSONMI:");//����+NSONMI:���������յ�UDP���������ص�����
    if(strx!=NULL){
        Delay(2);
        Clear_Buffer();
        printf("AT+NSORF=%s,%s\r\n","2","6");//�����Լ����
        command_str1=strstr((const char*)RxBuffer,(const char*)"414142423031");//��ȡ��AABB01�����ʮ�����Ƹ�ʽ
        command_str2=strstr((const char*)RxBuffer,(const char*)"414142423032");//��ȡ��AABB02�����ʮ�����Ƹ�ʽ
        while(command_str1 == NULL && command_str2 == NULL) {//ֻҪ���յ����������е�����һ��������ѭ��
            command_str1=strstr((const char*)RxBuffer,(const char*)"414142423031");//��ȡ��AABB01�����ʮ�����Ƹ�ʽ
            command_str2=strstr((const char*)RxBuffer,(const char*)"414142423032");//��ȡ��AABB02�����ʮ�����Ƹ�ʽ
        }
        if (command_str1) {
            //���յ�AABB01�����Ĵ�����
            //uart1->write_string(uart1,"\n*****************\nAABB01\n",-1);
            uart1->write_string(uart1,"�ƿ��ƽ�������ģʽ\n",-1);
        } else if(command_str2) {
            //���յ�AABB02�����Ĵ�����
            //uart1->write_string(uart1,"\n*****************\nAABB02\n",-1);
            uart1->write_string(uart1,"�ƿ��ƽ���߷�ģʽ\n",-1);
        }
    }
}
