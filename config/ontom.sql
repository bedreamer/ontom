CREATE TABLE user(
	username string,
	passwd string
	);
INSERT INTO user VALUES
    (
	'zeus',
	'45c7c9ffa8c374942d62a80602cd2d07'
    );
INSERT INTO user VALUES
    (
	'manufacturer',
	'415c92bb33c0496cf92dfc4a48b37bcd'
    );
INSERT INTO user VALUES
    (
	'system',
	'ac7d6f1b636fafe1985f2be26d615189'
    );
INSERT INTO user VALUES
    (
	'user',
	'9b1ce7dae449fe298abd224e2bab2888'
    );

CREATE TABLE log(
	timestamp string,
	logstring string
	);

CREATE TABLE errors(
	error_seq_id string,
	error_id string,
	error_begin string,
	error_recover string,
	recover_reason string,
	error_string string
	);

CREATE TABLE configs(
	config_attrib string,
	config_name string,
	config_type string,
	config_value string,
	config_options string,
	config_value_max string,
	config_value_min string,
	config_value_default string,
	config_value_pre string,
	config_writeable bool,
	config_readable bool,
	config_last_update string,
	config_comment string,
	config_dis bool,
	config_dis_name string,
	config_dis_comment string
	);
INSERT INTO configs VALUES
    (
	'flag',
	'ontom_configured',
	'BOOL',
	'false',
	'N/A',
	'true',
	'false',
	'false',
	'false',
	'false',
	'false',
	'0000-00-00 00:00:00',
	'系统初次启动配置项',
	'false',
	'N/A',
	'N/A'
    );
INSERT INTO configs VALUES
    (
	'user',
	'comm_methord_module',
	'OPTION',
	'S-01',
	'S-01:M-01',
	'N/A',
	'N/A',
	'S-01',
	'S-01',
	'true',
	'true',
	'0000-00-00 00:00:00',
	'系统通信模型选择',
	'true',
	'系统通信模型',
	'一体式充电桩使用U-01(充电机[485], 后台[CAN]),分体式充电桩选择S-01(充电机[485], 后台[集中器485])'
    );
INSERT INTO configs VALUES
    (
	'system',
	'charge_module_nr',
	'INT',
	'14',
	'N/A',
	'14',
	'N/A',
	'14',
	'14',
	'true',
	'true',
	'0000-00-00 00:00:00',
	'充电模块个数',
	'true',
	'充电模块个数',
	'用于配置单体充电机的个数，根据系统实际配置进行设置'

    );
INSERT INTO configs VALUES
    (
	'user',
	'charge_pipe_sn',
	'STRING',
	'unset',
	'N/A',
	'N/A',
	'N/A',
	'unset',
	'unset',
	'false',
	'true',
	'0000-00-00 00:00:00',
	'用户配置充电桩编号',
	'true',
	'充电桩编号',
	'用户配置根据自己的需求配置充电桩编号'

    );
INSERT INTO configs VALUES
    (
	'user',
	'monitor_comm_addr',
	'INT',
	'1',
	'N/A',
	'65535',
	'1',
	'1',
	'1',
	'true',
	'true',
	'0000-00-00 00:00:00',
	'充电桩通信地址',
	'true',
	'监控通信地址',
	'用户配置根据自己的需求配置充电桩的通信地址'

    );
INSERT INTO configs VALUES
    (
	'factory',
	'pipe_hw_sn',
	'STRING',
	'unset',
	'N/A',
	'N/A',
	'1',
	'unset',
	'unset',
	'false',
	'true',
	'0000-00-00 00:00:00',
	'充电桩工厂序列号',
	'true',
	'监控工厂编号',
	'由生产部设置该值'
    );
INSERT INTO configs VALUES
    (
	'core',
	'core_simple_box_swap_time',
	'INT',
	'0',
	'N/A',
	'1000',
	'0',
	'0',
	'0',
	'false',
	'false',
	'0000-00-00 00:00:00',
	'采样盒485通信收发转换时间修正',
	'false',
	'采样盒通信时间调整',
	'解决485收发时间切换问题'
    );
INSERT INTO configs VALUES
    (
	'core',
	'core_charger_yaoce_0_49',
	'INT',
	'0',
	'N/A',
	'1000',
	'0',
	'0',
	'0',
	'false',
	'false',
	'0000-00-00 00:00:00',
	'充电屏监控遥信1通信收发转换时间修正',
	'false',
	'充电屏监控遥信1通信时间调整',
	'解决485收发时间切换问题'
    );
INSERT INTO configs VALUES
    (
	'core',
	'core_charger_yaoce_50_100',
	'INT',
	'0',
	'N/A',
	'1000',
	'0',
	'0',
	'0',
	'false',
	'false',
	'0000-00-00 00:00:00',
	'充电屏监控遥信2通信收发转换时间修正',
	'false',
	'充电屏监控遥信2通信时间调整',
	'解决485收发时间切换问题'
    );
