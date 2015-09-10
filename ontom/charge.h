/*
 * 充电控制描述结构
 * All rights reserved Hangzhou Power Equipment Co., Ltd.
 * author: LiJie
 * email:  bedreamer@163.com
 * 2014-08-22
*/

#ifndef _CHARGE_INCLUDED_H_
#define _CHARGE_INCLUDED_H_

struct charge_task;
extern struct charge_task *task;
struct MDATA_ACK;
struct bp_uart;

#ifndef u8
#define u8 unsigned char
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef u32
#define u32 unsigned int
#endif


// 无效时戳，初始化时默认赋值， 用于time_t默认值
#define INVALID_TIMESTAMP  0x00000000
// BMS 通信时的缓冲区
#define CAN_BUFF_SIZE         1024
#define BUFF_NR               CONFIG_SUPPORT_BMS_NR
typedef enum {
    // 0# 充电枪
    GUN_SN0 = 0,
    // 1# 充电枪
    GUN_SN1 = 1,
    // 2# 充电枪
    GUN_SN2 = 2,
    // 3# 充电枪
    GUN_SN3 = 3,
    // 默认充电枪
    GUN_DEFAULT = 0,
    // 无效枪
    GUN_INVALID = 344,
    // 未指定充电枪
    GUN_UNDEFINE = 177
}CHARGE_GUN_SN;

typedef enum {
    // 寻卡模式
    SEQ_FIND_CARD = 0,
    // 读扇区密码认证
    SEQ_SECTOR_RD_AUTH,
    // 写扇区密码认证
    SEQ_SECTOR_WR_AUTH,
    // 读取公开数据区
    SEQ_READ_PUBLIC_BLK,
    // 写入公开数据区
    SEQ_WRITE_PUBLIC_BLK,
    // 读取私密数据区1
    SEQ_READ_PRIVATE_BLK1,
    // 读取私密数据区2
    SEQ_READ_PRIVATE_BLK2,
    // 读取私密数据区3
    SEQ_READ_PRIVATE_BLK3,
    // 读取私密数据区4
    SEQ_READ_PRIVATE_BLK4,
    // 读取私密数据区5
    SEQ_READ_PRIVATE_BLK5,
    // 读取私密数据区6
    SEQ_READ_PRIVATE_BLK6,
    // 读取私密数据区7
    SEQ_READ_PRIVATE_BLK7,
    SEQ_INVALID
}QUERY_STAT;

typedef enum {
    // 分离的，未连接
    GUN_CONN_DETACHED = 0,
    // 连接保护
    GUN_CONN_PROTECTED = 1,
    // 连接异常
    GUN_CONN_EXCEPTION = 2,
    // 已连接
    GUN_CONN_CONNECTIVE=3
}GUN_CONN_STAT;
// 充电计费模式
typedef enum {
    // 无效充电模式，默认值
    BILLING_MODE_INVALID    =0x00,
    // 自动充电
    BILLING_MODE_AS_AUTO    =0x01,
    // 按金额充电
    BILLING_MODE_AS_MONEY   =0x02,
    // 按时间充电
    BILLING_MODE_AS_TIME    =0x03,
    // 按容量充电
    BILLING_MODE_AS_CAP     =0x04,
    // 自由充电，不需要刷卡
    BILLING_MODE_AS_FREE    =0x05
}BILLING_MODE;

// 交易方式
typedef enum {
    // 无交易
    DEAL_NONE = 0,
    // 卡片式交易
    DEAL_USE_CARD = 1,
    // 网络式交易
    DEAL_USE_NET  = 2,
    // 卡片和网络同时存在
    DEAL_USE_BOTH
}DEAL_METHORD;

// 充电任务 状态
typedef enum {
    // 无效任务状态
    CHARGE_STAT_INVALID             =0x00,
    // 空闲等待,界面停止于充电模式选择界面
    CHARGE_STAT_TRIGER_PEDDING      =0x01,
    // 刷卡准备, 于自动模式下已经刷卡，或者手动选择了充电计费模式
    //CHARGE_STAT_PREPARE      =0x02,
    // 充电确认, 刷卡完成，进行充电确认等待
    CHARGE_STAT_CONFIRM_PEDDING     =0x03,
    // BMS 连接等待, 已经确认充电
    CHARGE_STAT_WAIT_BMS            =0x04,
    // BMS 已经连接，我收拾别完成，进行充电参数配置
    CHARGE_STAT_READY               =0x05,
    // 充电阶段
    CHARGE_STAT_CHARGING            =0x06,
    // 充电结束阶段, 断开接触器
    CHARGE_STAT_DONE                =0x07,
    // 计费阶段, 等待刷卡付费阶段
    CHARGE_STAT_BILLING_PEDDING     =0x08,
    // 充电任务退出阶段，日志记录，清理
    CHARGE_STAT_EXIT                =0x09,
    // 充电任务被终止
    CHARGE_STAT_ABORT               =0x0A,
    // 充电异常退出
    CHARGE_STAT_EXCEPTION           =0xFF
}CHARGE_TASK_STAT;

// 充电阶段定义
typedef enum {
    // 充电状态无效，可能是在初始化或者析构
    CHARGE_STAGE_INVALID      =0x00,
    // 充电握手
    CHARGE_STAGE_HANDSHACKING =0x01,
    // 充电配置
    CHARGE_STAGE_CONFIGURE    =0x02,
    // 充电
    CHARGE_STAGE_CHARGING     =0x03,
    // 充电结束
    CHARGE_STAGE_DONE         =0x04,
    // 任意阶段
    CHARGE_STAGE_ANY          =0x05
}CHARGE_STAGE;

// 充电阶段变更事件
typedef enum {
    // 充电状态变为无效状态
    STAGE_CHANGE_2_INVALID     = 0x00,
    // 充电状态变为握手阶段
    STAGE_CHANGE_2_HANDSHAKING = 0x01,
    // 充电状态变为配置阶段
    STAGE_CHANGE_2_CONFIGURE   = 0x02,
    // 充电阶段变为充电阶段
    STAGE_CHANGE_2_CHARGING    = 0x03,
    // 充电阶段变为充电完成阶段
    STAGE_CHANGE_2_DONE        = 0x04,
    // 充电阶段变为充电正常退出阶段
    STAGE_CHANGE_2_EXIT        = 0x05,
    // 充电阶段变为中止充电推出阶段
    STAGE_CHANGE_2_ABORT       = 0x06
}CHARGE_STAGE_CHANGE_EVENT;
// BMS CAN通信状态定义
typedef enum {
    // 无效模式
    CAN_INVALID               =0x00,
    // 普通模式
    CAN_NORMAL                =0x01,
    // 连接管理读模式, 多数据包读
    CAN_TP_RD                 =0x02,
    // 连接管理写模式，多数据包写，当前的协议标准没有用到
    CAN_TP_WR                 =0x04
}CAN_BMS_STATUS;

// CAN 传输链接状态
typedef enum {
    // 链接请求发送
    CAN_TP_RTS = 0x10,
    // 链接准备发送
    CAN_TP_CTS = 0x20,
    // 数据发送中
    CAN_TP_TX  = 0x30,
    // 数据接收中
    CAN_TP_RX  = 0x40,
    // 链接发送完成
    CAN_TP_ACK = 0x50,
    // 链接发送中止
    CAN_TP_ABRT= 0x60
}CAN_TP_STATUS;

// 通信管理单元状态
typedef enum {
    // 管理但愿正在初始化
    COMM_M_INIT = 0,
    // 管理单元正在执行
    COMM_M_WORKING = 1,
    // 管理单元正在停止
    COMM_M_STOPPING = 2,
    // 管理单元已经终止
    COMM_M_DIED = 3
}COMM_M_STRUCT_STATUS;

// 充电机控制状态
typedef enum {
    // 空闲
    CHARGER_IDLE,
    // 初始化
    CHARGER_INIT,
    // 充电工作中
    CHARGER_WORK
}CHARGER_STATUS;

// 充电作业状态
typedef enum {
    // 作业状态为空
    JOB_IDLE = 0x00,
    // 作业状态正在设置
    JOB_SETTING,
    // 正在等待前驱作业完成
    JOB_WAITTING,
    // 系统扫描测试
    JOB_SCANNING,
    // 12V 辅助电源测试
    JOB_12V_ASSIT_TEST,
    // 24V 辅助电源测试
    JOB_24V_ASSIT_TEST,
    // 作业开始执行
    JOB_START,
    // 作业就绪等待
    JOB_STANDBY,

    // 作业正在执行
    JOB_WORKING,
    // 作业因故暂停
    JOB_ERR_PAUSE,
    // 人工暂停作业
    JOB_MAN_PAUSE,
    // 作业从暂停状态恢复
    JOB_RESUMING,
    // 作业中止
    JOB_ABORTING,
    // 作业完成
    JOB_DONE,
    // 作业退出，正在进行清理工作
    JOB_EXITTING,

    // 作业销毁，等待其他线程引用退出
    JOB_DETACHING,
    // 作业销毁完成，等待主程序删除资源
    JOB_DIZZY
}JOBSTATUS;

typedef enum {
    // 无效
    PRIVATE_INVALID = 0x00,
    // 已经使用
    PRIVATE_BUSY    = 0x01,
    // 闲置等待
    PRIVATE_STANDBY = 0x02
}PRIVATE_STATUS;

// 双辅助电源投切方式
typedef enum {
    // 手工选择
    ASSIT_SELECT_MANUAL,
    // 自动投切
    ASSIT_SELECT_AUTO
}DOUBLE_ASSIT_SELECT_METHORD;

// 提交命令
typedef enum {
    // 创建任务
    COMMIT_CMD_FORK,
    // 完成任务
    COMMIT_CMD_DONE,
    // 中止任务
    COMMIT_CMD_ABORT
}COMMIT_CMD;
// 充电模式
typedef enum {
    // 自动充电，带BMS管理
    CHARGE_AUTO,
    // 手动恒压充电，不需要BMS
    CHARGE_BV,
    // 手动恒流充电，不需要BMS
    CHARGE_BI
}CHARGE_MODE;

typedef enum {
    // 暂停作业命令
    CMD_JOB_MAN_PAUSE = 0x00,
    // 恢复作业
    CMD_JOB_RESUME = 0x01,
    // 中止作业
    CMD_JOB_ABORT
}JOB_FLAGS;

