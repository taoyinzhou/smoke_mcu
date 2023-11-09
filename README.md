
1. # 目的

需求达成的期望是：MCU的开发者在取得和众提供的特定品类的AT协议文档以及此文档后，可以通过改造和众提供的MCU侧SDK代码，快速的与和众模组的AT指令协议完成对接

最终目的是能让所有品类共用一套相同的模版SDK，MCU开发者无需关心配网，及AT格式的拆装与解析，只需要对每种品类的特定的需要获取硬件信息的接口进行接口注册，即可完成MCU侧的开发

1. # AT指令介绍

1. 模块UART通信标准：
   1. 波特率  9600
   2. 数据位  8位
   3. 停止位  1位
   4. 校验位  0位
   5. 流控      无
2. 模块的AT指令集分为3大类：模块基础指令集、Matter基础指令集、Matter产品指令集。每条指令根据操作方式分为3种：查询命令（命令的查询或属性的读出）、执行命令（命令的执行或属性的写入）、通知消息。
3. 角色的定位：模块作为AT server，MCU作为AT client。client(MCU)负责发送AT指令和接收AT响应，server(模块)负责接收AT指令，开始执行对应的功能并发送AT响应。当某些状态或数值发生变化时以及产生事件时，server(模块)发送通知消息，client(MCU)接收和处理。
4. Matter基础指令集是Matter模块的一些通用命令，与业务无关。Matter产品指令集是以Matter现有支持的产品的标准物模型为核心，根据该产品类型使用的场景所制定的属性和命令的AT指令。
5. 模块的核心是本地的Matter物模型，网关或平台会读取模块物模型中的属性值。因此，在使用模块时，尽可能的保证MCU中的数值与模块中的属性值同步，每当状态或属性发生变化时，都要通过AT指令的形式，将最新的数值写入到对应属性中。

## AT指令格式

AT 指令以 “AT” 开始，代表 Attention，以CR LF(\r\n)为结尾。输入的每条命令都会返回执行结果，成功OK，出错 ERROR。

AT指令格式及规则：

| 指令类型 | 格式                               | 说明                                                         |
| -------- | ---------------------------------- | ------------------------------------------------------------ |
| 查询指令 | **AT+**<命令名称>[**=**<...>]?     | client端发送。查询该指令下的当前参数值。                     |
| 执行指令 | **AT+**<命令名称>[**=**<...>]      | client端发送。根据参数要求填入参数，并执行。如果是Matter物模型的属性操作则写入对应的属性值。 |
| 指令响应 | **+**<命令名称>**:**<结果>[,<...>] | server端发送。执行指令后，server端对查询指令或执行指令发送应答，返回执行结果。 |
| 主动通知 | **>**<命令名称>**:**<...>          | server端发送。当状态发送变化或某属性发生变化时，server端主动发送通知消息。 |

- 不是每条 AT 指令都具备上述四种类型的命令。
- 尖括号 < > 内的参数不可以省略。
- 方括号 [ ] 内的参数可以省略。
- 多个参数用 , 分隔。
- 所有参数凡是数值的，都是以十进制格式输入和输出

1. # 工程文件

1. ## main.c

该文件用于初始化SDK，启动各个监控线程，并且注册窗口发送函数

1. ## command.c

该文件实现的是设备接收模组下发的通知命令后的回调函数，以及串口调试命令的实现

1. ## data.c

该文件实现的是设备属性相关的功能。包括属性集合的抽象注册，烟感express值的计算、属性的变化检测、本地硬件变化上报、属性设置时的AT指令组装、

1. ## io.c

该文件抽象了设备的IO接口，由于不同硬件的实现可能并不相同，这个文件只是作为参考

1. ## test.c

实现测试属性变化上报的脚本功能

1. ## utils.c

该文件实现了对各个不同监控事件的thread生成和功能注册

1. ## analyze.c

该文件实现了对模组下发的AT指令的解析

