#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include "io.h"

extern void uart_puts(char *send_data);

// usart1 的端口信息
static usart_infor_st usart1_infor = 
{
	// .receiving_completed = &usart1_receiving_completed,
	.puts = uart_puts,
	// .printf = usart1_printf,
	// .rx_buff = usart1_rx_buff,
	// .tx_buff = usart1_tx_buff,
};
// usart2 的端口信息
static usart_infor_st usart2_infor = 
{
	// .receiving_completed = &usart2_receiving_completed,
	.puts = uart_puts,
	// .printf = usart2_printf,
	// .rx_buff = usart2_rx_buff,
	// .tx_buff = usart2_tx_buff,
};
// 端口编号和对应的端口信息
static port_to_infor_st port_infor_tbl[] = 
{
	{PORT_USART1, &usart1_infor},
	{PORT_USART2, &usart2_infor},
	{0xFFFF, NULL},
};
// 终端object_id和终端指定的端口
static id_to_port_st endpoint_id_tbl[] = 
{
	{1, ATEND1_PORT},
};
/*
*@函数：  get_information_by_port_num
*@功能：  通过端口号获取端口信息
*@参数：  端口号port_num
*@返回值：usart_infor_st* 指针，没有找到端口信息则返回NULL
*/
static void* get_information_by_port_num(uint16_t port_num)
{
	int i = 0;
	// 遍历数组，匹配端口信息
	for(i = 0; i < sizeof(port_infor_tbl)/sizeof(port_infor_tbl[0]); i++)
	{
		if(port_num == port_infor_tbl[i].port_num)
		{
			return port_infor_tbl[i].port_infor; 
		}
	}
	// 没有找到，返回NULL
	return NULL;
}
/*
*@函数：  get_shell_io
*@功能：  根据conf.h中的配置信息，获取shell端口信息
*@参数：  无
*@返回值：成功返回usart_infor_st* 指针，没有找到返回NULL
*/
usart_infor_st* get_shell_io(void)
{
	usart_infor_st* shell = (usart_infor_st*)get_information_by_port_num(SHELL_PORT);
	return shell;
}
/*
*@函数：  get_at_io
*@功能：  根据conf.h中的配置信息，获取object_id对应的端口信息
*@参数：  终端object_id
*@返回值：成功返回usart_infor_st* 指针，没有找到返回NULL
*/
usart_infor_st* get_at_io(uint16_t object_id)
{
	int8_t i = 0;
	uint16_t port_num = 0;
	usart_infor_st* at;
	
	for(i = 0; i < sizeof(endpoint_id_tbl)/sizeof(endpoint_id_tbl[0]); i++)
	{
		if(object_id == endpoint_id_tbl[i].object_id)
		{
			port_num = endpoint_id_tbl[i].port_num;
			break;
		}
	}
	
	at = (usart_infor_st*)get_information_by_port_num(port_num);
	return at;
}