// 故障恢复原因
typedef  enum {
    // 故障消除后恢复
    REASON_NORMAL,
    // 系统做初始化时恢复前一次运行时的故障
    REASON_MANAUL
}ERR_RECOVER_REASON;

// 充电系统类型
typedef enum {
    SYSTEM_FENTISHI, // 分体式
    SYSTEM_YITISHI   // 一体式
}SYSTEM_TYPE;

// 模块类型
typedef enum {
    MODEL_AN10680,
    MODEL_AN20750,
    MODEL_AN30750,
    MODEL_AN40750,
    MODEL_INCREASE,
    MODEL_UNKOWN
}MODULE_MODEL;

// UI 界面
typedef enum {
    UI_PAGE_OTHER,
    UI_PAGE_MAIN,
    UI_PAGE_JOBS_POOL,
    UI_PAGE_JOBS
}UI_PAGE;

// 电表定义
typedef enum {
    KWH_METER_NONE,
    KWH_METER_HUALI_3AC,
    KWH_METER_YADA_DC
}KWH_METER;

/* 系统信号定义
 * 前 128个信号为系统标记
 */
typedef enum {
    FLAG_BEGIN,
    // 扩展测量值刷新标记
    F_MEASURE_DATA_NEW,

    // BMS 结构体初始化完成
    F_BMS_INIT_DONE,
    // 创建BMS写线程完成
    F_BMS_FORK_WRITE_DONE,
    // 创建BMS读线程完成
    F_BMS_FORK_READ_DONE,

    // 系统需要刷卡扣费，用户站不用扣费
    F_NEED_BILLING,
    // 扣费完成
    F_BILLING_DONE,
    // 扣费超时
    F_BILING_TIMEOUT,
    // 手动充电需要辅助电源
    F_MANUL_CHARGE_NEED_ASSITER,
    // 需要进行电能计费
    F_NEED_KWH_METER,
    // 通讯模式, 1: 在线式， 0: 离线式
    F_SYSTEM_COMM_MODE,
    // 需要进行账户认证,1: 需要进行账户认证， 0: 不需要进行账户认证
    F_NEED_AUTH,

    // {{
    // 1#枪充电
    F_GUN1_CHARGE,
    // 2#枪充电
    F_GUN2_CHARGE,

    // 1#枪使用12V辅助电源
    F_GUN1_ASSIT_12V,
    // 1#枪使用24V电源
    F_GUN1_ASSIT_24V,
    // 2#枪使用12V辅助电源
    F_GUN2_ASSIT_12V,
    // 2#枪使用24V电源
    F_GUN2_ASSIT_24V,

    // 充电枪1电压抬高完成
    F_VOL1_SET_OK,
    // 充电枪2电压抬高完成
    F_VOL2_SET_OK,
    //

    //{{{ 状态标记
    // BMS 已经识别
    F_BMS_RECOGNIZED,
    // 已经发送车辆已经识别数据包
    F_VEHICLE_RECOGNIZED,
    // 已经收到BMS准备充电报文
    F_BMS_READY,
    // 充电机充电准备完成
    F_CHARGER_READY,
    // 接收到电池充电总状态报文
    F_PCK_BAT_STATUS,
    // 接收到BMS终止充电报文
    F_PCK_BMS_TRM,
    // 充电机主动中止充电报文
    F_PCK_CHARGER_TRM,
    // 接收到BSD
    F_PCK_RX_BSD,
    // 已经发送CSD
    F_PCK_TX_CSD,
    // 已经发送CST
    F_PCK_TX_CST,
    // 接收电池充电总状态报文超时
    F_PCK_BAT_STATUS_TIMEOUT,

    // 触发充电刷卡完成
    F_CARDING_TRIGER,
    // 确认充电刷卡完成
    F_CARDING_CONFIRM,
    // 充电结束刷卡完成
    F_CARDING_SETTLE,
    // 充电指示灯
    F_CHARGE_LED,
    // 市电指示
    F_AC_LED,

    // 系统人为条件可以充电
    F_MANUAL_CHARGE_ALLOW, // 人为禁止充电
    // 系统硬件条件可以充电
    F_SYSTEM_CHARGE_ALLOW, // 系统无故障, 关键故障
    // 直流输出总开关闭合状态
    F_DC_OUTPUT_SWITCH_STATUS,
    // 1#枪物理连接完成状态
    F_GUN_1_PHY_CONN_STATUS,
    // 1#枪输出开关状态
    F_GUN_1_OUTPUT_SWITCH_STATUS,
    // 1#枪辅助电源开关状态
    F_GUN_1_ASSIT_PWN_SWITCH_STATUS,
    // 2#枪物理连接完成状态
    F_GUN_2_PHY_CONN_STATUS,
    // 2#枪输出开关状态
    F_GUN_2_OUTPUT_SWITCH_STATUS,
    // 2#枪辅助电源开关状态
    F_GUN_2_ASSIT_PWN_SWITCH_STATUS,
    // }}}

    // {{ 输出控制命令状态
    // 直流输出总开关打开
    CMD_DC_OUTPUT_SWITCH_ON  = 0x0080,
    // 1#枪辅助电源打开
    CMD_GUN_1_ASSIT_PWN_ON,
    // 1#枪输出开关打开
    CMD_GUN_1_OUTPUT_ON,
    // 1#枪电子锁锁闭
    CMD_GUN_1_LOCK_ON,
    // 2#枪辅助电源打开
    CMD_GUN_2_ASSIT_PWN_ON,
    // 2#枪输出开关打开
    CMD_GUN_2_OUTPUT_ON,
    // 2#枪电子锁锁闭
    CMD_GUN_2_LOCK_ON,
    // }}
    // {{ 充电作业操作命令
    // 中止当前充电作业
    //CMD_JOB_ABORT,
    // 暂停当前充电作业
    //CMD_JOB_MAN_PAUSE,
    // }}
    //{{ 模块操作
    CMD_MODULE_ON,
    CMD_MODULE_OFF,
    //}}

    // {{ 采样盒校准
    // 校准母线1电压
    CMD_JIAOZHUN_BUS1_V,
    // 校准母线2电压
    CMD_JIAOZHUN_BUS2_V,
    // 校准电池电流
    CMD_JIAOZHUN_BAT_I,
    // }}

    // {{
    // 点亮照明灯板
    CMD_LIGHT_UP,
    CMD_TEST_GUN1_UP,
    CMD_TEST_GUN2_UP,
    CMD_TEST_GUN1ASSIT_UP,
    CMD_TEST_GUN2ASSIT_UP,
    CMD_TEST_GUN1_LOCK_UP,
    CMD_TEST_GUN2_LOCK_UP,
    // }}

    // {{卡操作
    CMD_CARD_FORMAT,
    CMD_CARD_SET,
    // }}

    // {{{ 故障标记
    // BMS通信故障
    S_BMS_COMM_DOWN,
    S_BACKCOMM_DOWN,
    // }}}

    // {{ 设备安装标志
    F_INSTALL_CARDREADER,
    F_TEST_CARDREADER,
    F_INSTALL_CARDREADER_DONE,
    F_INSTALL_CARDREADER_FAILE,

    F_INSTALL_METER,
    F_TEST_METER,
    F_INSTALL_METER_DONE,
    F_INSTALL_METER_FAILE,
    // }}
    FLAG_END,

    //总故障, 参照文档 -- 充电桩故障对照表
    S_ERROR = 0x100,
    S_BUS_0_VHI,
    S_BUS_0_VLO,
    S_BUS_0_SHORT,
    S_BUS_1_VHI,
    S_BUS_1_VLO,
    S_BUS_1_SHORT,
    S_BAT_0_VHI,
    S_BAT_0_VLO,
    S_BAT_0_SHORT,
    S_BAT_0_REVERT,
    S_BAT_0_INSTITUDE,
    S_BAT_0_IHI,
    S_BAT_1_VHI,
    S_BAT_1_VLO,
    S_BAT_1_SHORT,
    S_BAT_1_REVERT,
    S_BAT_1_INSTITUDE,
    S_BAT_1_IHI,
    S_ASSIT_PWN_ERR,
    S_TEMP_HI,
    S_TEMP_LO,
    S_WET_HI,
    S_WET_LO,
    S_DC_OUTPUT_RD,
    S_DC_OUTPUT_TRIP,
    S_DC_OUTPUT_0_TRIP,
    S_DC_OUTPUT_1_TRIP,
    S_FLQ_ERROR,
    S_CHARGE_GROUP_ERR,
    S_CHARGE_M_1_ERR,
    S_CHARGE_M_2_ERR,
    S_CHARGE_M_3_ERR,
    S_CHARGE_M_4_ERR,
    S_CHARGE_M_5_ERR,
    S_CHARGE_M_6_ERR,
    S_CHARGE_M_7_ERR,
    S_CHARGE_M_8_ERR,
    S_CHARGE_M_9_ERR,
    S_CHARGE_M_10_ERR,
    S_CHARGE_M_11_ERR,
    S_CHARGE_M_12_ERR,
    S_CHARGE_M_13_ERR,
    S_CHARGE_M_14_ERR,
    S_CHARGE_M_15_ERR,
    S_CHARGE_M_16_ERR,
    S_CHARGE_M_17_ERR,
    S_CHARGE_M_18_ERR,
    S_CHARGE_M_19_ERR,
    S_CHARGE_M_20_ERR,
    system_reserve0,
    system_reserve1,
    system_reserve2,
    system_reserve3,
    system_reserve4,
    S_AC_INPUT_LO,
    S_AC_INPUT_HI,
    S_AC_MISS,
    system_reserve8,
    system_reserve9,
    S_AC_SWITCH_TRIP,
    S_HALT,
    system_reserve12,
    system_reserve13,
    system_reserve14,
    system_reserve15,
    system_reserve16,
    system_reserve17,
    system_reserve18,
    system_reserve19,
    system_reserve20,
    S_CHARGER_COMM_DOWN,
    S_CHARGER_YX_1_COMM_DOWN,
    S_CHARGER_YX_2_COMM_DOWN,
    S_MEASURE_1_COMM_DOWN,
    S_CONVERT_BOX_COMM_DOWN,
    S_MEASURE_2_COMM_DOWN,
    S_CARD_READER_COMM_DOWN,
    S_KWH_METER_COMM_DOWN,
    system_reserve24,
    system_reserve25,
    system_reserve26,
    system_reserve27,
    system_reserve28,
    system_reserve29,
    system_reserve30,
    S_GUN1_DROPED,
    S_GUN2_DROPED,
    system_reserve33,
    system_reserve34,
    system_reserve35,
    system_reserve36,
    system_reserve37,
    system_reserve38,
    system_reserve39,
    system_reserve40,
    system_reserve41,
    system_reserve42,
    system_reserve43,
    system_reserve44,
    system_reserve45,
    S_END
}ONTOM_FLAG_SINGLE;

