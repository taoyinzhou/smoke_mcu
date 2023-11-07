#ifndef _COMMAND_H
#define _COMMAND_H

#include "analyze.h"
#include "data.h"

// 类型定义
typedef int8_t (*command_handler_ft)(void);
typedef struct
{
	char*  command;
	command_handler_ft handler;
}command_infor_st;

// 全局变量
extern int8_t print_at_receive_enable;

// 函数
/*
*@函数：  command_parse
*@功能：  查找并处理命令
*@参数：  char *buff
*@返回值：int8_t，成功返回0，失败返回-1
*/
extern int8_t command_parse(char *buff);
/*
*@函数：  find_handler_by_command
*@功能：  通过命令查找处理函数
*@参数：  char *command:命令字符串; uint8_t type:命令类型，定义在analyze.h中
*@返回值：command_handler_ft函数指针，失败返回NULL
*/
extern command_handler_ft find_handler_by_command(char *command, uint8_t type);


/*
*@函数：  command_xxx_xxx_handler，函数类
*@功能：  用来处理各种命令
*@参数：  void
*@返回值：int8_t，成功返回0，失败返回-1
*/
extern int8_t command_response_rst_handler(void);

extern int8_t command_notice_slvatt_handler(void);
extern int8_t command_notice_test_handler(void);
extern int8_t command_notice_start_handler(void);

extern int8_t command_usrctl_send_handler(void);
extern int8_t command_usrctl_set_handler(void);
extern int8_t command_usrctl_show_handler(void);
extern int8_t command_usrctl_default_handler(void);
extern int8_t command_usrctl_test_handler(void);
extern int8_t command_usrctl_print_handler(void);

#endif

