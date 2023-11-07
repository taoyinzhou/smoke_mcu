#ifndef _IO_H
#define _IO_H

// 端口号
#define PORT_USART1      1
#define PORT_USART2      2

// 配置端口
#define SHELL_PORT            PORT_USART1
#define ATEND1_PORT           PORT_USART2

// 类型定义
typedef struct __usart_infor
{
	uint8_t* receiving_completed;        // 接收完成标志位的地址
	void (*puts)(char *);                // 字符串输出函数
	void (*printf)(char *, ...);         // 按格式字符串输出函数
	char* rx_buff;                       // 接收缓冲区
	char* tx_buff;                       // 发送缓冲区
}usart_infor_st;
typedef struct __port_to_infor
{
	uint16_t port_num;                   // 端口号，使用本文件上方定义的端口号宏
	void *  port_infor;                  // 端口号信息指针，void*类型
}port_to_infor_st;
typedef struct __id_to_port
{
	uint16_t object_id;                  // 终端object_id
	uint16_t port_num;                   // 端口号
}id_to_port_st;

// 函数
/*
*@函数：  get_shell_io
*@功能：  根据conf.h中的配置信息，获取shell端口信息
*@参数：  无
*@返回值：成功返回usart_infor_st* 指针，没有找到返回NULL
*/
extern usart_infor_st* get_shell_io(void);
/*
*@函数：  get_at_io
*@功能：  根据conf.h中的配置信息，获取object_id对应的端口信息
*@参数：  终端object_id
*@返回值：成功返回usart_infor_st* 指针，没有找到返回NULL
*/
extern usart_infor_st* get_at_io(uint16_t object_id);

#endif