// 异常定义
typedef enum {
    // @ .DEF::EXCPT_UART_SELECT_EXCEPTION
    // select 函数出现异常
    EXCPT_UART_SELECT_EXCEPTION,
}ONTOM_EXCEPTIONS;

// 作业信号
typedef enum {
    J_BEGIN = 0x00,

    // 1: 自动充电， 0: 手动充电
    JF_AUTO_CHARGE = 0x01,
    // 1: 充电枪已连接，0: 充电枪未连接
    JF_GUN_OK,
    // 1: 辅助电源已经上电，0: 辅助电源未上电
    JF_ASSIT_PWR_ON,
    // 1: 24V辅助电源，0: 不使用辅助电源
    JF_ASSIT_12V,
    // 1: 使用24V辅助电源, 0: 不使用辅助电源
    JF_ASSIT_24V,
    // 1: 需要辅助电源，0: 不需要辅助电源
    JF_NEED_ASSIT_PWR,
    // 1: 电子锁已经闭合，0: 电子锁未闭合
    JF_LOCK_ON,
    // 1: BMS驱动已经绑定，0: 未绑定
    JF_BMS_DRV_BINDED,
    // 1: BMS允许充电，0：BMS不允许充电
    JF_CHARGE_BMS_ALLOW,
    // 1: 已经发送计费请求, 0: 未发送计费请求
    JF_BILLING_TX,
    // 1: 作业计费完成， 0: 作业计费未完成
    JF_BILLING_DONE,
    // 1: 计费超时， 0: 计费未超时
    JS_BILLING_TIMEOUT,
    // 1: 作业中止，0: 作业不中止
    JF_CMD_ABORT,
    // 1: 作业退出, 0：作业未退出
    JF_JOB_EXIT,
    // 1: 作业销毁, 0：未销毁作业
    JF_DESTROY,

    // {{ BMS 数据包事件
    // 1: 已经接收到BRM，0：还未接收到BRM
    JF_BMS_RX_BRM,
    // 1: 接收BRM超时，0: 正常
    JS_BMS_RX_BRM_TIMEOUT,
    // 1: 已经发送CRM，0: 还未发送CRM
    JF_BMS_TX_CRM,

    // 1: 已经接收到BCP, 0: 还未接收到BCP
    JF_BMS_RX_BCP,
    // 1: 接收BCP超时，0：正常
    JS_BMS_RX_BCP_TIMEOUT,
    // 1: 已经接收到BRO, 0: 还未接收到BRO
    JF_BMS_RX_BRO,
    // 1: 接收BRO超时，0：正常
    JS_BMS_RX_BRO_TIMEOUT,
    // 1: 已经发送CTS，0:还未发送CTS
    JF_BMS_TX_CTS,
    // 1: 已经发送CML，0:还未发送CTS
    JF_BMS_TX_CML,
    // 1: 已经发送CRO，0:还未发送CTS
    JF_BMS_TX_CRO,

    // 1: 已经接收到BCL, 0: 还未接收到BCL
    JF_BMS_RX_BCL,
    // 1: 接收BCP超时，0：正常
    JS_BMS_RX_BCL_TIMEOUT,
    // 1: 已经接收到BCS, 0: 还未接收到BCS
    JF_BMS_RX_BCS,
    // 1: 接收BRO超时，0：正常
    JS_BMS_RX_BCS_TIMEOUT,
    // 1: 已经接收到BSM, 0: 还未接收到BSM
    JF_BMS_RX_BSM,
    // 1: 接收BSM超时，0：正常
    JS_BMS_RX_BSM_TIMEOUT,
    // 1: 已经接收到BMV, 0: 还未接收到BMV
    JF_BMS_RX_BMV,
    // 1: 接收BMV超时，0：正常
    JS_BMS_RX_BMV_TIMEOUT,
    // 1: 已经接收到BMT, 0: 还未接收到BMT
    JF_BMS_RX_BMT,
    // 1: 接收BMT超时，0：正常
    JS_BMS_RX_BMT_TIMEOUT,
    // 1: 已经接收到BST, 0: 还未接收到BST
    JF_BMS_RX_BST,
    // 1: 接收BST超时，0：正常
    JS_BMS_RX_BST_TIMEOUT,
    // 1: 已经发送CTS，0:还未发送CTS
    JF_BMS_TX_CCS,
    // 1: 已经发送CST，0:还未发送CST
    JF_BMS_TX_CST,

    // 1: 已经接收到BSD, 0: 还未接收到BSD
    JF_BMS_RX_BSD,
    // 1: 接收BSD超时，0：正常
    JS_BMS_RX_BSD_TIMEOUT,
    // 1: 已经发送CSD，0:还未发送CSD
    JF_BMS_TX_CSD,

    // BMS通信故障
    JS_BMS_COMM_ERR,
    // }}

    //{{ BMS 控制逻辑标记
    // 1: BMS 已经识别， 0: BMS未识别
    JF_BMS_BRM_OK,
    // 1: 充电机已经识别，0: 充电机未识别
    JF_CHG_CRM_OK,

    // 1: BMS 已经准备好充电，0: BMS未准备好充电
    JF_BMS_BRO_OK,
    // 1: 充电机已经准备好充电，0: 充电机未准备好充电
    JF_CHG_CRO_OK,

    // 1: BMS 中止充电，0: BMS 未中止充电
    JF_BMS_TRM_CHARGE,
    // 1: 充电机中止充电，0: 充电机未中止充电
    JF_CHG_TRM_CHARGE,
    //}}

    // 作业退出资源销毁标记
    // {{
    // 心跳定时器
    JR_HEART_BEAT_TIMER,
    // 传输定时器
    JR_TP_CONN_TIMER,
    // 读线程引用
    JR_RD_THREAD_REF,
    // 写线程引用
    JR_WR_THREAD_REF,
    // }}
    
    // 已经上传退出状态
    JF_UPLOAD_EXIT_STATUS,
    // BMS 请求切换主副系统
    JF_NEED_SWITCH_SYSTEM,

    J_END
}JOB_FLAG_SINGLE;

// 硬件配置信息
typedef enum {
    // 交流电能表存在
    HW_AC_KWH_METER = 0,
    // 直流电能表存在
    HW_DC_KWH_METER,
    // 周立功读卡器存在
    HW_ZLG600SP_CARD_READER,
    // 故障蜂鸣器存在
    HW_ERR_BUZZER,
    // 操作提示蜂鸣器存在
    HW_OP_BUZZER,
    // 操作日志模块存在
    SW_LOG_MODULE,

    // HW_SW_CMD_BEGIN
    HW_SW_CMD_BEGIN = 32,
    HW_CMD_BUZZER_BEEP_OK,
    HW_CMD_BUZZER_BEEP_ERR,
    HW_CMD_BUZZER_BEEP_STARTUP,

    // HW_SW_CMD_END
    HW_SW_CMD_END
}ONTOM_HW_CONFIG;

typedef enum {
    /*
     * 系统日志，包含： 开关机，更新，设置
     */
    LOG_SYS = 0x0001,
    /*
     * 命令日志，包含： 接触器初始化、合闸分闸，输出限压限流，刷卡操作
     */
    LOG_CMD = 0x0002,
    /*
     * 作业日志，包含： 作业提交，创建，执行，暂停，中止，销毁过程，计费
     */
    LOG_JOB = 0x0004,
    /*
     * 故障日志，包含：通信故障，硬件故障，软件故障
     */
    LOG_FAULT = 0x0008,


    // 立即刷新
    LOG_FLUSH = 0x8000
}SYSLOG;

#pragma pack(1)
/* 卡信息
 */
struct user_card {
    // 触发任务时的卡号
    char triger_card_sn[64];
    struct {
        unsigned char id[16];
        unsigned char sector_num;
        union {
            unsigned char buff[16];
            struct {
                unsigned int magic;           /*奥能魔数 'ONPW', Only-Power 0x4F4E5057 */
                unsigned char remain_money[3]; /*余额， 范围 0.00 ~ 167772.15 */
                unsigned char remain_sum;    /*余额校验位 */
                unsigned char passwd_code[3];  /*6 位BCD 码密码*/
                unsigned char passwd_sum;    /*密码校验和 */
                unsigned short reserved;
                unsigned char unuesed;
                unsigned char sum;            /*校验和 */
            }data;
        }sector_4;
    }card;
};

// 用户认证结构
struct user_auth {
    // 认证序列号, 每认证一次自加1
    unsigned int auth_id;
    // 认证请求时戳
    time_t auth_request_tsp;
    // 发送认证时戳
    time_t auth_tx_tsp;
    // 认证返回时戳
    time_t auth_return_tsp;
    // 认证超时时长, 默认5秒
    unsigned int ttl;
    // 动作次数
    unsigned int verbose;

    // 卡类型
    unsigned char card_type;
    // 卡序号
    char card_sn[32];
    // 用户输入密码
    char user_input_passwd[32];

    // 认证返回结果
    unsigned char auth_return_value;
    // 返回提示
    const char *auth_msg;
    // 是否允许充电
    int auth_allow_charge;
    // 账户余额
    unsigned int auth_return_remain;
    // 账户剩余电量
    unsigned int auth_return_kwh;
};

// 充电请求
struct charge_request {
    // 发送请求的时戳
    time_t tx_request_tsp;
    // 请求时戳
    time_t request_tsp;
    // 充电枪
    CHARGE_GUN_SN gun;
    // 车辆编号
    char vin[18];
    // 车牌号
    char licence[9];
    // 卡类型
    unsigned char card_type;
    // 账户
    char account[33];
    // 密码
    char passwd[17];
    // 充电模式
    unsigned char service_mode;
    // 充电参数
    unsigned short service_param;

