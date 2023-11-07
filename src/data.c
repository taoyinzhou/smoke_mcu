#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

/*
ExpressedState;          ExpressedStateEnum类型      范围0~8   默认值为ExpState_Normal
SmokeState;              AlarmStateEnum类型          范围0~2   默认值为AlaState_Normal
BatteryAlert;            AlarmStateEnum类型          范围0~2   默认值为AlaState_Normal
TestInProgress;          Bool类型                    取值0或1  默认值为0
HardwareFaultAlert;      Bool类型                    取值0或1  默认值为0
EndOfServiceAlert;       EndOfServiceEnum类型        范围0~1   默认值为Eos_Normal
ContaminationState;      ContaminationStateEnum类型  范围0~3   默认值为CtmState_Normal
SmokeSensitivityLevel;   SensitivityEnum类型         范围0~2   默认值为Slv_Standard
*/
// 终端1的属性集合
static attribute_infor_st smoke_att_end1[] = 
{
	{"ExpressedState",             "MT_SMOKE_EXPSTATE_ATT", -1,                     -1,                ExpState_Normal,  ExpState_Normal, ExpState_Normal, event_allclear_handler},
	{"SmokeState",                 "MT_SMOKE_STATE_ATT",    ExpState_SmokeAlarm,    PRI_SmokeAlarm,    AlaState_Normal,  AlaState_Normal, AlaState_Normal, event_smokealarm_handler},
	{"BatteryAlert",               "MT_SMOKE_BAT_ATT",      ExpState_BatteryAlert,  PRI_BatteryAlert,  AlaState_Normal,  AlaState_Normal, AlaState_Normal, event_lowbattery_handler},
	{"TestInProgress",             "MT_SMOKE_TIP_ATT",      ExpState_Testing,       PRI_Testing,       0,                0,               0,               event_selftestcomplete_handler},
	{"HardwareFaultAlert",         "MT_SMOKE_HF_ATT",       ExpState_HardwareFault, PRI_HardwareFault, 0,                0,               0,               event_hardwarefault_handler},
	{"EndOfServiceAlert",          "MT_SMOKE_EOS_ATT",      ExpState_EndOfService,  PRI_EndOfService,  Eos_Normal,       Eos_Normal,      Eos_Normal,      event_endOfservice_handler},
	{"ContaminationState",         "MT_SMOKE_CTMS_ATT",     -1,                     -1,                CtmState_Normal,  CtmState_Normal, CtmState_Normal, NULL},
	{"SmokeSensitivityLevel",      "MT_SMOKE_SLV_ATT",      -1,                     -1,                Slv_Standard,     Slv_Standard,    Slv_Standard,    NULL},
	{"BatVoltage",                 "MT_PSBAT_VOL_ATT",      -1,                     -1,                3000,             3000,            3000,            event_bat_voltage_handler},
	{"BatPercentRemaining",        "MT_PSBAT_PR_ATT",       -1,                     -1,                100,             100,            100,            event_bat_percent_handler},
	{"BatReplacementneeded",       "MT_PSBAT_REPND_ATT",    -1,                     -1,                0,             0,            0,            event_bat_replacementneeded_handler},
	{"BatReplaceability",          "MT_PSBAT_REPBLT_ATT",   -1,                     -1,                0,             0,            0,            event_bat_replaceability_handler},
	{"BatChargeLevel",             "MT_PSBAT_CHGLV_ATT",    -1,                     -1,                0,             0,            0,            event_bat_chargelevel_handler},
	{"BatChargeState",             "MT_PSBAT_CHGSTA_ATT",   -1,                     -1,                0,             0,            0,            event_bat_chargestate_handler},
	{"BatFunctionalWhileCharging", "MT_PSBAT_WKWCHG_ATT ",  -1,                     -1,                0,             0,            0,            event_bat_functionalwhilecharging_handler},
	{"\0", "\0",-1,-1,0,0,0,NULL},   // 数组结束
};
// object_id对应的属性
endpoint_infor_st endpoint_infor_tbl[] = 
{
	{1, smoke_att_end1},
};

