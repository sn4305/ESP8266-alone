/******************************************************************************
									果云科技                                              

							  ESP8266   开发板例程 

							     SDK Verson: 1.30   IOT
							
						            CLASS  1 :     GPIO  篇

						            
主要 内容:

GPIO 的初始化和输出(包含特殊GPIO16)

系统微秒级延时os_delay_us

/***********************************************************************************************************************

果云开发板购买地址: https://item.taobao.com/item.htm?spm=a230r.1.14.136.sTa8ie&id=42115449685&ns=1&abbucket=20#detail

***********************************************************************************************************************/
#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"

#include "user_devicefind.h"
#include "user_webserver.h"


#include "driver/gpio16.h"//GPIO16头文件
#include "Gpio.h"//GPIO头文件

#include "../include/Tcp_Client.h"
#include "driver/uart.h"//串口头文件
#include "driver/sht3x.h"
#include "ip_addr.h"
#define delay_us   os_delay_us  //系统微妙级
#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

#ifdef SERVER_SSL_ENABLE
#include "ssl/cert.h"
#include "ssl/private_key.h"
#else
#ifdef CLIENT_SSL_ENABLE
unsigned char *default_certificate;
unsigned int default_certificate_len = 0;
unsigned char *default_private_key;
unsigned int default_private_key_len = 0;
#endif
#endif


//#define MY_SSID "Tony"
#define MY_PASSWORD "yangdong"
#define MY_SSID "free"

#define STATON_IF 0x00
#define REMOTE_IP 192,168,8,1
#define TCP_PORT 9999

LOCAL os_timer_t timer;
LOCAL os_timer_t connect_timer;
LOCAL struct station_config sta_conf;
LOCAL float tempdata,humiditydata;
uint8 wsd_val[4];
uint8 sw_val[2];
uint8 Prefix[3] = {0xaa,0xbb,0xcc};
short real_temp;
short real_humi;


char zt=1;


void My_JDQ_Init(void)   //继电器初始化
{
      gpio16_output_conf();//GPIO16设置成输出模式
      gpio16_output_set(0);//默认低电平
}



//configure WIFI as station
void My_Sta_Conf(void)
{
    wifi_set_opmode(STATION_MODE);//设置为STA模式
    os_delay_us(300);
    //os_printf("wifi mode is:%d",wifi_get_opmode());

    wifi_station_get_config(&sta_conf);

    os_memcpy(sta_conf.ssid,MY_SSID,strlen(MY_SSID));
    os_memcpy(sta_conf.password,MY_PASSWORD,strlen(MY_PASSWORD));

    while(!wifi_station_set_config(&sta_conf));
    //os_printf("wifi set successfully");
}


void My_LED_Init(void)
{
    //配置 GPIO4， 5  为 普通IO口
 	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,FUNC_GPIO4);//绿灯
 	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5);//红灯
}

void  USART_init(void)
{
   //初始化两个串口为波特率115200
   uart_init(BIT_RATE_9600,BIT_RATE_115200);

   LOCAL uint8 RELAY_MODE_SET[] = {0xdd, 0xcc, 0xaa, 0x01,\
		                            0x01, 0x00, 0x00, 0x00,\
                                    0x04, 0xf4, 0x78};
   //发送字符串数据
   uart0_sendStr("==========================\r\n");
   uart0_sendStr("==========================\r\n");
   os_delay_us(3000);
   //set relay output type as NONE
   my_uart0_sendStr(RELAY_MODE_SET,11);
   os_delay_us(1000);
}