1. # 接口约定

1. ## AT接收

```Shell
/*
*@函数：  command_parse
*@功能：  处理接收到的AT指令或是调试命令
*@参数：  buff    串口收到的数据
*@返回值：int8_t，成功返回0，失败返回-1
*/
int8_t command_parse(char *buff);
```

1. ## AT事件检测

```Shell
/*
*@函数：  attribute_change_detect_handler
*@功能：  根据object_id，完成对应终端的属性检查和更新
*@参数：  object_id
*@返回值：void
*/
void attribute_change_detect_handler(uint16_t object_id);
```

1. ## 硬件参数同步

```Shell
/*
*@函数：  attribute_set
*@功能：  设置一个属性的值
*@参数：  object_id    end point编号
*@参数：  att_num      参数编号
*@参数：  att_value    参数值
*@返回值：int8_t 成功返回0，失败返回-1
*/
int8_t attribute_set(uint16_t object_id, uint16_t att_num, uint16_t att_value);
```

1. ## AT发送

需要用户自己实现内部细节

```Shell
/*
*@函数：  attribute_set
*@功能：  MCU向AT窗口发送信息
*@参数：  send_data
*/
void uart_puts(char *send_data)
```

1. ## 使用示例

linux环境下的伪代码，MCU侧需根据实际使用环境替换接口

```Shell
void uart_puts(char* buf)
{
    //MCU侧实现，与硬件相关
    ....
}

//监控串口接收数据的线程
void* receive_func(void* arg) 
{
    while(1)
    {
        ...
        //uart接收到数据buf，处理模组过来的通知，下发的指令以及对MCU的回复
        command_parse(buf);
    }
}

int main(void)
{
    ...
    
    //创建一个处理接收到的串口数据的线程
    pthread_create(&thread_id, NULL, receive_func, NULL);
    
    while(1)
    {
        //一直监控着attribute的变化，计算event值，有变化时以AT指令上报
        attribute_change_detect_handler(1)；
    }
}

//硬件反馈有硬件变化
void irq_call()
{
    。。。
    //ep1, 检测到烟雾状态，同步attribute值
    attribute_set(1, AttNum_SmokeState, AlaState_Critical);
}
```

1. ## AT指令注册及回调说明

1. ### 属性回调

- 在头文件data.h中，有对设备属性的抽象定义

```Shell
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
```

data.c中，smoke_att_end1代表end point1的属性集合,开发在此注册属性检测的回调

```Shell
// 终端属性集合
static attribute_infor_st smoke_att_end1[] = 
{
        {"ExpressedState",        "MT_SMOKE_EXPSTATE_ATT", -1,                     -1,                ExpState_Normal,  ExpState_Normal, ExpState_Normal, event_allclear_handler},
        {"SmokeState",            "MT_SMOKE_STATE_ATT",    ExpState_SmokeAlarm,    PRI_SmokeAlarm,    AlaState_Normal,  AlaState_Normal, AlaState_Normal, event_smokealarm_handler},
        {"BatteryAlert",          "MT_SMOKE_BAT_ATT",      ExpState_BatteryAlert,  PRI_BatteryAlert,  AlaState_Normal,  AlaState_Normal, AlaState_Normal, event_lowbattery_handler},
        {"TestInProgress",        "MT_SMOKE_TIP_ATT",      ExpState_Testing,       PRI_Testing,       0,                0,               0,               event_selftestcomplete_handler},
        {"HardwareFaultAlert",    "MT_SMOKE_HF_ATT",       ExpState_HardwareFault, PRI_HardwareFault, 0,                0,               0,               event_hardwarefault_handler},
        {"EndOfServiceAlert",     "MT_SMOKE_EOS_ATT",      ExpState_EndOfService,  PRI_EndOfService,  Eos_Normal,       Eos_Normal,      Eos_Normal,      event_endOfservice_handler},
        {"ContaminationState",    "MT_SMOKE_CTMS_ATT",     -1,                     -1,                CtmState_Normal,  CtmState_Normal, CtmState_Normal, NULL},
        {"SmokeSensitivityLevel", "MT_SMOKE_SLV_ATT",      -1,                     -1,                Slv_Standard,     Slv_Standard,    Slv_Standard,    NULL},
        {"\0", "\0",-1,-1,0,0,0,NULL},   // 数组结束
};
```