/*
*@函数：  match_att_tbl_by_id
*@功能：  根据object_id，匹配对应的属性集合
*@参数：  object_id
*@返回值：成功返回attribute_infor_st*指针，失败返回NULL
*/
attribute_infor_st* match_att_tbl_by_id(uint16_t object_id)
{
	int i = 0;
	// 遍历数组，匹配id对应的属性集合
	for(i = 0; i < sizeof(endpoint_infor_tbl)/sizeof(endpoint_infor_tbl[0]); i++)
	{
		if(endpoint_infor_tbl[i].objectid == object_id)
		{
			return endpoint_infor_tbl[i].att_tbl;
		}
		i++;
	}
	return NULL;
}
/*
*@函数：  expressstate_handler
*@功能：  根据object_id，处理ExpressState属性
*@参数：  object_id
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t expressstate_handler(uint16_t object_id)
{
	int i = 0;
	int priority_max = 0;
	
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	// ExpressState的默认值为ExpState_Normal
	att_tbl[AttNum_ExpressState].value_local = ExpState_Normal;
	// 遍历属性集合
	while(att_tbl[i].name[0] != '\0')
	{
		if(priority_max < att_tbl[i].priority && att_tbl[i].value_local != att_tbl[i].value_default)
		{
			att_tbl[AttNum_ExpressState].value_local = att_tbl[i].value_exp;
			priority_max = att_tbl[i].priority;
		}
		i++;
	}
	return 0;
}
/*
*@函数：  check_attributes
*@功能：  根据object_id，检查属性是否变更
*@参数：  object_id
*@返回值：成功返回int32_t型flag，记录变更的属性，最高位无效；失败返回-1
*/
int32_t check_attributes(uint16_t object_id)
{
	int32_t i = 0;
	int32_t flag = 0;
	
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	// 用flag的位记录哪些属性变更了，最多记录31个属性
	while(att_tbl[i].name[0] != '\0' && i < 31)
	{
		if(att_tbl[i].value_remote != att_tbl[i].value_local)
		{
			flag |= (1 << i);
		}
		i++;
	}
	if(i < 31)
	{
		return flag;
	}
	return -1;  // 属性过多，异常退出
}
/*
*@函数：  update_attributes
*@功能：  通过AT向模块更新变更的属性
*@参数：  object_id, flag
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t update_attributes(uint16_t object_id, int32_t flag)
{
	int i = 0;
	
	usart_infor_st* atout = get_at_io(object_id);

	if(!atout)
	{
		return -1;  // 找不到终端id的端口，异常退出
	}

	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);

	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}

	while(flag)
	{
		if (flag & (1 << i))
		{
			printf("AT+%s=%d,%d\r\n", att_tbl[i].atstr, object_id, att_tbl[i].value_local);
			if (att_tbl[i].event_func != NULL)
			{
				// 调用事件函数，检查是否触发事件，如果触发则通过AT告知模块
				(*att_tbl[i].event_func)(atout, object_id, att_tbl[i].value_local);
			}
			att_tbl[i].value_remote = att_tbl[i].value_local;
			flag &= ~(1 << i);
		}
		i++;
	}
	printf("%d\n", __LINE__);
	return 0;
}
/*
*@函数：  attribute_change_detect_handler
*@功能：  根据object_id，完成对应终端的属性检查和更新
*@参数：  object_id
*@返回值：void
*/
void attribute_change_detect_handler(uint16_t object_id)
{
	int32_t flag = 0;
	
	expressstate_handler(object_id);// 处理expressstate属性
	flag = check_attributes(object_id);// 检查属性，如果变更记录在flag中
	if(flag > 0)
	{
		update_attributes(object_id, flag);
	}

}
/*
*@函数：  default_attributes
*@功能：  将所有属性置为默认值
*@参数：  object_id
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t default_attributes(uint16_t object_id)
{
	int i = 0;
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	while(att_tbl[i].name[0] != '\0' && i < 31)
	{
		att_tbl[i].value_local = att_tbl[i].value_default;
		i++;
	}
	if(i < 31)
	{
		return 0;
	}
	return -1;  // 属性过多，异常退出
}
/*
*@函数：  show_attributes
*@功能：  通过指定的端口（建议为shell）显示所有属性值
*@参数：  object_id
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t show_attributes(uint16_t object_id)
{
	int i = 0;
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	while(att_tbl[i].name[0] != '\0')
	{
		printf("%-25s%d\r\n", att_tbl[i].name, att_tbl[i].value_local);
		i++;
	}
	printf("\r\n");
	
	return 0;
}
/*
*@函数：  attribute_set
*@功能：  设置一个属性的值
*@参数：  object_id, att_num, att_value
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t attribute_set(uint16_t object_id, uint16_t att_num, uint16_t att_value)
{
	uint16_t value_max = 0;
	attribute_infor_st* att = match_att_tbl_by_id(object_id);
	if(!att)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	switch(att_num)
	{
		case AttNum_ExpressState:
			return -1; // ExpressState不允许用户设置，由程序自行判断进行赋值
		case AttNum_SmokeState:
			value_max = AlaState_Max;
			break;
		case AttNum_BatteryAlert:
			value_max = AlaState_Max;
			break;
		case AttNum_TestInProgress:
			value_max = 1; // bool变量
			break;
		case AttNum_HardwareFaultAlert:
			value_max = 1; // bool变量
			break;
		case AttNum_EndOfServiceAlert:
			value_max = Eos_Max;
			break;
		case AttNum_ContaminationState:
			value_max = CtmState_Max;
			break;
		case AttNum_SmokeSensitivityLevel:
			value_max = Slv_Max;
			break;
		default:
			return -1;  // 属性编号超过上限，异常退出
	}
	if(att_value > value_max)
	{
		return -1; // 属性值超过上限，异常退出
	}
	att[att_num].value_local = att_value;
	return 0;
}
/*
*@函数：  update_one_attribute
*@功能：  向模块更新指定属性，即使这个属性值并没有变更
*@参数：  object_id, att_num
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t update_one_attribute(uint16_t object_id, uint16_t att_num)
{
	if(att_num > AttNum_Max)
	{
		return -1;  // 无效的属性编号，异常退出
	}
	usart_infor_st* atout = get_at_io(object_id);
	if(!atout)
	{
		return -1;  // 无效的id导致获取到无效的端口，异常退出
	}
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl)
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	printf("AT+%s=%d,%d\r\n", att_tbl[att_num].atstr, object_id, att_tbl[att_num].value_local);
	att_tbl[att_num].value_remote = att_tbl[att_num].value_local;
	return 0;
}
/*
*@函数：  show_one_attribute
*@功能：  显示某终端的一个指定属性值，通过指定的端口
*@参数：  object_id, att_num
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t show_one_attribute(uint16_t object_id, uint16_t att_num)
{
	attribute_infor_st* att_tbl = match_att_tbl_by_id(object_id);
	if(!att_tbl )
	{
		return -1;  // 无效的id，匹配不到属性集合，异常退出
	}
	if(att_num > AttNum_Max)
	{
		return -1;  // 无效的属性编号，异常退出
	}
	printf("%-25s%d\r\n", att_tbl [att_num].name, att_tbl [att_num].value_local);
	return 0;
}

/*
*@函数：  event_XXX_handler函数类
*@功能：  检查是否触发事件，如果触发进行处理（使用AT向模块发送）
*@参数：  atout object_id value
*@返回值：void
*/
void event_smokealarm_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};

	if(value != 0)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_SmokeAlarm);
		printf("%s", buf);
		atout->puts(buf);
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+MT_SMOKE_STATE_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_lowbattery_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	if(value != 0)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_LowBattery);
		printf("%s", buf);
		atout->puts(buf);
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+MT_SMOKE_BAT_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_hardwarefault_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	if(value == 1)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_HardwareFault);
		printf("%s", buf);
		atout->puts(buf);
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+MT_SMOKE_HF_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_endOfservice_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	if(value == 1)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_EndOfService);
		printf("%s", buf);
		atout->puts(buf);
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+MT_SMOKE_EOS_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_selftestcomplete_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	if(value == 0)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_SelfTestComplete);
		printf("%s", buf);
		atout->puts(buf);
	}
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "AT+MT_SMOKE_TIP_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_allclear_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	if(value == 0)
	{
		sprintf(buf, "AT+MT_SMOKE_EVT=%d,%d\r\n", object_id, Event_AllClear);
		printf("%s", buf);
		atout->puts(buf);
	}
}
void event_bat_voltage_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_VOL_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_percent_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_PR_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_replacementneeded_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_REPND_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_replaceability_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_REPBLT_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_chargelevel_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_CHGLV_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_chargestate_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_CHGSTA_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}
void event_bat_functionalwhilecharging_handler(usart_infor_st* atout, uint16_t object_id, uint16_t value)
{
	char buf[64] = {0};
	sprintf(buf, "AT+MT_PSBAT_WKWCHG_ATT=%d,%d\r\n", object_id, value);
	printf("%s", buf);
	atout->puts(buf);
}






