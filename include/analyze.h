#ifndef _ANALYZE_H
#define _ANALYZE_H

// 配置解析信息的参数
#define ANALYZE_MAX_CMDLEN    64     // 命令长度上限
#define ANALYZE_MAX_PARALEN   64     // 参数长度上限
#define ANALYZE_MAX_PARANUM   6      // 参数个数上限

// 格式类型编号
#define ANALYZE_FORMAT_ATTEST          0x11          // AT测试反馈
#define ANALYZE_FORMAT_RESPONSE        0x12          // 命令回复（响应）
#define ANALYZE_FORMAT_NOTICE          0x13          // 模块通知
#define ANALYZE_FORMAT_USERCTL         0x14          // 用户控制命令

// 类型定义
typedef struct
{
	uint8_t start_index;       // 起始索引
	char    flag_command;      // 命令分隔符，对于AT信息为":"，对于用户命令为" "
	char    flag_parameter;    // 参数分隔符，对于AT信息为","，对于用户命令为" "
}analyze_type_infor_st;

// 全局变量
extern  uint8_t analyze_parameter_num;
extern  char    analyze_command[ANALYZE_MAX_CMDLEN + 1];
extern  char    analyze_parameter[ANALYZE_MAX_PARANUM][ANALYZE_MAX_PARALEN + 1];

// 函数
/*
*@函数：  get_args
*@功能：  从字符串中解析args，写入analyze_command和analyze_parameter中，并更新analyze_parameter_num
*@参数：  缓冲区地址src
*@返回值：成功返回格式类型编号，失败返回-1
*/
extern int8_t get_args(char *src);

#endif

