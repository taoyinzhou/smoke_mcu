#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"

void uart_puts(char *send_data);
int8_t print_at_receive_enable = 0;
static command_infor_st command_infor_response_tbl[] = 
{
	{"RST", command_response_rst_handler},
};
static command_infor_st command_infor_notice_tbl[] = 
{
	{"MT_SMOKE_SLV_ATT", command_notice_slvatt_handler},
	{"MT_SMOKE_TEST_CMD", command_notice_test_handler},
	{"SERVER", command_notice_start_handler},
};
static command_infor_st command_infor_usrctl_tbl[] = 
{
	{"send", command_usrctl_send_handler},
	{"set", command_usrctl_set_handler},
	{"show", command_usrctl_show_handler},
	{"default", command_usrctl_default_handler},
	{"test", command_usrctl_test_handler},
	{"print", command_usrctl_print_handler},
};

/*
*@函数：  command_parse
*@功能：  查找并处理命令
*@参数：  char *buff
*@返回值：int8_t，成功返回0，失败返回-1
*/
int8_t command_parse(char *buff)
{
	int ret;
	command_handler_ft temp;

	ret = get_args(buff);
	temp = find_handler_by_command(analyze_command, ret);
	

	if(temp)
	{
		return (*temp)();
	}
	return -1;
}
/*
*@函数：  find_handler_by_command
*@功能：  通过命令查找处理函数
*@参数：  char *command:命令字符串; uint8_t type:命令类型，定义在analyze.h中
*@返回值：command_handler_ft函数指针，失败返回NULL
*/
command_handler_ft find_handler_by_command(char *command, uint8_t type)
{
	command_infor_st *temp = NULL;
	int len = 0;
	int i = 0;
	switch(type)
	{
		case ANALYZE_FORMAT_RESPONSE:
			temp = command_infor_response_tbl;
			len = sizeof(command_infor_response_tbl)/sizeof(command_infor_response_tbl[0]);
			break;
		case ANALYZE_FORMAT_NOTICE:
			temp = command_infor_notice_tbl;
			len = sizeof(command_infor_notice_tbl)/sizeof(command_infor_notice_tbl[0]);
			break;
		case ANALYZE_FORMAT_USERCTL:
			temp = command_infor_usrctl_tbl;
			len = sizeof(command_infor_usrctl_tbl)/sizeof(command_infor_usrctl_tbl[0]);
			break;
		default:
			return NULL;
	}
	for(i = 0; i < len; i++)
	{
		if(strcmp((const char *)temp[i].command, (const char *)command) == 0)
		{
			return temp[i].handler;
		}
	}
	return NULL;
}


/*
*@函数：  command_xxx_xxx_handler，函数类
*@功能：  用来处理各种命令
*@参数：  void
*@返回值：int8_t，成功返回0，失败返回-1
*/
int8_t command_response_rst_handler(void)
{
	return 0;
}
int8_t command_notice_slvatt_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	
	uint16_t object_id = atoi(analyze_parameter[0]);
	uint16_t att_num = AttNum_SmokeSensitivityLevel;
	uint16_t att_value = atoi(analyze_parameter[1]);
	
	if(analyze_parameter_num != 2)
	{
		printf("Warning: Wrong parametter number of AT\r\n");
		return -1;
	}
	if(attribute_set(object_id, att_num, att_value) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}
	if(update_one_attribute(object_id, att_num) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}
	printf("OK!\r\n");
	return 0;
}
int8_t command_notice_test_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	
	uint16_t object_id = atoi(analyze_parameter[0]);
	uint16_t att_num = AttNum_TestInProgress;
	uint16_t att_value = 1;
	
	if(analyze_parameter_num != 1)
	{
		printf("Warning: Wrong parametter number of AT\r\n");
		return -1;
	}
	if(attribute_set(object_id, att_num, att_value) < 0)
	{	
		printf("Process error!\r\n");
		return -1;
	}
	if(update_one_attribute(object_id, att_num) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}
	
	printf("OK!\r\n");
	return 0;
}

int8_t command_notice_start_handler(void)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_START\r\n");
	printf("%s", buf);
	uart_puts(buf);

	return 0;
}

