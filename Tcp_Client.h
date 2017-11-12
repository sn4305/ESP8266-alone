#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"

#include "user_devicefind.h"
#include "user_webserver.h"


#include "driver/gpio16.h"//GPIO16头文件
#include "Gpio.h"//GPIO头文件
#include "driver/uart.h"//串口头文件

#include "espconn.h"
#include "mem.h"
#include "ip_addr.h"

struct espconn user_tcp_conn;




void my_station_init(struct ip_addr *remote_ip,struct ip_addr *local_ip,int remote_port);//client业务主函数
void my_uart0_sendStr(const char *str,uint8 length);

#endif /* CLIENT_H_ */
