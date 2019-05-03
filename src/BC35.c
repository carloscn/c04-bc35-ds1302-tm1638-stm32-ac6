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

void Clear_Buffer(void)//清空缓存
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
    display = 1;//打开串口输出
    bc35_receive = 0;
    bc35_receivedata = 0;

    uart1->write_string(uart1,"\nBC35 上电自检...\n",-1);
    Delay(2000);
    Clear_Buffer();
    display = 1;//打开串口输出
    printf("AT\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    display = 0;//关闭串口输出
    Clear_Buffer();
    display = 1;
    uart1->write_string(uart1,"\nBoot BC35...\n",-1);
    printf("AT+NRB\r\n");//重启BC35
//    Delay(8000);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    //    Clear_Buffer();
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"ERROR");//返回OK
        if(strx != NULL ){
            BC35_Init();
        }
        //        Clear_Buffer();
        //        printf("AT+NRB\r\n");
        Delay(20);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
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
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT\r\n");
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }


    //printf("AT+CMEE=1\r\n"); //允许错误值
    uart2->write_string(uart2, "AT+CMEE=1\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    uart2->write_string(uart2,"AT+NBAND?\r\n", -1 );
    //printf();//获取频段号
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+NBAND:5");//返回5  电信  移动是返回8 测试返回:+NBAND:5,8,3
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        //printf("AT+NBAND?\r\n");//获取频段号
        uart2->write_string(uart2,"AT+NBAND?\r\n", -1 );
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+NBAND:5");//返回OK
    }
    //printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    uart2->write_string(uart2,"AT+CIMI\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"46004");//返46004
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        //printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        uart2->write_string(uart2,"AT+CIMI\r\n", -1 );
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"46004");//返回OK,说明卡是存在的
    }

    //printf("AT+CGATT=1\r\n");//激活网络，PDP
    uart2->write_string(uart2,"AT+CGATT=1\r\n", -1 );
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返OK
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
    printf("AT+CGATT?\r\n");//查询激活状态
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返1 表明激活成功 获取到IP地址了
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CGATT?\r\n");//获取激活状态
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
        bc35_init_flag ++;
        if (bc35_init_flag == 10) {
            BC35_Init();
        }
    }
#endif
    printf("AT+CSQ\r\n");//查看获取CSQ值
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CSQ");//返回CSQ
    if(strx)
    {
        BC35_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//获取CSQ
        if(BC35_Status.CSQ==99)//说明扫网失败
        {
            while(1)
            {
                uart1->write_string(uart1,"信号搜索失败，请查看原因!\r\n",-1);
                Delay(800);
            }
        }
    }
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+CSQ\r\n");//查看获取CSQ值
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CSQ");//
        if(strx)
        {
            BC35_Status.CSQ=(strx[5]-0x30)*10+(strx[6]-0x30);//获取CSQ
            if(BC35_Status.CSQ==99)//说明扫网失败
            {
                while(1)
                {
                    uart1->write_string(uart1,"信号搜索失败，请查看原因!\r\n",-1);
                    Delay(800);
                }
            }
        }
    }
    Clear_Buffer();
    printf("AT+CEREG?\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"+CEREG:0,1");//返回注册状态
    extstrx=strstr((const char*)RxBuffer,(const char*)"+CEREG:1,1");//返回注册状态
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG?\r\n");//判断运营商
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"+CEREG:0,1");//返回注册状态
        extstrx=strstr((const char*)RxBuffer,(const char*)"+CEREG:1,1");//返回注册状态
    }
    printf("AT+CEREG=1\r\n");
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();
    while(strx==NULL&&extstrx==NULL)
    {
        Clear_Buffer();
        printf("AT+CEREG=1\r\n");//判断运营商
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    /*   printf("AT+COPS?\r\n");//判断运营商
Delay(800);
strx=strstr((const char*)RxBuffer,(const char*)"46011");//返回电信运营商
Clear_Buffer();
while(strx==NULL)
{
Clear_Buffer();
printf("AT+COPS?\r\n");//判断运营商
Delay(800);
strx=strstr((const char*)RxBuffer,(const char*)"46011");//返回电信运营商
}
     *///偶尔会搜索不到 但是注册是正常的，返回COPS 2,2,""，所以此处先屏蔽掉
    uart1->write_string(uart1,"Init BC35 success...\n",-1);
}

void BC35_ConUDP(void)
{
    uint8_t i;
    //    printf("AT+NSOCL=1\r\n");//关闭socekt连接
    Delay(800);
    printf("AT+NSOCR=DGRAM,17,3568,1\r\n");//创建一个Socket UDP
    Delay(800);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");
    socket_number = RxBuffer[2];
    Clear_Buffer();
    while(strx==NULL)
    {
        Clear_Buffer();
        printf("AT+NSOCR=DGRAM,17,3568,1\r\n");//创建一个Socket UDP
        Delay(800);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回电信运营商
    }
    uart1->write_string(uart1,"创建Socket连接成功...\n",-1);
    Delay(800);
    //BC35_Senddata("4", "aabbccdd");
    uart1->write_string(uart1,"\n准备使用BC35发送数据...\n",-1);
}
void BC35_Senddata(uint8_t len,uint8_t *data)
{
    Clear_Buffer();
    switch (socket_number) {
    // 服务器IP   ,端口号
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
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    while(strx==NULL)
    {
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    //    Clear_Buffer();//防止返回数据被清零

    //    BC35_RECData();
}

void BC35_RECData(void)
{
    strx=strstr((const char*)RxBuffer,(const char*)"+NSONMI:");//返回+NSONMI:，表明接收到UDP服务器发回的数据
    if(strx!=NULL){
        Delay(2);
        Clear_Buffer();
        printf("AT+NSORF=%s,%s\r\n","2","6");//长度以及编号
        command_str1=strstr((const char*)RxBuffer,(const char*)"414142423031");//获取到AABB01命令的十六进制格式
        command_str2=strstr((const char*)RxBuffer,(const char*)"414142423032");//获取到AABB02命令的十六进制格式
        while(command_str1 == NULL && command_str2 == NULL) {//只要接收到两个命令中的其中一个，跳出循环
            command_str1=strstr((const char*)RxBuffer,(const char*)"414142423031");//获取到AABB01命令的十六进制格式
            command_str2=strstr((const char*)RxBuffer,(const char*)"414142423032");//获取到AABB02命令的十六进制格式
        }
        if (command_str1) {
            //接收到AABB01命令后的处理函数
            //uart1->write_string(uart1,"\n*****************\nAABB01\n",-1);
            uart1->write_string(uart1,"云控制进入正常模式\n",-1);
        } else if(command_str2) {
            //接收到AABB02命令后的处理函数
            //uart1->write_string(uart1,"\n*****************\nAABB02\n",-1);
            uart1->write_string(uart1,"云控制进入高峰模式\n",-1);
        }
    }
}
