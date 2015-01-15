CREATE TABLE log(
	timestamp string,
	logstring string
	);

CREATE TABLE errors(
	error_seq_id unsigned int,
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
	'S-01|M-01',
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
	'一体式充电桩使用U-01(充电机[485], 后台[CAN]),'
	'分体式充电桩选择S-01(充电机[485], 后台[集中器485])'

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
