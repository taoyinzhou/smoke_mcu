#ifndef _DATA_H
#define _DATA_H

#include "io.h"

// 告警优先级，值越大优先级越高
#define PRI_SmokeAlarm        5
#define PRI_BatteryAlert      4
#define PRI_Testing           3
#define PRI_HardwareFault     2
#define PRI_EndOfService      1

// 事件编号
#define  Event_SmokeAlarm                0
#define  Event_LowBattery                2
#define  Event_HardwareFault             3
#define  Event_EndOfService              4
#define  Event_SelfTestComplete          5
#define  Event_AllClear                  10

// 属性编号
#define  AttNum_ExpressState             0
#define  AttNum_SmokeState               1
#define  AttNum_BatteryAlert             2
#define  AttNum_TestInProgress           3
#define  AttNum_HardwareFaultAlert       4
#define  AttNum_EndOfServiceAlert        5
#define  AttNum_ContaminationState       6 
#define  AttNum_SmokeSensitivityLevel    7
#define  AttNum_BatVoltage               8
#define  AttNum_BatPercentRemaining      9
#define  AttNum_BatReplacementneeded    10
#define  AttNum_BatReplaceability       11
#define  AttNum_BatChargeLevel          12
#define  AttNum_BatChargeState          13
#define  AttNum_BatFunctionalWhileCharging    14
// 属性编号最大值
#define  AttNum_Max                      AttNum_BatFunctionalWhileCharging


// 属性取值
// ExpressedStateEnum类型：ExpressedState
#define  ExpState_Normal                 0
#define  ExpState_SmokeAlarm             1
#define  ExpState_COAlarm                2
#define  ExpState_BatteryAlert           3
#define  ExpState_Testing                4
#define  ExpState_HardwareFault          5
#define  ExpState_EndOfService           6
#define  ExpState_InterconnectSmoke      7
#define  ExpState_InterconnectCO         8
// ExpressedStateEnum最大值
#define  ExpState_Max                    ExpState_InterconnectCO

// AlarmStateEnum类型：SmokeState, BatteryAlert
#define  AlaState_Normal                 0
#define  AlaState_Warning                1
#define  AlaState_Critical               2
// AlarmStateEnum最大值
#define  AlaState_Max                    AlaState_Critical

// EndOfServiceEnum类型：EndOfServiceAlert
#define  Eos_Normal                      0
#define  Eos_Expired                     1
// EndOfServiceEnum最大值
#define  Eos_Max                         Eos_Normal

// ContaminationStateEnum类型：ContaminationState
#define  CtmState_Normal                 0
#define  CtmState_Low                    1
#define  CtmState_Warning                2
#define  CtmState_Critical               3
// ContaminationStateEnum最大值
#define  CtmState_Max                    CtmState_Critical

// SensitivityEnum类型：SmokeSensitivityLevel
#define  Slv_High                        0
#define  Slv_Standard                    1
#define  Slv_Low                         2
// SensitivityEnum最大值
#define  Slv_Max                         Slv_Low

// 类型定义
typedef void (*event_handler_ft)(usart_infor_st*, uint16_t, uint16_t);
typedef struct __attribute_infor
{
	char*          name;                           // 属性名
	char*          atstr;                          // 属性AT字符串，用于填充%s
	int16_t        value_exp;                      // 属性与ExpressState的关系，-1表示该属性不影响ExpressState
	int16_t        priority;                       // 属性优先级，-1表示该属性不考虑优先级
	uint16_t       value_local;                    // 属性本地值
	uint16_t       value_remote;                   // 属性远程值（即记录最后一次MCU写入模块的属性值）
	const uint16_t value_default;                  // 属性默认值，只读，用于判断是否有告警产生
	event_handler_ft event_func;                   // 属性关联事件对应的处理函数
}attribute_infor_st;
typedef struct __endpoint_infor
{
	uint16_t             objectid;   // 终端id
	attribute_infor_st*  att_tbl;    // 终端属性信息
}endpoint_infor_st;

// 函数
/*
*@函数：  attribute_change_detect_handler
*@功能：  根据object_id，完成对应终端的属性检查和更新
*@参数：  object_id
*@返回值：void
*/
extern void attribute_change_detect_handler(uint16_t object_id);
/*
*@函数：  default_attributes
*@功能：  将所有属性置为默认值
*@参数：  object_id
*@返回值：int8_t 成功返回0，失败返回-1
*/
extern int8_t default_attributes(uint16_t object_id);
/*
*@函数：  show_attributes
*@功能：  通过指定的端口（建议为shell）显示所有属性值
*@参数：  object_id, port
*@返回值：int8_t 成功返回0，失败返回-1
*/
extern int8_t show_attributes(uint16_t object_id);
/*
*@函数：  attribute_set
*@功能：  设置一个属性的值
*@参数：  object_id, att_num, att_value
*@返回值：int8_t 成功返回0，失败返回-1
*/
extern int8_t attribute_set(uint16_t object_id, uint16_t att_num, uint16_t att_value);
/*
*@函数：  update_one_attribute
*@功能：  向模块更新指定属性，即使这个属性值并没有变更
*@参数：  object_id, att_num
*@返回值：int8_t 成功返回0，失败返回-1
*/
extern int8_t update_one_attribute(uint16_t object_id, uint16_t att_num);
/*
*@函数：  show_one_attribute
*@功能：  显示某终端的一个指定属性值，通过指定的端口
*@参数：  object_id, att_num, port
*@返回值：int8_t 成功返回0，失败返回-1
*/
extern int8_t show_one_attribute(uint16_t object_id, uint16_t att_num);


/*
*@函数：  event_XXX_handler函数类
*@功能：  检查是否触发事件，如果触发进行处理（使用AT向模块发送）
*@参数：  atout object_id value
*@返回值：void
*/
extern void event_smokealarm_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_lowbattery_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_hardwarefault_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_endOfservice_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_selftestcomplete_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_allclear_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_voltage_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_percent_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_replacementneeded_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_replaceability_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_chargelevel_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_chargestate_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);
extern void event_bat_functionalwhilecharging_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value);

#endif
