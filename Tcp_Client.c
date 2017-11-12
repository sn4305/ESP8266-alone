/*
 * Tcp_Client.c
 *
 *  Created on: 2017年10月12日
 *      Author: dongdong
 */

#include "../include/Tcp_Client.h"
#include "driver/uart.h"//串口头文件

LOCAL uint8 CLOSE_RELAY1[7] = {0xdd, 0xcc, 0xab, 0x00, 0x02, 0x07, 0x62};
LOCAL uint8 CLOSE_RELAY2[7] = {0xdd, 0xcc, 0xab, 0x02, 0x00, 0x87, 0xc3};
LOCAL uint8 OPEN_RELAY1[7] = {0xdd, 0xcc, 0xab, 0x01, 0x02, 0x06, 0xf2};
LOCAL uint8 OPEN_RELAY2[7] = {0xdd, 0xcc, 0xab, 0x02, 0x01, 0x46, 0x03};
LOCAL uint8 Prefix_wsd_val[9];
extern uint8 wsd_val[4];
extern uint8 sw_val[2];

//const u16 wCRCTalbeAbs[]={0x0000, 0xCC01, 0xD801, 0x1400,\
//		                  0xF001, 0x3C00, 0x2800, 0xE401,\
//						  0xA001, 0x6C00, 0x7800, 0xB401,\
//						  0x5000, 0x9C01, 0x8801, 0x4400};

//u16 CRC16(u8 *pchMsg,u16 wDataLen)
//{
//    u16 wCRC=0xFFFF;
//    u16 i;
//    u8 chChar;
//    for(i=0;i<wDataLen;i++)
//    {
//        chChar=*pchMsg++;
//        wCRC=wCRCTalbeAbs[(chChar^wCRC)&15]^(wCRC>>4);
//        wCRC=wCRCTalbeAbs[((chChar>>4)^wCRC)&15]^(wCRC>>4);
//    }
//    return wCRC;
//}



void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg,
        char *pdata,
        unsigned short len)
{//接收数据的回调函数

//	uint8 *temp;
//	uint8 *humi;
//    os_printf("收到数据：%s\r\n");//将接收到的数据打印出来
    if(strstr(pdata,"close") != NULL)
    {
    	if(strstr(pdata,"y1") != NULL)
    	{
//    		os_printf("%s",CLOSE_RELAY1);
    		my_uart0_sendStr(CLOSE_RELAY1,7);
    	}
    	else if(strstr(pdata,"y2") != NULL)
    	{
//    		os_printf("%s",CLOSE_RELAY2);
    		my_uart0_sendStr(CLOSE_RELAY2,7);
    	}
    }
    else if(strstr(pdata,"open") != NULL)
    {
       	if(strstr(pdata,"y1") != NULL)
       	{
//       	os_printf("%s",OPEN_RELAY1);
       		my_uart0_sendStr(OPEN_RELAY1,7);
       	}
       	else if(strstr(pdata,"y2") != NULL)
    	{
//    		os_printf("%s",OPEN_RELAY2);
       		my_uart0_sendStr(OPEN_RELAY2,7);
    	}

    }
    else if(strstr(pdata,"temp") != NULL || strstr(pdata,"humi") != NULL)
    {
    	Prefix_wsd_val[0] = 0xcc;
    	Prefix_wsd_val[1] = 0xbb;
    	Prefix_wsd_val[2] = 0xaa;
    	Prefix_wsd_val[3] = wsd_val[0];
    	Prefix_wsd_val[4] = wsd_val[1];
    	Prefix_wsd_val[5] = wsd_val[2];
    	Prefix_wsd_val[6] = wsd_val[3];
    	Prefix_wsd_val[7] = sw_val[0];
    	Prefix_wsd_val[8] = sw_val[1];
    	espconn_send(arg,Prefix_wsd_val,9);


    }

    //espconn_disconnect((struct espconn *)arg);//断开连接，是作为客户机完成一次请求然后断开的意思
}

void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg){//发送数据成功的回调函数
    os_printf("发送数据成功！！\r\n");
}

void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg){//断开连接的回调函数
    os_printf("断开连接成功！\r\n");
    GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
    os_delay_us(100000);
    espconn_connect(&user_tcp_conn);
}

void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg,sint8 err){//重连回调函数
    os_printf("连接错误，错误代码为%d\r\n",err);//打印出连接错误代码
    GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
    espconn_connect(&user_tcp_conn);
}

void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg){
    struct espconn *pespconn = arg;

    espconn_regist_recvcb(pespconn,user_tcp_recv_cb);//注册接收数据的回调函数
    espconn_regist_sentcb(pespconn,user_tcp_sent_cb);//注册发送数据成功的回调函数
    espconn_regist_disconcb(pespconn,user_tcp_discon_cb);//注册断开连接的回调函数

//    espconn_sent(pespconn,"这是ESP8266",strlen("这是ESP8266"));
    GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
}

void ICACHE_FLASH_ATTR my_station_init(struct ip_addr *remote_ip,struct ip_addr *local_ip,int remote_port){
    user_tcp_conn.type=ESPCONN_TCP;
    user_tcp_conn.state=ESPCONN_NONE;
    user_tcp_conn.proto.tcp=(esp_tcp *)os_zalloc(sizeof(esp_tcp));

    //espconn参数配置
    os_memcpy(user_tcp_conn.proto.tcp->local_ip,local_ip,4);
    os_memcpy(user_tcp_conn.proto.tcp->remote_ip,remote_ip,4);
    user_tcp_conn.proto.tcp->local_port=espconn_port();
    user_tcp_conn.proto.tcp->remote_port=remote_port;

    //注册回调函数和重连回调函数
    espconn_regist_connectcb(&user_tcp_conn,user_tcp_connect_cb);
    espconn_regist_reconcb(&user_tcp_conn,user_tcp_recon_cb);

    //启用连接
    espconn_connect(&user_tcp_conn);
}