void timer_callback(){
    if(zt == 1)
    {
//    	GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 1);
//    	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 0);
//    	os_printf("zt:%d\r\n",zt);
        zt = 0;
        //uint32 current_stamp, rtc_t1, err_RTC_CurrentStamp,Cal_RTCPeriod,t1;
       // uint8 *TimeString;
       // float Cal_RTCPeriod_Real=0.0;

        //Cal_RTCPeriod = system_rtc_clock_cali_proc();
       // Cal_RTCPeriod_Real = (Cal_RTCPeriod>>12) + (((Cal_RTCPeriod*1000)>>12)%1000)/1000.0;

       // rtc_t1 = system_get_rtc_time();
       // t1 = rtc_t1 * Cal_RTCPeriod_Real / 1000000;


       //current_stamp = sntp_get_current_timestamp();

       // err_RTC_CurrentStamp = current_stamp - rtc_t1;


      //  os_printf("###Snap: %d, %s \n",current_stamp, sntp_get_real_time(current_stamp));
       // os_printf("###RTC: %d, %s \n",rtc_t1, sntp_get_real_time(current_stamp));
       // os_printf("###Cal_RTCPeriod: %d\n",Cal_RTCPeriod);
      //  os_printf("###Cal_RTCPeriod_Real: %d.%d\n",Cal_RTCPeriod>>12,((Cal_RTCPeriod*1000)>>12)%1000);
      //  os_printf("###t1: %d \n",t1);


//        my_uart0_sendStr(OPEN_RELAY2,7);

    }
    else
    {
//    	GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 0);
//    	GPIO_OUTPUT_SET(GPIO_ID_PIN(5), 1);
//    	os_printf("zt:%d\r\n",zt);
        zt = 1;
    }

	SHT3X_ReadMeasurementBuffer(&tempdata, &humiditydata);

	tempdata*=10;
	humiditydata*=10;
	real_temp=(short)(tempdata);
	real_humi=(short)(humiditydata);

	if((real_temp<0)&&(real_humi==0x3e8))
	{

	  //WWDG_SWReset();
	  SHT3X_Init(0x44);
	  SHT3X_GetTempAndHumi(&tempdata, &humiditydata, REPEATAB_HIGH, MODE_POLLING, 200);
	  SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_4HZ);
	}
	wsd_val[0]=(u8)(((short)tempdata>>8)&0xff);
	wsd_val[1]=(u8)((short)tempdata&0xff);
        wsd_val[2]=(u8)(((short)humiditydata>>8)&0xff);
	wsd_val[3]=(u8)((short)humiditydata&0xff);
	
	Start_BH1750();
	delay_ms(180);
	Read_BH1750();

	//my_uart0_sendStr(Prefix,3);
	//my_uart0_sendStr(wsd_val,4);




}

void ICACHE_FLASH_ATTR Wifi_conned(void *arg){
    static uint8 count=0;
    struct ip_info info; //用于获取IP地址的信息
    uint8 status;
    os_timer_disarm(&connect_timer);
    count++;
    status=wifi_station_get_connect_status();
    if(status==STATION_GOT_IP){
       // os_printf("Wifi connect success!");
        GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 1);

        const char remote_ip[4]={REMOTE_IP};//用于存放本地IP地址,一定要是自己的电脑真实的本地IP地址
        wifi_get_ip_info(STATON_IF,&info);

        my_station_init((struct ip_addr *)remote_ip,&info.ip,TCP_PORT);//TCP client端的业务主函数入口
        return;
    }else{
        if(count>=7){
       // os_printf("Wifi connect fail！");
        GPIO_OUTPUT_SET(GPIO_ID_PIN(4), 0);
        return;
        }
    }
    os_timer_arm(&connect_timer,2000,NULL);
}

void delay_ms(unsigned int t)
{
	 for(;t>0;t--)
	 	{
	   	delay_us(1000);
	       system_soft_wdt_feed();//这里我们喂下看门狗  ，不让看门狗复位，实际开发上不会用那么长的阻塞延时
	 	}
}
void user_rf_pre_init(void)
{
}


/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    os_printf("SDK version:%s\n", system_get_sdk_version());
 
    My_JDQ_Init();
    My_LED_Init();
    USART_init();
    My_Sta_Conf();
    i2c_master_gpio_init();
    i2c_master_init();
    SHT3X_Init(0x44);
    tempdata = 0;
    humiditydata = 0;
    sw_val[0] = 0x02;
    sw_val[1] = 0x02;
    SHT3X_GetTempAndHumi(&tempdata, &humiditydata, REPEATAB_HIGH, MODE_POLLING, 200);
    SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_4HZ);

    //SNTP get time from Internet
   // ip_addr_t *addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
   // sntp_setservername(0, "us.pool.ntp.org"); // set server 0 by domain name
   // sntp_setservername(1, "ntp.sjtu.edu.cn"); // set server 1 by domain name
   // ipaddr_aton("210.72.145.44", addr);
   // sntp_setserver(2, addr); // set server 2 by IP address
  //  sntp_init();
  //  os_free(addr);


    os_timer_disarm(&timer);
    os_timer_setfn(&timer,(os_timer_func_t *)timer_callback,NULL);
    os_timer_arm(&timer,1000,1);

//    os_timer_disarm(&timer);
//    os_timer_setfn(&timer,(os_timer_func_t *)timer_callback,NULL);
//    os_timer_arm(&timer,5000,1);

    os_timer_disarm(&connect_timer);
//    os_timer_setfn(&connect_timer,(os_timer_func_t *)Wifi_conned,NULL);
    os_timer_setfn(&connect_timer,Wifi_conned,NULL);
    os_timer_arm(&connect_timer,2000,NULL);
}