int8_t command_usrctl_send_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	
	uint16_t object_id = atoi(analyze_parameter[0]);
	usart_infor_st *atout = get_at_io(object_id);
	
	if(analyze_parameter_num != 2)
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: sent 1 xxx\r\n");
		return -1;
	}
	if(!atout)
	{
		printf("Process error!\r\n");
		return -1;
	}
	printf("%s\r\n", analyze_parameter[1]);
	printf("OK!\r\n");
	return 0;
}
int8_t command_usrctl_set_handler(void)
{
	//格式：set 1 1 1
	/* 属性编号和取值
	0	ExpressedState;          ExpressedStateEnum类型      范围0~8   默认值为ExpState_Normal
	1	SmokeState;              AlarmStateEnum类型          范围0~2   默认值为AlaState_Normal
	2	BatteryAlert;            AlarmStateEnum类型          范围0~2   默认值为AlaState_Normal
	3	TestInProgress;          Bool类型                    取值0或1  默认值为0
	4	HardwareFaultAlert;      Bool类型                    取值0或1  默认值为0
	5	EndOfServiceAlert;       EndOfServiceEnum类型        范围0~1   默认值为Eos_Normal
	6	ContaminationState;      ContaminationStateEnum类型  范围0~3   默认值为CtmState_Normal
	7	SmokeSensitivityLevel;   SensitivityEnum类型         范围0~2   默认值为Slv_Standard
	*/
	usart_infor_st* shell = get_shell_io();
	
	uint16_t object_id = atoi(analyze_parameter[0]);
	uint16_t att_num = atoi(analyze_parameter[1]);
	uint16_t att_value = atoi(analyze_parameter[2]);
	
	if(analyze_parameter_num != 3)
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: set 1 1 0\r\n");
		return -1;
	}
	printf("%s,%d\n", __FUNCTION__, __LINE__);
	if(attribute_set(object_id, att_num, att_value) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}

	printf("OK!\r\n");

	return 0;
}
int8_t command_usrctl_show_handler(void)
{
	//格式：show 1 1
	usart_infor_st* shell = get_shell_io();
	
	uint16_t object_id = atoi(analyze_parameter[0]);
	uint16_t att_num = atoi(analyze_parameter[1]);
	
	if(analyze_parameter_num == 1)
	{
		if(show_attributes(object_id) < 0)
		{
			printf("Process error!\r\n");
			return -1;
		}
		return 0;
	}
	else if(analyze_parameter_num == 2)
	{
		object_id = atoi(analyze_parameter[0]);
		if(show_one_attribute(object_id, att_num) < 0)
		{
			printf("Process error!\r\n");
			return -1;
		}
		printf("OK!\r\n");
		return 0;
	}
	else
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: show 1\r\n");
		printf("Example: show 1 1\r\n");
		return -1;
	}
}
int8_t command_usrctl_default_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	uint16_t object_id;
	
	if(analyze_parameter_num != 1)
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: default 1\r\n");
		return -1;
	}
	
  object_id = atoi(analyze_parameter[0]);
	if(default_attributes(object_id) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}		
	printf("OK!\r\n");
	return 0;
}
int8_t command_usrctl_test_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	uint16_t object_id = atoi(analyze_parameter[0]);
	uint16_t att_num = AttNum_TestInProgress;
	uint16_t att_value;
	
	if(analyze_parameter_num != 2)
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: test 1 on\r\n");
		printf("Example: test 1 off\r\n");
		return -1;
	}
	
	if(strcmp(analyze_parameter[1], "on") == 0)
	{
		att_value = 1;
	}
	else if(strcmp(analyze_parameter[1], "off") == 0)
	{
		att_value = 0;
	}
	else
	{
		printf("Invalid operation\r\n");
		return -1;
	}
	
	if(attribute_set(object_id, att_num, att_value) < 0)
	{
		printf("Process error!\r\n");
		return -1;
	}
	printf("OK!\r\n");
	return 0;
}
int8_t command_usrctl_print_handler(void)
{
	usart_infor_st* shell = get_shell_io();
	
	if(analyze_parameter_num != 1)
	{
		printf("Warning: Wrong parametter number of command\r\n");
		printf("Example: print on\r\n");
		printf("Example: print off\r\n");
		return -1;
	}
	
	if(strcmp(analyze_parameter[0], "on") == 0)
	{
		print_at_receive_enable = 1;
	}
	else if(strcmp(analyze_parameter[0], "off") == 0)
	{
		print_at_receive_enable = 0;
	}
	else
	{
		printf("Invalid operation\r\n");
		return -1;
	}
	
	printf("OK!\r\n");
	return 0;
}