    // 响应时戳
    time_t ack_tsp;
    // 操作代码
    unsigned char result;
    // 等待充电
    unsigned char action;
    // 预扣金额
    double pre_pay;
    // 账户余额
    double remian;
    // 允许充电时长
    unsigned int allow_time;
    // 允许充电电量
    unsigned short allow_kwh;
    // 交易流水号
    unsigned char billing_sn[33];
};

/*
 * 综合采样盒通信数据定义
 */
struct MDATA_ACK {
    // 数据包起始魔数
    unsigned char magic[4];
    // 地址
    unsigned char addr;
    // 功能吗
    unsigned char op;
    // 载荷长度
    unsigned char len;

    unsigned short VinKM0; //母线0电压 实际电压*10
    unsigned short VinKM1;//母线1电压 实际电压*10
    unsigned short VinBAT0;//电池0组电压，实际电压*10
    unsigned short VinBAT1;//电池1组电压，实际电压*10
    unsigned short IoutBAT0; //电池0组电流，实际电流*10
    unsigned short IoutBAT1; //电池1组电流，实际电流*10
    unsigned short VinBAT0RESP;//电池0组正对地电阻，实际电阻*10（K）
    unsigned short VinBAT1RESP;//电池1组正对地电阻，实际电阻*10（K）
    unsigned short VinBAT0RESN;//电池0组负对地电阻，实际电阻*10（K）
    unsigned short VinBAT1RESN;//电池1组负对地电阻，实际电阻*10（K）
    unsigned short Temp ;//当前温度，实际温度*10
    unsigned short Humi; //当前湿度，实际湿度*10
    unsigned short Reserve2;// 预留
    unsigned short Reserve3; //预留
    unsigned char Ver; //软件版本号， 如10 表示 Ver= 1.0;  数据一共29字节


    unsigned char Flag_prtc1;   //充电断开类保护状态字节1：（同步状态）
    //BIT0：母线0过压状态位；0 = 无故障，1 = 母线过压；
    //BIT1：母线0欠压状态位；0 = 无故障，1 = 母线欠压；
    //BIT2：母线0短路状态位；0 = 无故障，1 = 母线短路；
    //BIT3：母线1过压状态位；0 = 无故障，1 = 母线过压；
    //BIT4：母线1欠压状态位；0 = 无故障，1 = 母线欠压；
    //BIT5：母线1短路状态位；0 = 无故障，1 = 母线短路；
    //剩余bit保留

    unsigned char Flag_prtc2;   //充电断开类保护状态字节2：（同步状态）
    //BIT0：电池0组过压状态位；0 = 无故障，1 = 电池过压；
    //BIT1：电池0组欠压状态位：0 = 无故障，1 = 电池欠压；
    //BIT2：电池0组短路状态位：0 = 无故障，1 = 电池短路；
    //BIT3：电池0组反接状态位；0 = 无故障，1 = 电池反接；
    //BIT4：电池0组绝缘状态位；0 = 无故障，1 = 电池绝缘故障；
    //BIT5：电池0组过流状态位；0 = 无故障，1 = 电池过流；
    //剩余bit保留

    unsigned char Flag_prtc3;   //充电断开类保护状态字节3：（同步状态）
    //BIT0：电池1组过压状态位；0 = 无故障，1 = 电池过压；
    //BIT1：电池1组欠压状态位：0 = 无故障，1 = 电池欠压；
    //BIT2：电池1组短路状态位：0 = 无故障，1 = 电池短路；
    //BIT3：电池1组反接状态位；0 = 无故障，1 = 电池反接；
    //BIT4：电池1组绝缘状态位；0 = 无故障，1 = 电池绝缘故障；
    //BIT5：电池1组过流状态位；0 = 无故障，1 = 电池过流；
    //剩余bit保留

    unsigned char Flag_prtc4;   //充电断开类保护状态字节4：（同步状态）
    //BIT0：辅助电源状态位；0 = 无故障，1 = 辅助电源异常；
    //BIT1..2：温度状态位；0 0= 无故障，01 = 温度过高，10= 温度过低
    //BIT3..4：湿度状态位；0 0= 无故障，01 = 湿度过大，10=湿度过低
    //剩余bit保留

    unsigned char Flag_prtc5;   //充电断开类保护状态字节5：保留（同步状态）

    unsigned char Flag_prtc6;   //充电断开类保护状态字节4：（异步状态）
    //BIT0：总直流输出熔断器状态位：0 = 无故障，1 = 熔断；
    //BIT1：总直流输出跳闸状态位：0 = 无故障，1 = 跳闸；
    //BIT2：充电1路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT3：充电2路输出跳闸状态位；0 = 无故障，1 = 跳闸；
    //BIT4：防雷器状态位；0 = 无故障，1 = 防雷器故障；
    //BIT5：交流输入跳闸；0 = 无故障，1 = 跳闸；
    //BIT6：急停状态位；0 = 无故障，1 = 急停；
    //剩余bit保留

    unsigned char Flag_prtc7;   //充电断开类保护状态字节5：（异步状态）保留

    unsigned char Flag_run1;   //当前运行状态字节1：
    //BIT 0: 交流输入是否合闸： 0 = 分；1 = 合； //一般情况常1
    //BIT 1：加热状态：    0 = 未加热；1 = 加热；
    //BIT 2：风扇状态：  0 = 未启动； 1 = 启动；
    //BIT 3：总直流输出是否合闸：0 =分；1 =合；
    //BIT 4：充电枪1合闸状态： 0 = 分； 1 = 合；
    //BIT 5..6：充电枪1连接状态：00= 未连接； 1 1= 连接；01=连接保护状态；10= 连接异常
    //BIT 7：充电枪1通信辅助电源合闸状态： 0 = 分； 1 = 合；

    unsigned char Flag_run2;   //当前运行状态字节2：
    //BIT 0：充电枪2合闸状态：0 = 分； 1 = 合；
    //BIT 1..2：充电枪2连接状态：00 = 未连接； 11 = 连接；01=连接保护状态；10= 连接异常
    //BIT 3：充电枪2通信辅助电源合闸状态： 0 = 分； 1 = 合；

    unsigned char Flag_run3;   //当前运行状态字节3：保留 状态一共10字节
    unsigned char unused;

    unsigned short crc;
};

// 下发数据包
struct MDATA_QRY {
    // 数据包起始魔数
    unsigned char magic[5];
    // 地址
    unsigned char addr;
    // 载荷长度
    unsigned char len;
#if 1
    // 直流总开关输出闭合，0：断开，1：闭合
    unsigned char dc_output_hezha:1;
    // 1#充电枪辅助电源上电，0：断电，1：上电
    unsigned char gun_1_assit_power_on:1;
    // 1#充电枪输出开关合闸，0：分闸，1：合闸
    unsigned char gun_1_output_hezha:1;
    // 2#充电枪辅助电源上电，0：断电，1：上电
    unsigned char gun_2_assit_power_on:1;
    // 2#充电枪输出开关合闸，0：分闸，1：合闸
    unsigned char gun_2_output_hezha:1;
    unsigned char cmd_rsv:3;
#else
    unsigned char cmd_rsv;
#endif
    unsigned char cmd_copy;
    unsigned char rsvs[14];

    unsigned short crc;
};
typedef enum {
    // 端口未使用
    OUT_NOT_USED,
    // 1枪输出开关
    OUT_GUN1_DC_OUTPUT,
    // 1枪辅助电源
    OUT_GUN1_ASSIT,
    // 1枪电子锁
    OUT_GUN1_LOCK,
    // 2枪输出开关
    OUT_GUN2_DC_OUTPUT,
    // 2枪辅助电源
    OUT_GUN2_ASSIT,
    // 2枪电子锁
    OUT_GUN2_LOCK,
    // 系统故障状态
    OUT_SYS_ERR_STAT,
    // 系统充电状态
    OUT_SYS_CHARGE_STAT
}SIMPLE_BOX_OUT_PORT;

// 握手阶段
// 充电机辨识报文
struct pgn256_CRM {
    // 充电机辨识结果 @ enum recognize_result
    u8 spn2560_recognize;

    // 充电机编号， 范围1-100
    u8 spn2561_charger_sn;
    // 充电机/充电站所在区域编码
    unsigned char spn2562_charger_region_code[6];
};
enum recognize_result {
    BMS_RECOGNIZED     =  0xAA, // 充电机能识别BMS
    BMS_NOT_RECOGNIZED =  0x00  // 充电机不能识别BMS
};

// BMS辨识报文
struct pgn512_BRM {
    // BMS 通信协议版本号, 0x000100
    unsigned char spn2565_bms_version[3];
    /* 电池类型
     * 0x01: 铅酸电池
     * 0x02: 镍氢电池
     * 0x03: 磷酸铁锂电池
     * 0x04: 锰酸锂电池
     * 0x05: 钴酸锂电池
     * 0x06: 三元材料电池
     * 0x07: 聚合物锂离子电池
     * 0x08: 钛酸锂电池
     * 0xFF: 其他电池
     */
    u8 spn2566_battery_type;
    // 蓄电池额定容量, 0.1A.H 每位， 0-1000A.H
    u16 spn2567_capacity;
    // 蓄电池额定电压， 0.1V 每位，0-750V
    u16 spn2568_volatage;

    // 以下成员结构为可选结构, 若没有需要使用1填充
    // 电池生产商名称,ASCII 码
    unsigned char spn2569_battery_manufacturer[4];
    // 电池组序列号
    unsigned char spn2570_battery_sn[4];
    // 电池组生产日期
    u8 spn2571_year; // 1985年偏移
    u8 spn2571_month;
    u8 spn2571_day;
    // 电池组充电次数
    unsigned char spn2572_charge_count[3];
    // 电池组产权标识 @ enum battery_property
    u8 spn2573_battery_property;

    u8 spn2574_reserved; // 保留
    // 车辆识别码
    unsigned char spn2575_vin[17];
};
enum battery_property {
    BATTERY_LEASE  = 0x00, // 租赁
    BATTERY_OWNER  = 0x01  // 私有
};