1. ### 通知命令回调

- 在头文件command.c中，有针对模组发送的通知的回调定义

```Shell
static command_infor_st command_infor_notice_tbl[] = 
{
    {"MT_SMOKE_SLV_ATT",  command_notice_slvatt_handler},
    {"MT_SMOKE_TEST_CMD", command_notice_test_handler},
    {"SERVER",            command_notice_start_handler},
};
```

1. # 快速调试

这两种串口调试方式都存在多次使用后无法正常读写的现象，如果失败后需要重启虚拟机

1. ## 通过Linux程序模拟测试思维烟感协议

1. 安装socat `sudo apt install socat`
2. 创建第一对虚拟串口对 打开第一个窗口运行 `socat -d -d PTY PTY` 会显示创建的串口号：

```Plain
hoorii@hoorii-virtual-machine:~$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
2023/11/06 16:33:51 socat[4157] N PTY is /dev/pts/5
2023/11/06 16:33:51 socat[4157] N PTY is /dev/pts/6
2023/11/06 16:33:51 socat[4157] N starting data transfer loop with FDs [5,5] and [7,7]
```

1. 创建第二对虚拟串口对 打开第二个窗口运行 `socat -d -d PTY PTY` 会显示创建的串口号：

```Shell
hoorii@hoorii-virtual-machine:~$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
2023/11/06 16:34:01 socat[4158] N PTY is /dev/pts/8
2023/11/06 16:34:01 socat[4158] N PTY is /dev/pts/9
2023/11/06 16:34:01 socat[4158] N starting data transfer loop with FDs [5,5] and [7,7]
```

1. 运行程序 打开第三个窗口运行

```Plain
./run.sh /dev/pts/5 /dev/pts/8
```

1. echo AT指令测试 打开第四个窗口运行

```Plain
echo "AT\r\n" > /dev/pts/6
```

1. 打开第5个窗口，可以通过命令设置烟感属性

```Shell
//第一个参数是object_id
//第二个参数是属性编号
//第三个参数是设置属性值
//举例：设置object_id为1的ExpressedState的值为1
set 1 1 1 > /dev/pts/9
```

可以在第3个窗口看到运行时的日志打印，第四个窗口可以看到MCU发出的AT指令

1. ## 通过真实模组测试思维烟感协议

1. 将模组的AT引脚接到PC上的串口
2. 确认在串口在Linux虚拟机中创建的设备名，一般会是ttyUSB0

```Shell
ls /dev
```

1. 创建一对虚拟串口对 打开一个窗口运行 `socat -d -d PTY PTY` 会显示创建的串口号：

```Plain
hoorii@hoorii-virtual-machine:~$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
2023/11/06 16:33:51 socat[4157] N PTY is /dev/pts/5
2023/11/06 16:33:51 socat[4157] N PTY is /dev/pts/6
2023/11/06 16:33:51 socat[4157] N starting data transfer loop with FDs [5,5] and [7,7]
```

1. 运行程序 打开第三个窗口运行

```Plain
./run.sh /dev/ttyUSB0 /dev/pts/5
```

1. 打开第5个窗口，可以通过命令设置烟感属性

```Shell
//第一个参数是object_id
//第二个参数是属性编号
//第三个参数是设置属性值
//举例：设置object_id为1的ExpressedState的值为1
set 1 1 1 > /dev/pts/9
```

1. ttyUSB0侧的设备可以与linux中的程序进行AT指令的交互