INSERT INTO configs VALUES
    (
	'core',
	'core_charger_config',
	'INT',
	'0',
	'N/A',
	'1000',
	'0',
	'0',
	'0',
	'false',
	'false',
	'0000-00-00 00:00:00',
	'充电屏配置通信收发转换时间修正',
	'false',
	'充电屏监控配置通信时间调整',
	'解决485收发时间切换问题'
    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_conflict_map',
	'STRING',
	'NC;CN;',
	'N/A',
	'N/A',
	'N/A',
	'NC;CN',
	'NC;CN',
	'false',
	'false',
	'2015-01-21 11:21:23',
	'充电枪充电充电冲突配置表',
	'false',
	'冲突配置表',
	'用于配置2把枪或4把枪是否可以同时进行充电，N标识不冲突C标识冲突'

    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_config_gun_nr',
	'INT',
	'2',
	'N/A',
	'4',
	'0',
	'2',
	'2',
	'false',
	'false',
	'2015-01-21 11:26:43',
	'系统配置充电枪个数',
	'false',
	'充电枪个数',
	'用户配置系统中存在的充电枪个数'
    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_can_name',
	'STRING',
	'can0;can1;',
	'N/A',
	'N/A',
	'N/A',
	'can0;can1;',
	'can0;can1;',
	'false',
	'false',
	'2015-01-21 11:29:02',
	'BMS通信用CAN设备文件名称',
	'false',
	'BMS通讯设备',
	'若系统支持同时对多辆车进行充电那么就需要支持多个CAN设备'

    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_simple_box_nr',
	'STRING',
	'1',
	'N/A',
	'4',
	'0',
	'1',
	'1',
	'false',
	'false',
	'2015-01-21 11:31:02',
	'采样盒个数',
	'false',
	'综合采样盒个数',
	'系统中现有综合采样盒个数'
    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_charge_group_nr',
	'STRING',
	'1',
	'N/A',
	'4',
	'0',
	'1',
	'1',
	'false',
	'false',
	'2015-01-21 11:31:02',
	'充电机分组数',
	'false',
	'充电机组数',
	'系统进行多机同时充电时充电机组数'
    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_rs485_dev_nr',
	'STRING',
	'1',
	'N/A',
	'4',
	'0',
	'1',
	'1',
	'false',
	'false',
	'2015-01-21 12:19:02',
	'RS485串口个数',
	'false',
	'通信串口个数',
	'系统进行多机同时充电时485串口个数'

    );
INSERT INTO configs VALUES
    (
	'factory',
	'sys_uart_name',
	'STRING',
	'/dev/ttyO4;/dev/ttyO5;',
	'N/A',
	'N/A',
	'N/A',
	'/dev/ttyO4;/dev/ttyO5;',
	'/dev/ttyO4;/dev/ttyO5;',
	'false',
	'false',
	'2015-01-21 12:19:02',
	'RS485串口文件名',
	'false',
	'通信串口文件名',
	'系统进行多机同时充电时485串口文件名'

    );
CREATE TABLE bms_can_pack_generator(
	bms_can_stage string,
	bms_can_mnemonic string,
	bms_can_name string,
	bms_can_pgn string,
	bms_can_pgn_hex string,
	bms_can_prioriy string,
	bms_can_datalen string,
	bms_can_period string,
	bms_can_tolerate_silence,
	bms_can_source string,
	bms_can_status string
	);
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_HAND_SHAKING',
	'CRM',
	'充电机辨识报文',
	'256',
	'0x000100',
	'6',
	'8',
	'250',
	'250',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_HAND_SHAKING',
	'BRM',
	'BMS辨识报文',
	'512',
	'0x000200',
	'6',
	'41',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CONFIGURATION',
	'BCP',
	'动力蓄电池充电参数',
	'1536',
	'0x000600',
	'6',
	'13',
	'500',
	'500',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CONFIGURATION',
	'CTS',
	'充电机发送时间同步信息',
	'1792',
	'0x000700',
	'6',
	'7',
	'500',
	'500',
	'C2B',
	'DISABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CONFIGURATION',
	'CML',
	'充电机最大输出能力',
	'2048',
	'0x000800',
	'6',
	'6',
	'250',
	'250',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CONFIGURATION',
	'BRO',
	'电池充电准备就绪状态',
	'2304',
	'0x000900',
	'4',
	'1',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CONFIGURATION',
	'CRO',
	'充电机充电准备就绪状态',
	'2560',
	'0x000A00',
	'4',
	'1',
	'250',
	'250',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BCL',
	'电池充电需求',
	'4096',
	'0x001000',
	'6',
	'5',
	'50',
	'50',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BCS',
	'电池充电总状态',
	'4352',
	'0x001100',
	'6',
	'9',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'CCS',
	'充电机充电总状态',
	'4608',
	'0x001200',
	'6',
	'6',
	'50',
	'50',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BSM',
	'动力蓄电池状态信息',
	'4864',
	'0x001300',
	'6',
	'7',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BMV',
	'单体动力蓄电池电压',
	'5376',
	'0x001500',
	'6',
	'-1',
	'1000',
	'1000',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BMT',
	'单体动力蓄电池温度',
	'5632',
	'0x001600',
	'6',
	'-1',
	'1000',
	'1000',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BSP',
	'动力蓄电池保留报文',
	'5888',
	'0x001700',
	'6',
	'-1',
	'50',
	'50',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'BST',
	'BMS中止充电',
	'6400',
	'0x001900',
	'4',
	'4',
	'10',
	'10',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_CHARGE',
	'CST',
	'充电机中止充电',
	'6656',
	'0x001A00',
	'4',
	'4',
	'10',
	'10',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_END',
	'BSD',
	'BMS统计数据',
	'7168',
	'0x001C00',
	'6',
	'7',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_END',
	'CSD',
	'充电机统计数据',
	'7424',
	'0x001D00',
	'6',
	'5',
	'250',
	'250',
	'C2B',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_ANY',
	'BEM',
	'BMS错误报文',
	'7680',
	'0x001E00',
	'2',
	'4',
	'250',
	'250',
	'B2C',
	'ENABLE'
    );
INSERT INTO bms_can_pack_generator VALUES
    (
	'STAGE_ANY',
	'CEM',
	'充电机错误报文',
	'7936',
	'0x001F00',
	'2',
	'4',
	'250',
	'250',
	'C2B',
	'ENABLE'
    );
