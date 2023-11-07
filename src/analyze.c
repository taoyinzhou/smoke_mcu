#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "analyze.h"

uint8_t analyze_parameter_num = 0;
char    analyze_command[ANALYZE_MAX_CMDLEN + 1];
char    analyze_parameter[ANALYZE_MAX_PARANUM][ANALYZE_MAX_PARALEN + 1];

/*
*@函数：  check_format
*@功能：  检查缓冲区格式，并将格式对应的信息输出到ptype参数中
*@参数：  消息地址src，输出型参数ptype
*@返回值：返回格式类型编号
*/
static int8_t check_format(char *src, analyze_type_infor_st *ptype)
{
	if(strcmp((const char *)src, "OK") == 0)
	{
	
		return ANALYZE_FORMAT_ATTEST;
	}
	else if(strcmp((const char *)src, "") == 0)
	{
		return ANALYZE_FORMAT_ATTEST;
	}
	else if(src[0] == '+')
	{
		ptype->start_index = 1;
		ptype->flag_command = ':';
		ptype->flag_parameter = ',';
		return ANALYZE_FORMAT_RESPONSE;
	}
	else if(src[0] == '>')
	{
		ptype->start_index = 1;
		ptype->flag_command = ':';
		ptype->flag_parameter = ',';
		return ANALYZE_FORMAT_NOTICE;
	}
	else
	{	
		ptype->start_index = 0;
		ptype->flag_command = ' ';
		ptype->flag_parameter = ' ';
		return ANALYZE_FORMAT_USERCTL;
	}
}
/*
*@函数：  get_args
*@功能：  从字符串中解析args，写入analyze_command和analyze_parameter中，并更新analyze_parameter_num
*@参数：  缓冲区地址src
*@返回值：成功返回格式类型编号，失败返回-1
*/
int8_t get_args(char *src)
{
	int n;
	int i, j, len;
	analyze_type_infor_st rec_type;
	
	analyze_parameter_num = 0;
	len = strlen(src);// 计算消息长度
	int8_t ret = check_format(src, &rec_type);// 检查消息格式
	
	switch(ret)
	{
		case ANALYZE_FORMAT_ATTEST:
			return ret;// 对于AT测试回复，无需解析直接返回
		default:
			// 解析command
			j = 0;
			memset(analyze_command, 0, sizeof(analyze_command));
			for (n = rec_type.start_index; j < ANALYZE_MAX_CMDLEN && n < len; n++, j++)
			{
				if (src[n] == rec_type.flag_command)
				{
					break;
				}
				analyze_command[j] = src[n];
			}
			// 如果command长度达到conf.h配置的最大值，且下个字符既不是分隔符也不是结束符，异常退出
			if (j >= ANALYZE_MAX_CMDLEN && src[n] != rec_type.flag_command && src[n] != '\0')
			{
				// 将command清空，参数个数置为0
				memset(analyze_command, 0, sizeof(analyze_command));
				analyze_parameter_num = 0;
				return -1;
			}
			// 解析parameter
			i = 0, j = 0, analyze_parameter_num = 0;
			n++;
			while(n <= len)
			{
				analyze_parameter[i][j] = src[n];
				if (analyze_parameter[i][j] == rec_type.flag_parameter || analyze_parameter[i][j] == '\0')
				{
					if (j)
					{
						analyze_parameter[i][j] = '\0';
						analyze_parameter_num++;
						j = 0;
						i++;
					}
				}
				else
				{
					j++;
				}
				// 如果parameter长度超过限制，异常退出
				if (j > ANALYZE_MAX_PARALEN)
				{
					memset(analyze_command, 0, sizeof(analyze_command));
					analyze_parameter_num = 0;
					return -1;
				}
				// 如果parameter个数超过限制，异常退出
				if (i > ANALYZE_MAX_PARANUM)
				{
					memset(analyze_command, 0, sizeof(analyze_command));
					analyze_parameter_num = 0;
					return -1;
				}
				n++;
			}
			return ret;
	}
}