// 充电参数配置阶段
// 动力蓄电池充电参数
struct pgn1536_BCP {
    // 单节电池允许最高充电电压, 0.01V 每位， 0-24V
    u16 spn2816_max_charge_volatage_single_battery;
    // 最高允许充电电流, 0.1A每位，-400A偏移 -400A - 0A
    short spn2817_max_charge_current;
    // 蓄电池标称总能量, 0.1kW。h每位， 0-1000 kW。h
    u16 spn2818_total_energy;
    // 最高允许充电电压, 0.1V 每位， 0-750V
    u16 spn2819_max_charge_voltage;
    // 最高允许温度，1 度每位，-50度偏移，-50 度 - 200度
    u8 spn2820_max_temprature;
    // 整车蓄电池荷电状态，0.1% 每位，0-100%
    u16 spn2821_soc;
    // 整车蓄电池总电压， 0.1V 每位，0-750V
    u16 spn2822_total_voltage;
};

// 充电机发送时间同步信息
struct pgn1792_CTS {
    // 压缩的BCD码
    // 第6-7字节为年
    u8 spn2823_bcd_sec;
    u8 spn2823_bcd_min;
    u8 spn2823_bcd_hour;
    u8 spn2823_bcd_day;
    u8 spn2823_bcd_mon;
    u8 spn2823_bcd_year_h;
    u8 spn2823_bcd_year_l;
};

// 充电机最大输出能力
struct pgn2048_CML {
    // 最大输出电压, 0.1V 每位， 0-750V
    u16 spn2824_max_output_voltage;
    // 最低输出电压，0.1V 每位，0-750V
    u16 spn2825_min_output_voltage;
    // 最大输出电流，0.1A 每位，-400A偏移，范围-400-0 A
    u16 spn2826_max_output_current;
};

// 电池充电准备就绪状态
struct pgn2304_BRO {
    // BMS 充电准备完成 @ enum charger_charge_status
    u8 spn2829_bms_ready_for_charge;
};
enum bms_charge_status {
    BMS_NOT_READY_FOR_CHARGE =  0x00, // 没有准备好
    BMS_READY_FOR_CHARGE     =  0xAA, // 已准备好
    BMS_INVALID              =  0xFF // 无效
};

// 充电机输出准备就绪状态
struct pgn2560_CRO {
    // 充电机充电准备完成 @ enum charger_charge_status
    u8 spn2830_charger_ready_for_charge;
};
enum charger_charge_status {
    CHARGER_NOT_READY_FOR_CHARGE = 0x00, // 没有准备好
    CHARGER_READY_FOR_CHARGE     = 0xAA, // 已准备好
    CHARGER_INVALID              = 0xFF // 无效
};

// 充电阶段
// 电池充电需求
struct pgn4096_BCL {
    // 充电电压需求，0.1V 每位，0-750V
    u16 spn3072_need_voltage;
    // 充电电流需求，0.1A 每位，-400V偏移，-400A-0A
    short spn3073_need_current;
    // 充电模式 @ enum charge_mode
    u8  spn3074_charge_mode;
};
enum charge_mode {
    CHARGE_WITH_CONST_VOLTAGE  = 0x01,  // 恒流充电
     CHARGE_WITH_CONST_CURRENT = 0x02  // 恒压充电
};

// 电池充电总状态
struct pgn4352_BCS {
    // BMS 的充电电压测量值，0.1V 每位，0V偏移， 0-750V
    u16 spn3075_charge_voltage;
    // BMS的充电电流测量值，0.1A 每位， -400A偏移， -400A-0A
    short spn3076_charge_current;
    // 最高单体动力蓄电池电压及其组号，1-12位： 最高单体电池电压， 0.01V 每位， 0-24V
    // 13-16： 最高电压电池所在组号， 1/位， 1-16
    u16 spn3077_max_v_g_number;
    // 当前荷电容量, 1% 每位， 0-100%
    u8  spn3078_soc;
    // 剩余充电时间, 1Min 每位 0-600min
    u16 spn3079_need_time;
};

// 充电机充电状态
struct pgn4608_CCS {
    // 充电机输出电压，0.1V/位， 0V偏移量，0-750V
    u16 spn3081_output_voltage;
    // 充电机输出电流，0.1A/位，-400A偏移，-400A-0A
    u16 spn3082_output_current;
    // 充电持续时间，1min/位，0偏移，0-600min
    u16 spn3083_charge_time;
};

// 动力蓄电池状态信息
struct pgn4864_BSM {
    // 最高单体电压的蓄电池编号
    u8 sn_of_max_voltage_battery;
    // 最高蓄电池温度
    u8 max_temperature_of_battery;
    // 最高温度采样点编号
    u8 sn_of_max_temperature_point;
    // 最低蓄电池温度
    u8 min_temperature_of_battery;
    // 最低温度采样点编号
    u8 sn_of_min_temperature_point;
    // 遥信 @ enum pgn4846_remote_single
    u16 remote_single;
};
enum pgn4846_remote_single {
    // 单体蓄电池电压正常
    SINGLE_BATTERY_VOLTAGE_NORMAL = 0x00000000,
    // 单体蓄电池电压过高
    SINGLE_BATTERY_VOLTAGE_HIGH   = 0x00000001,
    // 单体学电池电压过低
    SINGLE_BATTERY_VOLTAGE_LOW    = 0x00000002,

    // 蓄电池组荷电状态正常
    BATTERY_SOC_NORMAL            = 0x00000000,
    // 蓄电池组荷电状态过高
    BATTERY_SOC_HIGH              = 0x00000004,
    // 蓄电池组荷电状态过低
    BATTERY_SOC_LOW               = 0x00000008,

    // 充电电流正常
    BATTERY_CHARGE_CURRENT_NORMAL = 0x00000000,
    // 充电电流过高
    BATTERY_CHARGE_CURRENT_HIGH   = 0x00000010,
    // 充电电流过低
    BATTERY_CHARGE_CURRENT_LOW    = 0x00000020,

    // 电池温度正常
    BATTERY_TEMPRATUEE_NORAML     = 0x00000000,
    // 电池温度过高
    BATTERY_TEMPRATURE_HIGH       = 0x00000040,
    // 电池温度过低
    BATTERY_TEMPRATURE_LOW        = 0x00000080,

    // 电池绝缘状态正常
    INSULATION_NORMAL             = 0x00000000,
    // 电池有绝缘故障
    INSULATION_FAULT              = 0x00000100,
    // 绝缘信号不可信
    INSULATION_UNRELIABLE         = 0x00000200,

    // 冲电连接器状态正常
    CONNECTOR_STATUS_NORMAL       = 0x00000000,
    // 充电连接器不正常
    CONNECTOR_STATUS_FAULT        = 0x00000400,
    // 冲电连接器状态不可信
    CONNECTOR_STATUS_UNRELIABLE   = 0x00000800,

    // 允许充电
    CHARGE_ALLOWED                = 0x00000000,
    // 禁止充电
    CHARGE_FORBIDEN               = 0x00001000
};

// 单体动力蓄电池电压
struct pgn5376_BMV {
    // bit[0:11]: 电压， 0.01V 每位, 0V偏移，范围 0- 24V
    // bit[12:15]: 蓄电池编号，1 / 每位， 范围 1-16
    u16 bat_v[256];
};

// 动力蓄电池温度
struct pgn5632_BMT {
    // 1 度每位， -50度偏移， 范围 -50 - 200
    u16 bat_tmp[128];
};

// 动力蓄电池预留报文
struct pgn5888_BSP {
    u16 reseved[16];
};

// BMS终止充电
struct pgn6400_BST {
    // BMS 中止充电原因 @ enum REASON_PGN6400
    u8 reason;
    // BMS 中止充电故障原因 enum ERROR_PGN6400
    u16 error;
    // BMS 中止充电错误原因 enum FAULT_PGN6400
    u16 fault;
};
enum REASON_PGN6400 {
    // 达到所需SOC值 bit[0:1]
    REASON_UN_REACH_SOC_VAL  = 0x00,
    REASON_REACH_SOC_VAL     = 0x01,
    REASON_SOC_UNRELIABLE        = 0x02,

    // 达到总电压设定值 bit[2:3]
    REASON_UN_REACH_VOL_VAL  = 0x00,
    REASON_REACH_VOL_VAL     = 0x04,
    REASON_VOL_UNRELIABLE        = 0x08,
    //                bit[4:5]
    REASON_UN_REACH_SINGLE_BAT_VOL = 0x00,
    REASON_REACH_SINGLE_BAT_VOL    = 0x10,
    REASON_SINGLE_VOL_UNRELIABLE   = 0x20
};
//enum ERROR_PGN6400 {};
//enum FAULT_PGN6400 {};

// 充电机终止充电
struct pgn6656_CST {
    // 中止充电原因
    u8 reason;
    // 终止充电故障原因
    u16 err_reason;
    // 中止充电错误原因
    u8 fault_reason;
};

// 充电结束阶段
// BMS统计数据
struct pgn7168_BSD {
    u8 end_soc;   // 中止SOC
    u16 min_bat_V;// 最低电压
    u16 max_bat_V;// 最高电压
    signed char min_bat_T; // 最低温度
    signed char max_bat_T; // 最高温度
};

// 充电机统计数据
struct pgn7424_CSD {
    u16 total_min; // 充电时长
    u16 total_kwh; // 充电输出能量
    u8  charger_sn;// 充电机编号
};

// 错误报文分类
// BMS 错误报文
struct pgn7680_BEM {
};

// 充电机错误报文
struct pgn7936_CEM {
    u8 brm_timeout:2;
    u8 brm_unused:6;
    u8 bcp_timeout:2;
    u8 bro_timeout:2;
    u8 bcp_unused:4;
    u8 bcs_timeout:2;
    u8 bcl_timeout:2;
    u8 bst_timeout:2;
    u8 bcs_unused:2;
    u8 bsd_timeout:2;
    u8 bsd_unused:6;
};

#pragma pack()

// 充电屏数据读写数据结构
struct charger_config_03h_04h_10h {

};

// 充电屏数据结构
struct charger_config_10h {
    // {{ 充电屏监控返回数据
    // 充电屏编号
    unsigned short charger_sn;
    // 充电机组充电状态
    unsigned short charger_status;
    // 充电及自身状态
    unsigned short charger_self_status;
    // 充电机组最高输出电压
    unsigned short charger_max_v_out;
    // 充电机组最低输出电压
    unsigned short charger_min_v_out;
    // 充电机组最大输出电流
    unsigned short charger_max_i_out;
    // 充电机组实际输出电压值
    unsigned short charger_v_out;
    // 充电机组实际输出电流值
    unsigned short charger_i_out;

    // 充电机模块输出电压
    unsigned short charge_module_v[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块输出电流
    unsigned short charge_module_i[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块温度
    unsigned short charge_module_t[CONFIG_SUPPORT_CHARGE_MODULE];
    // 充电机模块状态
    unsigned short charge_module_status[CONFIG_SUPPORT_CHARGE_MODULE/2];
    // 充电机模块机身编号
    unsigned short charge_module_sn[CONFIG_SUPPORT_CHARGE_MODULE][3];
    unsigned short charge_kaichu;
    unsigned short charge_kairu;
    // }}
};
// 开机
static inline int __module_set_on(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    st[ n ] &= ~0x10;
    return st[ n ] >> 4;
}
// 关机
static inline int __module_set_off(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    st[ n ] |= 0x10;
    return st[ n ] >> 4;
}
// 测试模块是否开机
static inline int __module_is_on(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    return st[ n ] >> 4;
}
// 设置故障位
static inline int __module_set_err(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    st[ n ] |= 0x01;
    return st[ n ] & 0x01;
}
// 清除故障位
static inline int __module_clr_err(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    st[ n ] &= ~0x0F;
    return st[ n ] & 0x01;
}
// 测试模块是否故障
static inline int __module_is_err(struct charger_config_10h *p, unsigned int n) {
    unsigned char *st = (unsigned char *)p->charge_module_status;
    return st[ n ] & 0x01;
}

// 通信报文生成依据
struct can_pack_generator {
    // 所属阶段
    CHARGE_STAGE stage;
    // 生成PGN
    unsigned int can_pgn;
    // 数据包优先级
    unsigned int prioriy;
    // 数据包长度
    unsigned int datalen;
    // 数据包发送周期
    unsigned int period;
    // 心跳计数
    unsigned int heartbeat;

    // 数据包静默时间，只针对接受数据包有效
    unsigned int can_silence;
    // 接受数据包容忍的沉默时常，针对接收数据有效
    unsigned int can_tolerate_silence;
    // 计数器，接收或发送的次数
    unsigned int can_counter;
};

// CAN 链接临时参数
struct can_tp_param {
    // 传输的数据包PGN
    unsigned int tp_pgn;
    // 即将传输的数据包大小
    unsigned int tp_size;
    // 即将传输的数据包个数
    unsigned int tp_pack_nr;

    // 已经接收的数据字节数
    unsigned int tp_rcv_bytes;
    // 已经接收的数据包个数
    unsigned int tp_rcv_pack_nr;
};

// BMS 通讯管理描述结构，JOB的下属成员结构
struct bms_struct {
    COMM_M_STRUCT_STATUS status;
    // 充电作业所处阶段,BMS 链接阶段
    CHARGE_STAGE charge_stage;

    struct charge_job *job;
    // BMS通讯设备名, "can1|can2"
    char *can_dev;
    // BMS写初始化完成
    int bms_write_init_ok;
    // BMS读初始化完成
    int bms_read_init_ok;
    // CAN BMS 通信所处状态
    CAN_BMS_STATUS can_bms_status;
    // 连接管理的传输控制参数
    struct can_tp_param can_tp_param;
    // 连接管理超时控制器
    struct Hachiko_food can_tp_bomb;
    // 连接超时参数组状态
    PRIVATE_STATUS can_tp_private_status;
    // CAN数据包心跳时钟
    struct Hachiko_food can_heart_beat;

    // 车辆基本信息
    struct pgn512_BRM  brm;
    // BMS充电配置信息
    struct pgn1536_BCP bcp;
    // BMS当前充电需求信息
    struct pgn4096_BCL bcl;
    // BMS 电池充电总状态信息
    struct pgn4352_BCS bcs;
    // BMS 动力蓄电池状态信息
    struct pgn4864_BSM bsm;
    // BMS 动力蓄电池电压信息
    struct pgn5376_BMV bmv;
    // BMS 动力蓄电池温度信息
    struct pgn5632_BMT bmt;
    // BMS 中止充电原因
    struct pgn6400_BST bst;
    // BMS 充电统计信息
    struct pgn7168_BSD bsd;
    // 充电机 中止充电原因
    struct pgn6656_CST cst;
    // 充电机统计信息
    struct pgn7424_CSD csd;

    // BMS 驱动
    struct bmsdriver *driver;

    // 发送的数据包总数
    unsigned int frame_tx;
    // 发送速率
    unsigned int frame_speed_tx;
    unsigned int tx_seed;
    // 接收的数据包总数
    unsigned int frame_rx;
    // 接收速率
    unsigned int frame_speed_rx;
    unsigned int rx_seed;

    // 数据库操作计数器
    unsigned int readed;
    unsigned int can_pack_gen_nr;
    struct can_pack_generator *generator;
};

typedef enum {
    // 储值卡
    ACCOUNT_PRESTORCARD = 0x00,
    // BOSS帐号
    ACCOUNT_BOSS_ID = 0x01,
    // 支付宝帐号
    ACCOUNT_ALIPAY_ID = 0x11,
    // 微信号
    ACCOUNT_WEIXIN_ID = 0x12,
    // 银行卡
    ACCOUNT_BANK_ID = 0x13
}ACCOUNT_TYPE;

// 充电计费方式
struct billing_methord {
    // 计费方式
    BILLING_MODE mode;
    union {
        // 设定充电金额, 0 - 9999
        double set_money;
        // 设定充电时长, 0 - 600
        unsigned int set_time;
        // 设定充电目标电量, 0 - 100
        double set_kwh;
    }option;

    // 交易账单号
    unsigned char billing_sn[32 + 1];
    // 帐号类型
    ACCOUNT_TYPE account_type;
    // 帐号
    unsigned char account[32 + 1];
    // 密码
    unsigned char passwd[16 + 1];
    // 预扣金额
    double pre_hold_amount;
};

// 采样盒 通讯管理描述结构，JOB的下属成员结构
struct measure_struct {
    COMM_M_STRUCT_STATUS status;
    // 扩展测量值
    struct MDATA_ACK measure;
    // 前一次测量拷贝值
    struct MDATA_ACK measure_pre_copy;
    // 前一次读取扩展测量得到的时间戳, 通过对比时间戳来确定扩展测量是否已经更新了数据
    time_t pre_stamp_ex_measure;
};

// 充电机 通讯管理描述结构，JOB的下属成员结构
struct charger_struct {
    COMM_M_STRUCT_STATUS status;
    // 充电机状态
    CHARGER_STATUS  cstats;
    // 充电屏信息
    struct charger_config_10h chargers;
};

// 电表数据
struct meter_data {
    // 地址
    unsigned char addr[13];
    // A 相电压
    double Va;
    // B 相电压
    double Vb;
    // C 相电压
    double Vc;

    // 正向总电能
    double kwh_zong;
    // 尖电能
    double kwh_jian;
    // 峰电能
    double kwh_feng;
    // 平电能
    double kwh_ping;
    // 谷电能
    double kwh_gu;

    // 电流
    double current[2];

    // 电流有效
    int current_ok[2];
};

// 作业提交结构
struct job_commit_data {
    // 作业提交命令
    COMMIT_CMD cmd;
    // URL端提交的时间戳
    time_t url_commit_timestamp;
    // 服务端提交的时间日期
    time_t ontom_commit_date_time;

    //{{ 创建作业参数
    char card_sn[32];            // 所刷卡卡号
    double card_remain;          // 卡内余额
    char card_passwd[16];        // 卡片密码
    CHARGE_GUN_SN charge_gun;    // 充电枪选择
    CHARGE_MODE charge_mode;     // 选择自动或是手动
    int assit_power_level;
    double manual_set_charge_volatage; // 手动设置充电电压, 手动充电有效
    double manual_set_charge_current;  // 手动设置充电电流, 手动充电有效
    BILLING_MODE  biling_mode; // 充电计费方式
    double as_kwh;   // 按电量充，KW.h, 自动充电，默认充满
    double as_money; // 按金额充，元
    unsigned int as_time;  // 按时间充电，分钟数
    //}}

    struct list_head job_node;
};

//充电作业描述，充电管理的最小单位
struct charge_job {
    //  作业状态
    JOBSTATUS job_status;
    // 产生故障时的状
    JOBSTATUS status_befor_fault;
    // 作业序号
    time_t jid;
    // 作业充电枪
    CHARGE_GUN_SN job_gun_sn;
    // 充电模式
    CHARGE_MODE charge_mode;
    // 辅助电源大小
    int assit_power_level;
    // 通信CAN名称
    char job_can_dev_name[32];
    // 充电起始时戳， 闭合充电开关的那一刻
    time_t charge_begin_timestamp;
    // 充电结束时戳， 断开充电开关的那一刻
    time_t charge_stop_timestamp;
    // 中止充电的那一刻
    time_t charge_abort_timestamp;
    // 转为实施状态时的时戳, 执行实施函数的那一刻
    time_t charge_implemention_timestamp;
    // BMS握手成功的时戳, 接收到第一次BRM的时刻
    time_t charge_bms_establish_timestamp;
    // 作业创建的时间戳
    time_t charge_job_create_timestamp;
    // 开始充电时的电表度数
    double charge_begin_kwh_data;
    // 终止充电时的电表度数
    double charge_exit_kwh_data;

	// 退出延时时戳
	time_t delay_exit_tsp;

    // 当前充电阶段使用的电能
    double section_kwh;
    // 已经消耗的电能
    double charged_kwh;
    // 当前阶段充电时长
    unsigned int section_seconds;
    // 已经充电的时间长度，按秒计
    unsigned int charged_seconds;
    // 已经消费的数额, 按元记，精确到分
    double charged_money;

    // 当前充电电压
    double charge_voltage;
    // 当前充电电流
    double charge_current;

    // 结构体引用计数
    unsigned int ref_nr;

    // CAN读句柄
    int can_read_handle;
    // CAN写句柄
    int can_write_handle;

    // {{ 充电控制参数
    double need_V;
    double need_I;
    // }}

    // 充电计费方式
    struct billing_methord charge_billing;
    // 刷卡状态
    struct user_card card;
    // BMS 管理模块
    struct bms_struct bms;
    // 多包通信参数
    struct bms_event_struct param;

    struct charge_task *tsk;

    // 作业状态字
    volatile unsigned char single[8];

    struct list_head job_node;
};

// 历史故障定义
struct error_history {
    // 故障唯一ID
    unsigned int error_seqid;
    // 故障代码编号
    unsigned int error_id;
    // 故障起始日期, 时间
    char error_begin[64];
    // 故障恢复日期，时间
    char error_recover[64];
    // 故障参数
    char error_string[64];
    // 故障恢复原因
    ERR_RECOVER_REASON error_recover_reason;

    struct list_head error_me;
};

// 配置项
struct task_config {
    // 系统配置项
    struct {
    }sys;

    // 通讯配置项
    struct {
    }comm;

    // 充电机配置项
    struct {
    }charger;
};

// 代码跟踪
struct code_trace {
    volatile unsigned char single[16];
};

//充电任务描述, 详细描述了系统的配置参数
struct charge_task {
    // 已经编译为多线程安全模式，所以不用加锁
    sqlite3 *database;
    pthread_t tid;
    pthread_attr_t attr;
    // 扩展程序
    struct exso_struct *exsos;
    // BMS 驱动程序
    struct bmsdriver *bmsdriver;
    // BMS 读线程ID
    pthread_t tid_read;
    // BMS 写线程ID
    pthread_t tid_write;

    // 作业任务提交列表
    struct list_head *commit_head;
    // 作业任务提交列表锁
    pthread_mutex_t commit_lck;

    // 刷卡数据临时存储区
    struct user_card card;
    // 刷卡数据临时存储区
    struct user_card op_card;
    // 当前界面显示的页面号
    UI_PAGE uipage;
    // 当处于作业详情页刷卡时的当前作业ID
    unsigned int ui_job_id;

    // 后台认证结构
    struct user_auth auth;
    // 请求充电
    struct charge_request request;

    // 当前正在执行的充电任务
    struct charge_job *job[CONFIG_SUPPORT_CHARGE_JOBS];

    // 等待作业列表
    struct list_head *wait_head;
    // 作业任务等待列表锁
    pthread_mutex_t wait_lck;
    // 等待作业个数
    unsigned int wait_job_nr;

    // 当前故障列表
    pthread_mutex_t err_list_lck;
    unsigned int err_nr;
    struct list_head *err_head;

    // 任务记录故障总数
    unsigned int err_seq_id_next;

    // 串口设备管理模块
    struct bp_uart *uarts[CONFIG_SUPPORT_RS485_NR];
    // 采样单元管理模块
    struct measure_struct *measure[CONFIG_SUPPORT_SIMPLE_BOX_NR];
    // 充电机管理模块
    struct charger_struct *chargers[CONFIG_SUPPORT_CHARGE_GRP_NR];
    // 电表电能
    struct meter_data meter[CONFIG_SUPPORT_CHARGE_GRP_NR];

    // 系统信号, 最多支持64 * 8 种信号标记
    // 前面 16 * 8 = 128 个信号是系统内部使用信号标记
    // 后面 的为遥信 信号定义 @ enum ONTOM_FLAG_SINGLE
    volatile unsigned char single[64];
    // 系统前一次信号状态，用来做状态跳变比较
    volatile unsigned char pre_single[64];

    // 代码跟踪
    struct code_trace trace;

    // {{ 以下为充电桩系统监控的配置数据
    SYSTEM_TYPE sys_type;
    bool sys_conflict_map[CONFIG_SUPPORT_BMS_NR][CONFIG_SUPPORT_BMS_NR];
    /*系统配置的充电枪个数*/
    unsigned int sys_config_gun_nr;
    /* 不同充电接口对应的CAN设备名称*/
    char sys_can_name[CONFIG_SUPPORT_BMS_NR][128];
    /*采样单元个数, 默认为1个，若配置两个采样盒则可以*/
    unsigned int sys_simple_box_nr;
    /*充电机组数， 默认1组.*/
    unsigned int sys_charge_group_nr;
    /*通信用485串口数个数*/
    unsigned int sys_rs485_dev_nr;
    /*通信用485串口设备文件名*/
    char sys_uart_name[CONFIG_SUPPORT_RS485_NR][128];
    // 每度电的价格
    double kwh_price;
    // 系统模式模式
    char sys_work_mode[32];
    // BMS 默认驱动厂商及版本
    char bms_vendor_version[32];
    // }}

    // 以下适用于采样盒配置值
    double bus_1_v_hi;
    double bus_1_v_lo;
    double bus_2_v_hi;
    double bus_2_v_lo;
    double bat_1_v_hi;
    double bat_1_v_lo;
    double bat_2_v_hi;
    double bat_2_v_lo;
    double bat_1_I_hi;
    double bat_2_I_hi;
    double bat_1_insti_ohm_v;
    double bat_2_insti_ohm_v;
    double bus1_bat1_check_v;
    double bus2_bat2_check_v;
    double bat1_bat2_check_I;

    /*额定电流*/
    double max_output_I;
    /*输出限流*/
    double limit_output_I;
    /*电压上限值*/
    double limit_max_V;
    /*电压下限值*/
    double limit_min_V;
    /*工作电压值*/
    double running_V;
    /*当前电流值*/
    double running_I;
    /*模块个数*/
    unsigned short modules_nr;
    /*充电状态*/
    unsigned short charge_stat;
    /*模块型号*/
    MODULE_MODEL module_model;

    /*模块开关机, 0x00: 无动作， 0x80: 开机, 0x81: 关机*/
    unsigned char modules_on_off[CONFIG_SUPPORT_CHARGE_MODULE];
    //}}

    // 电能表电压系数比
    double meter_V_xishu;
    // 电能表电流系数比
    double meter_I_xishu;
    // 电表类型
    KWH_METER meter_type;

    // 电流分流器系数
    unsigned int flq_xishu;
    // 模块功率因数
    double module_power_fact;
    // 单台模块额定电流
    double single_module_A;
    // 单台模块最大允许电流
    double single_module_max_I;
    // 充电触发压差
    double charge_triger_V;
    // 模块输出最高电压
    double module_max_V;
    // 模块输出最低电压
    double moudle_min_V;
    // {{ 电压电流校准参数
    double bus1_correct_V;
    double bus1_read_V;
    double bus2_correct_V;
    double bus2_read_V;
    double bus_correct_I;
    double bus_read_I;
    // }}

#define KAICHU_SYSCHG          0
#define KAICHU_SYSERR          1
#define KAICHU_GUN1_SW         2
#define KAICHU_GUN2_SW         3
#define KAICHU_GUN1_ASSIT_12   4
#define KAICHU_GUN1_ASSIT_24   5
#define KAICHU_GUN2_ASSIT_12   6
#define KAICHU_GUN2_ASSIT_24   7
#define KAICHU_GUN1_LOCK       8
#define KAICHU_GUN2_LOCK       9
#define KAICHU_AC_LED          10
#define KAICHU_LIGHT           11
#define KAICHU_EXT1            12
#define KAICHU_EXT2            13
#define KAICHU_EXT3            14
#define KAICHU_EXT4            15

    // 开出端口掩码
    unsigned short kaichu_mask[16];
    // 开出端口映射
    unsigned int kaichu_map[16];
    // 辅助电源投切方式
    DOUBLE_ASSIT_SELECT_METHORD assit_select_methord;

    // 采样板输出端口功能定义
    SIMPLE_BOX_OUT_PORT ctl_port[8];

    // 交流输入过欠压
    double ac_input_lo;
    double ac_input_hi;

    // 授权序号, BCD 码
    unsigned char bcd_auth_code[17];
    // 网卡MAC  地址
    unsigned char mac_addr[32];

    // 硬件软件配置映射表
    volatile unsigned char h_s_ware_config[8];

    // 本机地址
    int charger_sn;
    // 服务器地址
    char server_addr[32];
    // 服务器端口
    int server_port;

    // 照明开启时间
    char lightup_time[32];
    // 照明时长
    double lightup_hours;

    // 交易方式
    DEAL_METHORD deal_methord;
};

static inline unsigned int system_power(struct charge_task *tsk) {
    switch ( task->module_model ) {
    default:
    case MODEL_AN10680:
        return 10 * task->modules_nr;
        break;
    case MODEL_INCREASE:
        return 10 * task->modules_nr;
        break;
    }
}

static inline unsigned char check_sum(unsigned char *buff, size_t len) {
    unsigned int i = 0;
    unsigned char sum = 0;
    while ( i < len ) {
        sum = sum + buff[i ++];
    }
    return sum;
}
static inline double __card_read_remain(const struct user_card *c) {
    unsigned int imoney = 0;

    imoney += c->card.sector_4.data.remain_money[0];
    imoney += c->card.sector_4.data.remain_money[1] * 256;
    imoney += c->card.sector_4.data.remain_money[2] * 256 * 256;

    return imoney / 100.0f;
}

static inline double __card_write_remain(struct user_card * c, double money) {
    unsigned int imoney = money * 100;
    c->card.sector_4.data.remain_money[0] = imoney & 0xFF;
    c->card.sector_4.data.remain_money[1] = (imoney >> 8) & 0xFF;
    c->card.sector_4.data.remain_money[2] = (imoney >> 16) & 0xFF;
    c->card.sector_4.data.remain_sum =
            check_sum(c->card.sector_4.data.remain_money, 3);
    return money;
}

static inline void __card_read_passwd(const struct user_card *c, char *passwd) {
    sprintf(passwd, "%d%d%d%d%d%d",
            c->card.sector_4.data.passwd_code[2]>>4,
            c->card.sector_4.data.passwd_code[2]&0xF,
            c->card.sector_4.data.passwd_code[1]>>4,
            c->card.sector_4.data.passwd_code[1]&0xF,
            c->card.sector_4.data.passwd_code[0]>>4,
            c->card.sector_4.data.passwd_code[0]&0xF);
}

static inline unsigned char __chars2bcd(char h, char l) {
    if ( h > '9' || h <'0' ) h = '0';
    if ( l > '9' || l <'0' ) l = '0';
    return (((h - '0')&0xF)<<4)|((l-'0')&0x0F);
}

static inline void __card_write_passwd(struct user_card * c, const char* passwd) {
    c->card.sector_4.data.passwd_code[2] = __chars2bcd(passwd[0], passwd[1]);
    c->card.sector_4.data.passwd_code[1] = __chars2bcd(passwd[2], passwd[3]);
    c->card.sector_4.data.passwd_code[0] = __chars2bcd(passwd[4], passwd[5]);
    c->card.sector_4.data.passwd_sum =
            check_sum(c->card.sector_4.data.passwd_code, 3);
}

/* 从BMS版本描述字串中截取BMS厂商号
 */
static inline int __string_to_bms_vendor(const char *bmsv) {
    char buff[16] = {0};
    int i = 0;

    if ( bmsv == NULL ) return 0;
    while ( bmsv[i] && bmsv[i] >= '0' && bmsv[i] <= '9' ) {
        buff[ i ] = bmsv[i];
        i ++;
    }
    return atoi(buff);
}

/* 从BMS版本描述字串中截取BMS版本号
 */
static inline int __string_to_bms_version(const char *bmsv, char *ver) {
    char buff[16] = {0};
    int i = 0;

    if ( bmsv == NULL ) return 0;
    while ( bmsv[i] && bmsv[i] >= '0' && bmsv[i] <= '9' ) {
        buff[ i ] = bmsv[i];
        i ++;
    }
    if ( bmsv[i] !='_' ) return ERR_ERR;
    i ++;
    while ( bmsv[i] ) {
        *ver++ = bmsv[i];
        i ++;
    }
    return ERR_OK;
}



// 位设置
static inline void __bit_set(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte |= (1 << (single % 8 ));
}
#define bit_set(tsk, bits) __bit_set(tsk->single, bits)
#define bit_set_pre(tsk, bits) __bit_set(tsk->pre_single, bits)
// 位清除
static inline void __bit_clr(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return;
    byte += single / 8;
    * byte &= (~(1 << (single % 8 )));
}
#define bit_clr(tsk, bits) __bit_clr(tsk->single, bits)
#define bit_clr_pre(tsk, bits) __bit_clr(tsk->pre_single, bits)
// 位读取
static inline int __bit_read(volatile unsigned char *byte, ONTOM_FLAG_SINGLE single)
{
    if ( ! byte ) return 0;
    byte += single / 8;

    return (* byte & (1 << (single % 8 ))) ? 1 : 0;
}
#define bit_read(tsk, bits) __bit_read(tsk->single, bits)
#define bit_read_pre(tsk, bits) __bit_read(tsk->pre_single, bits)

//1字节crc16计算
static inline void calc_crc16(unsigned short *crc, unsigned short  crcbuf)
{
     unsigned char  i,TT;

    *crc=*crc^crcbuf;
    for(i=0;i<8;i++)
    {
        TT=*crc&1;
        *crc=*crc>>1;
        *crc=*crc&0x7fff;
        if (TT==1)
            (*crc)=(*crc)^0xa001;
        *crc=*crc&0xffff;
    }
}

//多字节CRC16计算
static inline unsigned short load_crc(unsigned short cnt, unsigned char *dat)
{
     unsigned short i;
     unsigned short crc=0xffff;
    for(i=0;i<cnt&&i<0xff;i++)
    {
        calc_crc16(&crc,dat[i]);
    }
    return crc;
}

// 大小端转换, 字
static inline unsigned short swap_hi_lo_bytes(unsigned short b)
{
    return (((b << 8)&0xFF00) | ((b >> 8)&0x00FF));
}
#define b2l swap_hi_lo_bytes
#define l2b swap_hi_lo_bytes
// 大小端转换, 双字
static inline unsigned int swap_hiw_low(unsigned int b)
{
    return  (0xFF000000&(b << 24)) |
            (0x00FF0000&(b << 8 )) |
            (0x0000FF00&(b >> 8 )) |
            (0x000000FF&(b >> 24));
}
#define bb2ll swap_hiw_low
#define ll2bb swap_hiw_low

static inline CHARGE_GUN_SN __is_gun_phy_conn_ok(struct charge_job *thiz)
{
    if ( thiz->job_gun_sn == GUN_SN0 ) {
        if ( bit_read(task, F_GUN_1_PHY_CONN_STATUS) ) {
            return GUN_SN0;
        } else return GUN_INVALID;
    } else if ( thiz->job_gun_sn == GUN_SN1 ) {
        if ( bit_read(task, F_GUN_2_PHY_CONN_STATUS) ) {
            return GUN_SN1;
        } else return GUN_INVALID;
    } else if ( thiz->job_gun_sn == GUN_UNDEFINE ) {
        return GUN_UNDEFINE;
    } else {
        return GUN_INVALID;
    }
}

static inline unsigned int __atoh(const char *hex)
{
    unsigned int v = 0;

    if ( !hex ) return 0;
    if ( hex[0] == '0' && (hex[1] == 'X' || hex[1] == 'x' ) )
        hex ++, hex ++;

    while ( *hex ) {
        if (*hex >= '0' && *hex <= '9') {
            v = v * 16 + (*hex) - '0';
        } else if (*hex >= 'A' && *hex <= 'F') {
            v = v * 16 + (*hex) - 'A' + 10;
        } else if (*hex >= 'a' && *hex <= 'f') {
            v = v * 16 + (*hex) - 'a' + 10;
        } else break;
        hex ++;
    }

    return v;
}
#define atoh __atoh
static inline double __bytes2double(unsigned short bytes)
{
    if ( 0x8000 & bytes ) {
        return (bytes&0x7FFF)/(-10.0f);
    }
    return (bytes & 0x7FFF) / 10.0f;
}

static inline unsigned short double2short(double df, unsigned int acc) {
    return  (unsigned short)(df * acc * 1.0f);
}

static inline double bytes2double(unsigned char h, unsigned l, unsigned int acc) {
    return (h * 256 + l) / (1.0f * acc);
}
#if 0
static inline void mac_public_code(unsigned char *obf, unsigned char *mac)
{
    unsigned char feed_code[12] = {10, 12, 13, 13,
                                   8, 11, 13, 2,
                                   8, 9, 1, 12};
    int i = 0;
    unsigned char maccode[6 + 1] = {0};
    maccode[0] = (unsigned char)atoh(&mac[0]);
    maccode[1] = (unsigned char)atoh(&mac[3]);
    maccode[2] = (unsigned char)atoh(&mac[6]);
    maccode[3] = (unsigned char)atoh(&mac[8]);
    maccode[4] = (unsigned char)atoh(&mac[11]);
    maccode[5] = (unsigned char)atoh(&mac[14]);

    obf[0] = (maccode[0] & 0xF0) | feed_code[0];
    obf[1] = ((maccode[0] << 4) & 0xF0) | feed_code[1];
    obf[2] = (maccode[1] & 0xF0) | feed_code[2];
    obf[3] = ((maccode[1] << 4) & 0xF0) | feed_code[3];
    obf[4] = (maccode[2] & 0xF0) | feed_code[4];
    obf[5] = ((maccode[2] << 4) & 0xF0) | feed_code[5];
    obf[6] = (maccode[3] & 0xF0) | feed_code[6];
    obf[7] = ((maccode[3] << 4) & 0xF0) | feed_code[7];
    obf[8] = (maccode[4] & 0xF0) | feed_code[8];
    obf[9] = ((maccode[4] << 4) & 0xF0) | feed_code[9];
    obf[10] = (maccode[5] & 0xF0) | feed_code[10];
    obf[11] = ((maccode[5] << 4) & 0xF0) | feed_code[11];

    obf[12] = 0;
    while ( i < 12 ) {
        obf[12] += obf[i];
    }
}

static inline void gen_key(unsigned char *key, const unsigned char *mac_pub_key)
{
    int i;
    const unsigned char private_key[13] =
                          {0x62, 0x79, 0x20, 0x6C,
                           0x69, 0x6A, 0x69, 0x65,
                           0x20, 0x32, 0x30, 0x31, 0x35}; //"by lijie 2015"
    const unsigned char hex_map2_bcd[16] = {9, 6, 3, 1,
                                      9, 4, 8, 3,
                                      2, 0, 7, 4,
                                      4, 5, 4, 4};
    unsigned char check_code[13 + 1] ={0};

    for ( i = 0; i < 13; i ++ ) {
        check_code[i] = private_key[i] ^ mac_pub_key[i];
    }

    for ( i = 0; i < 13; i ++ ) {
        key[i] = hex_map2_bcd[check_code[i] % 16 ] + '0';
    }
}

static inline int check_auth(const char *mac, const char *bcdcode)
{
    unsigned char mac_pub_key[13 + 1] = {0};
    unsigned char check_code_bcd[13 + 1] = {0};

    mac_public_code(mac_pub_key, mac);
    gen_key(check_code_bcd, mac_pub_key);

    if ( 0 == strcmp(check_code_bcd, bcdcode) ) return 1;

    return 0;
}
#endif
// 获取模块最高电压
static inline double __module_max_voltage(struct charger_config_10h *p, unsigned int module_nr) {
    unsigned int nr = 0;
    double max_v = 0.0f;
    double module_v;

    for ( nr = 0; nr < module_nr &&
          nr < CONFIG_SUPPORT_CHARGE_MODULE; nr ++ ) {
        module_v = b2l(p->charge_module_v[nr])/10.0f;
        if (  module_v > max_v ) {
            max_v = module_v;
        }
    }

    return module_v;
}

int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd);
unsigned int error_history_begin(struct charge_job *job, unsigned int error_id, char *error_string);
void error_history_recover(struct charge_job *job, unsigned int error_id);
void deal_with_system_protection(struct charge_task *tsk, struct charge_job *thiz);
void job_running(struct charge_task *, struct charge_job *);
void job_destroy(struct charge_job *);
int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd);
int job_exsit(time_t id);
int job_export(struct charge_job *);
struct charge_job * job_fork(struct charge_task *tsk, struct job_commit_data *need);
struct charge_job* job_search(time_t ci_timestamp);
struct charge_job * job_select_wait(struct charge_task *tsk, CHARGE_GUN_SN gun);
struct job_commit_data *job_select_commit(struct charge_task *tsk);
void job_detach_wait(struct charge_task *tsk);

int job_wait(struct charge_task *, struct charge_job *);
int job_standby(struct charge_task *, struct charge_job *);
int job_working(struct charge_task *, struct charge_job *);
int job_done(struct charge_task *, struct charge_job *);
int job_billing(struct charge_task *, struct charge_job *);
#endif /*_CHARGE_INCLUDED_H_*/
