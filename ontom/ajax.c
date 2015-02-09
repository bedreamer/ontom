#include "stdafx.h"

// 生成xml文件，并返回文件大小，头部由调用者生成
int ajax_deal_xml_proc(struct ajax_xml_struct *);
int ajax_charge_status_xml_proc(struct ajax_xml_struct *);
int ajax_battery_status_xml_proc(struct ajax_xml_struct *);
int ajax_alarm_xml_proc(struct ajax_xml_struct *);
int ajax_version_xml_proc(struct ajax_xml_struct *);
int ajax_autheticate_xml_proc(struct ajax_xml_struct *);
int ajax_query_xml_proc(struct ajax_xml_struct *thiz);
int ajax_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_system_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz);
int ajax_debug_list(struct ajax_xml_struct *thiz);
int ajax_debug_json_list(struct ajax_xml_struct *thiz);
int ajax_debug_commit(struct ajax_xml_struct *thiz);
int ajax_uart_debug_page(struct ajax_xml_struct *thiz);
int ajax_debug_bit_read(struct ajax_xml_struct *thiz);
int ajax_debug_bit_write(struct ajax_xml_struct *thiz);

// 充电任务操作接口
int ajax_job_create_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_delete_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_query_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_edit_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_abort_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_manpause_json_proc(struct ajax_xml_struct *thiz);
int ajax_job_resume_json_proc(struct ajax_xml_struct *thiz);

struct xml_generator {
	// xml 文件名
	char * xml_name;
	// xml 生成过程地址
    int (*xml_gen_proc)(struct ajax_xml_struct *);
}xmls[]={
    {"/query.json",             ajax_query_json_proc},
    {"/query.xml",              ajax_query_xml_proc},
    {"/deal.xml",               ajax_deal_xml_proc},
    {"/chargestatus.xml",       ajax_charge_status_xml_proc},
    {"/battrystatus.xml",       ajax_battery_status_xml_proc},
    {"/alarm.xml",              ajax_alarm_xml_proc},
    {"/version.xml",            ajax_version_xml_proc},
    {"/autheticate.xml",        ajax_autheticate_xml_proc},
    {"/confirm.xml",            ajax_confirm_charge_xml_proc},
    {"/system/query.json",      ajax_system_query_json_proc},

    // 充电作业调用接口
    {"/job/create.json",        ajax_job_create_json_proc},
    {"/job/delete.json",        ajax_job_delete_json_proc},
    {"/job/query.json",         ajax_job_query_json_proc},
    {"/job/edit.json",          ajax_job_edit_json_proc},
    {"/job/abort.json",         ajax_job_abort_json_proc},
    {"/job/manpause.json",      ajax_job_manpause_json_proc},
    {"/job/resume.json",        ajax_job_resume_json_proc},

    // 调试接口
    {"/debug/list.html",        ajax_debug_list},
    {"/debug/list.json",        ajax_debug_json_list},
    {"/debug/commit.html",      ajax_debug_commit},
    {"/debug/singlebit/read.json",    ajax_debug_bit_read},
    {"/debug/singlebit/write.json",   ajax_debug_bit_write},
    // 生成串口通信统计页面
    {"/debug/uart.json",        ajax_uart_debug_page},
    {"", NULL}
};

// 生成xml文件
int ajax_gen_xml(struct ajax_xml_struct *thiz)
{
	if ( thiz == NULL ) return ERR_WRONG_PARAM;
	else {
        static struct xml_generator *cursor = xmls;

        if ( cursor->xml_gen_proc == NULL )
            cursor = xmls;

        if ( 0 == strcmp(thiz->xml_name, cursor->xml_name) )
            return cursor->xml_gen_proc(thiz);

        cursor = xmls;

        for ( ; cursor->xml_gen_proc ; cursor ++ ) {
            log_printf(DBG_LV0, "%s:%s", thiz->xml_name, cursor->xml_name);
            if ( 0 != strcmp(thiz->xml_name, cursor->xml_name) ) continue;
            return cursor->xml_gen_proc(thiz);
		}

		return ERR_FUN_FAILE;
	}
}

// 生成触发充电任务的卡信息
static inline int xml_gen_triger_card(char *buff)
{
    return sprintf(buff,
                   "<triger>\r\n"
                   "  <cardid>%s</cardid>\r\n"
                   "  <valid>%s</valid>\r\n"
                   "  <remaind>%s</remaind>\r\n"
                   "</triger>\r\n",
                   config_read("triger_card_sn"),
                   "yes",
                   config_read("card_remaind_money")
    );
}

// 生成确认充电任务的卡信息
static inline int xml_gen_confirm_card(char *buff)
{
    return sprintf(buff,
                   "<confirm>\r\n"
                   "  <cardid>%s</cardid>\r\n"
                   "  <valid>%s</valid>\r\n"
                   "  <remaind>%s</remaind>\r\n"
                   "</confirm>\r\n",
                   config_read("confirm_card_sn"),
                   "yes",
                   config_read("card_remaind_money")
    );
}

// 生成结账任务的卡信息
static inline int xml_gen_settle_card(char *buff)
{
    return sprintf(buff,
                   "<settle>\r\n"
                   "  <cardid>%s</cardid>\r\n"
                   "  <valid>%s</valid>\r\n"
                   "  <super>%s</super>\r\n"
                   "  <remaind>%s</remaind>\r\n"
                   "</settle>\r\n",
                   config_read("settle_card_sn"),
                   "yes",
                   "no",
                   config_read("card_remaind_money")
    );
}

// 生成当前系统故障
// 最多16条故障
static inline int xml_gen_system_error(char *buff)
{
    int err_cnt = 0;
    char errbuff[2048];
#define MAX_ERR  16
#if 0
    if ( magic++ % 3 == 0 ) {
        return sprintf(buff,
                       "<error>\r\n"
                       " <total>\r\n"
                       "   <fault>3</fault>\r\n"
                       "   <warning>0</warning>\r\n"
                       " </total>\r\n"
                       " <fault>\r\n"
                       "    <e0>\r\n"
                       "       <code>998</code>\r\n"
                       "       <comment>BMS 未就绪.</comment>\r\n"
                       "    </e0>\r\n"
                       "    <e1>\r\n"
                       "       <code>997</code>\r\n"
                       "       <comment>充电机未就绪.</comment>\r\n"
                       "    </e1>\r\n"
                       "    <e2>\r\n"
                       "       <code>999</code>\r\n"
                       "       <comment>充电枪连接故障.</comment>\r\n"
                       "    </e2>\r\n"
                       " </fault>\r\n"
                       "</error>\r\n"
                       );
    } else return 0;
#endif

    if ( err_cnt ) {
    return sprintf(buff, "<error>\r\n"
                  "<total>"
                  "%d"
                  "</total>"
                  "%s"
                  "</error>\r\n",
            err_cnt,
            errbuff);
    } else return 0;
}

/* 充电任务事件查询
 * 不同的使用阶段需要检查不同的字段
 * <start>..</start>:
 *    刷卡触发充电服务响应。 可以是自动或是选择充电方式后获得
 * <confirm>..</confirm>:
 *    充电服务触发后确认开始充电服务。即将开始计费
 * <settle_accounts>..</settle_accounts>:
 *    结账刷卡。达到结束充电条件后触发。
 *               首页/按金钱/按时间/按容量充电页面
 *  UI ------- GET /query.xml?mode=auto ------->>> ontom   没有刷卡
 *  or UI ---- GET /query.xml?mode=asmoney ---->>> ontom   没有刷卡
 *  or UI ---- GET /query.xml?mode=astime ----->>> ontom   没有刷卡
 *  or UI ---- GET /query.xml?mode=ascap ------>>> ontom   没有刷卡
 *  UI <<<--------RETURN /query.xml -------------- ontom   没有刷卡
 *  UI ------- GET /query.xml?mode=auto ------->>> ontom   刷   卡
 *  or UI ---- GET /query.xml?mode=asmoney ---->>> ontom   刷   卡
 *  or UI ---- GET /query.xml?mode=astime ----->>> ontom   刷   卡
 *  or UI ---- GET /query.xml?mode=ascap ------>>> ontom   刷   卡
 *  UI <<<---------RETURN /query.xml ------------- ontom   有 刷卡
 *  UI ----GET /query.xml?start=yes&bm=auto --->>> ontom   有 刷卡
 *                    页面跳转至充电确认页面
 * 完整的定量充电刷卡流程如下：
 * UI定时向ontom发送 GET /qerycard.xml请求，并附带当前的请求模式（自动，按金额，时间，容量）
 * ontom返回当前的刷卡状态，如果有刷卡，则返回刷卡卡号，卡内余额等信息，没有则返回卡号为 N/A.
 *
 * UI 根据ontom返回的xml判定刷卡卡号是否有效，例如：
 *  自动模式下收到有效刷卡卡号后下一次请求URL为：
 *  GET /query.xml?mode=auto&triger=valid
 * 如果是按条件充电则现需要设定有效参数，并被ontom接受，因此，可以按如方式发送请求：
 *  GET /query.xml?mode=asmoney&money=100.0 或者
 *  GET /query.xml?mode=astime&time=503 或者
 *  GET /query.xml?mode=ascap&cap=89
 * 按上述方式发送请求收到query.xml后需要解析<param_accept>字段， 若该字段为yes,
 * 则表示ontom接受该设置请求，否则需要修改设置值，此时也可以刷卡，但不能触发triger字段设为valid
 * 当收到query.xml 解析的param_accept字段为yes后方可，根据刷卡的ID设定triger 是否为
 * valid, 当解析出param_accept字段为yes后，发送的请求可以如下：
 *  GET /query.xml?mode=asmoney&money=100.9&triger=valid 或者
 *  GET /query.xml?mode=astime&time=102&triger=valid 或者
 *  GET /query.xml?mode=ascap&cap=98&triger=valid
 */
int ajax_query_xml_proc(struct ajax_xml_struct *thiz)
{
#if 0
    char mode[8 + 1] = {0}, start[8 + 1] = {0};
    const char *begin = config_read("begin_card_sn");
    const char *confirm = config_read("confirm_card_sn");
    const char *settle = config_read("end_card_sn");
    const char *godcard = config_read("super_card_sn");
    const char *yes_or_no[] = {"no", "yes"};
    int begin_valid = 1, confirm_valid = 1, settle_valid = 1;
    int super = 0, timeout = 0;

    if ( 0 == strcmp(begin, "N/A") ) begin_valid = 0;
    if ( 0 == strcmp(confirm, "N/A") ) confirm_valid = 0;
    if ( 0 == strcmp(settle, "N/A") ) settle_valid = 0;

    if ( 0 == strcmp(godcard, "N/A") ) super = 0;
    else if ( 0 == strcmp(settle, godcard) ) super = 1;

    thiz->xml_len = sprintf(thiz->iobuff,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
        "<tom>\r\n"
        "<triger>\r\n"
        "  <valid>%s</valid>\r\n"
        "  <cardid>%s</cardid>\r\n"
        "  <remaind>4567.9</remaind>\r\n"
        "</triger>\r\n"
        "<confirm>\r\n"
        "  <valid>%s</valid>\r\n"
        "  <cardid>%s</cardid>\r\n"
        "  <remaind>FFFFFFFF</remaind>\r\n"
        "</confirm>\r\n"
        "<settle_accounts>\r\n"
        "  <valid>%s</valid>\r\n"
        "  <cardid>%s</cardid>\r\n"
        "  <remaind>FFFFFFFF</remaind>\r\n"
        "  <god>%s</god>\r\n"
        "  <timeout>%s</timeout>\r\n"
        "</settle_accounts>\r\n"
        "</tom>\r\n"
        "\r\n",
        yes_or_no[ begin_valid ], begin,
        yes_or_no[ confirm_valid ], confirm,
        yes_or_no[ settle_valid ], settle,
        yes_or_no[ super ], yes_or_no[ timeout ]
        );

    mg_get_var(thiz->xml_conn, "mode", mode, 8);
    mg_get_var(thiz->xml_conn, "start", start, 8);

    log_printf(DBG, "card.xml?mode=<%s>&start=<%s>", mode, start);

    return ERR_OK;
//#else
    // 充电模式声明
    char mode[ 8 + 1 ] = {0};
    // 刷卡是否有效
    char triger[ 8 + 1 ] = {0}, confirm[ 8 + 1 ] = {0}, endding[ 8 + 1 ] = {0};
    // 制定模式下的参数
    char themoney[ 8 + 1 ] = {0}, thetime[ 8 + 1 ] = {0}, thecap[ 8 + 1 ] ={0};
    // 刷卡有效
    enum {
          CARD_INVALID = 0x00,      // 无效刷卡
          CARD_TRIGER_VALID=0x01|0x80,   // 触发书卡有效
          CARD_CONFIRM_VALID=0x02|0x80,  // 确认充电刷卡有效
          CARD_SETTLE_VALID=0x03|0x80,   // 结账刷卡有效
          CARD_TRIGER_INVALID=4, // 触发刷卡无效, 标识否定刷卡
          CARD_CONFIRM_INVALID=5,// 确认充电刷卡无效, 标识否定刷卡
          CARD_SETTLE_INVALID=6  // 结账刷卡无效, 标识否定刷卡
    } cardvalid;
    // 输出缓冲指针
    char *output = thiz->iobuff;
    // 当前输出缓冲长度
    int output_len = 0;
    // 参数错误标识
    int wrongparam = 0;

    cardvalid = CARD_INVALID;

    mg_get_var(thiz->xml_conn, "mode", mode, 8);
    mg_get_var(thiz->xml_conn, "triger", triger, 8);
    mg_get_var(thiz->xml_conn, "confirm", confirm, 8);
    mg_get_var(thiz->xml_conn, "settle", endding, 8);

    // 需要在URL中避免同时出现triger, confirm, endding 参数字段
    // 若出现[triger|confirm|endding]=valid则表示参数检查也是没有错误的
    // 在充电过程中，URL中可以不带参数选项，仅仅进行状态查询即可
    if (  0 == strcmp("valid", triger) )
        cardvalid = CARD_TRIGER_VALID;
    if ( 0 == strcmp("invalid", triger) )
        cardvalid = CARD_TRIGER_INVALID;
    if ( 0 == strcmp("valid", confirm) )
        cardvalid = CARD_CONFIRM_VALID;
    if ( 0 == strcmp("invalid", confirm) )
        cardvalid = CARD_CONFIRM_INVALID;
    if ( 0 == strcmp("valid", endding) )
        cardvalid = CARD_SETTLE_VALID;
    if ( 0 == strcmp("invalid", endding) )
        cardvalid = CARD_SETTLE_INVALID;

    output_len += sprintf(&output[output_len],
         "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<start>\r\n");
    // 首先生成系统错误，警告列表
    output_len += xml_gen_system_error(&output[output_len]);

    if ( 0 == strcmp("auto", mode) ) {
        output_len += sprintf(&output[output_len], "<auto>\r\n");
        output_len += xml_gen_triger_card(&output[output_len]);
        output_len += xml_gen_confirm_card(&output[output_len]);
        output_len += xml_gen_settle_card(&output[output_len]);

        task->this_job[0]->charge_billing.mode = BILLING_MODE_AS_AUTO;
        output_len += sprintf(&output[output_len], "</auto>\r\n");
    } else if ( 0 == strcmp("asmoney", mode) ) {
        output_len += sprintf(&output[output_len], "<asmoney>\r\n");
        output_len += xml_gen_triger_card(&output[output_len]);
        output_len += xml_gen_confirm_card(&output[output_len]);
        output_len += xml_gen_settle_card(&output[output_len]);

        task->this_job[0]->charge_billing.mode = BILLING_MODE_AS_MONEY;
        if ( mg_get_var(thiz->xml_conn, "money", themoney, 8) ) {
            double cash = atof(themoney);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            if ( cash > 0.0f && cash < 999.99f ) {
                task->this_job[0]->charge_billing.option.set_money = cash;
                output_len += sprintf(&output[output_len],
                                      "<param_accept>yes</param_accept>\r\n");
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
                output_len += sprintf(&output[output_len],
                                      "<param_accept>no</param_accept>\r\n");
                wrongparam = 1;
                log_printf(WRN,
                           "wrong money=<%s> fetched. respect 0.01 ~ 999.99",
                           themoney);
            }
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
            log_printf(INF, "AJAX: emputy query, do nothing.");
        }
        output_len += sprintf(&output[output_len], "</asmoney>\r\n");
    } else if ( 0 == strcmp("astime", mode) ) {
        output_len += sprintf(&output[output_len], "<astime>\r\n");
        output_len += xml_gen_triger_card(&output[output_len]);
        output_len += xml_gen_confirm_card(&output[output_len]);
        output_len += xml_gen_settle_card(&output[output_len]);

        task->this_job[0]->charge_billing.mode = BILLING_MODE_AS_TIME;
        if ( mg_get_var(thiz->xml_conn, "time", thetime, 8) ) {
            unsigned int minits = atoi(thetime);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            if ( minits > 0 && minits <= 600 ) {
                task->this_job[0]->charge_billing.option.set_time = minits;
                output_len += sprintf(&output[output_len],
                                      "<param_accept>yes</param_accept>\r\n");
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
                output_len += sprintf(&output[output_len],
                                      "<param_accept>no</param_accept>\r\n");
                wrongparam = 1;
                log_printf(WRN,"AJAX: wrong time=<%s> fetched. respect 1 ~ 600",
                           thetime);
            }
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
            log_printf(INF, "AJAX: emputy query, do nothing.");
        }
        output_len += sprintf(&output[output_len], "</astime>\r\n");
    } else if ( 0 == strcmp("ascap", mode) ) {
        output_len += sprintf(&output[output_len], "<ascap>\r\n");
        output_len += xml_gen_triger_card(&output[output_len]);
        output_len += xml_gen_confirm_card(&output[output_len]);
        output_len += xml_gen_settle_card(&output[output_len]);

        task->this_job[0]->charge_billing.mode = BILLING_MODE_AS_CAP;
        if ( mg_get_var(thiz->xml_conn, "cap", thecap, 8) ) {
            unsigned int caps = atoi(thecap);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            if ( caps > 0 && caps <= 100 ) {
                task->this_job[0]->charge_billing.option.set_cap = caps;
                output_len += sprintf(&output[output_len],
                                      "<param_accept>yes</param_accept>\r\n");
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
                output_len += sprintf(&output[output_len],
                                      "<param_accept>no</param_accept>\r\n");
                wrongparam = 1;
                log_printf(WRN, "AJAX: wrong cap=<%s> fetched. respect 1 ~ 100",
                           thecap);
            }
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
            log_printf(INF, "AJAX: emputy query, do nothing.");
        }
        output_len += sprintf(&output[output_len], "</ascap>\r\n");
    } else {
        // 如果出现不需要刷卡就能充电的需求，可以在这里作相应处理
    }

    // 状态机切换确认, 为了保证UI和ontom的状态机一致，必须使用该字段来进行状态同步
    if ( cardvalid & 0x80 ) {
        output_len += sprintf(&output[output_len], "<echo>copy</echo>\r\n");
    } else {
        output_len += sprintf(&output[output_len], "<echo>N/A</echo>\r\n");
    }

    // 充电任务，状态机转移
    switch ( cardvalid ) {
    case CARD_INVALID:
    break;
    case CARD_TRIGER_VALID:    // 触发刷卡有效, 充电任务触发
     // 前驱充电任务必须是充电任务触发等待状态，
     // 考虑到该事件可能会重复出现，所以
     if ( wrongparam ) {
         log_printf(ERR, "AJAX: gave a wrong param, system might be hacked.");
         log_printf(WRN, "AJAX: ues auto mode instead");
         // change to auto mode.
     }
     if ( task->charge_task_stat == CHARGE_STAT_TRIGER_PEDDING ) {
         // 条件符合，状态机发生转移, 进入充电确认等待阶段
         task->charge_task_stat = CHARGE_STAT_CONFIRM_PEDDING;
         log_printf(INF, "AJAX: charge task trigered, wait to confirm.");
     } else if ( task->charge_task_stat == CHARGE_STAT_CONFIRM_PEDDING ) {
         // 重入问题，可以忽略
         // ...
         log_printf(INF,
                    "AJAX: status machine sync, from CHARGE_STAT_TRIGER_PEDDING"
                    " to CHARGE_STAT_CONFIRM_PEDDING");
     } else {
         log_printf(ERR, "AJAX: charge task status machine crashed, errcode=100!!!!");
     }
     break;
    case CARD_CONFIRM_VALID:   // 确认充电刷卡有效, 充电准备触发
     // 前驱充电任务必须是确认等待状态
     if ( wrongparam ) {
         log_printf(ERR, "AJAX: gave a wrong param, system might be hacked.");
         log_printf(WRN, "AJAX: ues auto mode instead");
         // change to auto mode.
     }
     if ( task->charge_task_stat == CHARGE_STAT_CONFIRM_PEDDING ) {
        task->charge_task_stat = CHARGE_STAT_WAIT_BMS;
        log_printf(INF, "AJAX: charge task confirmed, wait to BMS establish.");
     } else if ( task->charge_task_stat == CHARGE_STAT_WAIT_BMS ) {
         // 重入问题，可以忽略
         // ...
         log_printf(INF,
                    "AJAX: status machine sync, from CHARGE_STAT_CONFIRM_PEDDING"
                    " to CHARGE_STAT_WAIT_BMS");
     } else {
         log_printf(ERR, "AJAX: charge task status machine crashed, errcode=101!!!!");
     }
     break;
    case CARD_SETTLE_VALID:    // 结账刷卡有效, 充电结账触发
     // 前驱充电任务必须是充电计费等待阶段
     if ( task->charge_task_stat == CHARGE_STAT_BILLING_PEDDING ) {
        task->charge_task_stat = CHARGE_STAT_EXIT;
        log_printf(INF, "AJAX: charge task settled, charge task will exit normally.");
     } else if ( task->charge_task_stat == CHARGE_STAT_BILLING_PEDDING ) {
         // 重入问题，可以忽略
         // ...
     } else if (task->charge_task_stat == CHARGE_STAT_WAIT_BMS ) {
        // BMS 还未就绪，此时刷卡表示终止充电任务
         task->charge_task_stat = CHARGE_STAT_ABORT;
         log_printf(INF, "AJAX: charge task abort! triger: <%s>"
                    "confirm: <%s> abort: <%s>",
                    config_read("triger_card_sn"),
                    config_read("confirm_card_sn"),
                    config_read("settle_card_sn"));
     } else if (task->charge_task_stat == CHARGE_STAT_CHARGING) {
         // 正在充电，手动终止充电
         task->charge_task_stat = CHARGE_STAT_ABORT;
         log_printf(INF, "AJAX: 手动中止充电.");
     } else if (task->charge_task_stat == CHARGE_STAT_DONE) {
         // 充电正常结束，刷卡完成付款
         task->charge_task_stat = CHARGE_STAT_EXIT;
         log_printf(INF, "AJAX: 充电结束，费用结算.");
     } else if (task->charge_task_stat == CHARGE_STAT_ABORT) {
         // 重入问题，可以忽略
         // ...
     } else {
         log_printf(ERR, "AJAX: charge task status machine crashed, errcode=102!!!!");
     }
     break;
    case CARD_TRIGER_INVALID:  // 触发刷卡无效, 标识否定刷卡
     // 充电桩被禁用，或是所刷卡被冻结
     log_printf(WRN, "AJAX: charger disabled or card freezed!");
     break;
    case CARD_CONFIRM_INVALID: // 确认充电刷卡无效, 标识否定刷卡
     // 必须在充电确认状态时，才有效
     if ( task->charge_task_stat == CHARGE_STAT_CONFIRM_PEDDING ) {
        // 做出错处理
         log_printf(WRN, "AJAX: invalid card information. confirm failed.");
     } else {
         log_printf(ERR, "AJAX: charge task status machine crashed, errcode=103!!!!");
     }
     break;
    case CARD_SETTLE_INVALID:  // 结账刷卡无效, 标识否定刷卡
     // 必须在充电结账时才有效
     if ( task->charge_task_stat == CHARGE_STAT_BILLING_PEDDING ) {
        // 做响应出错处理
         log_printf(WRN, "AJAX: invalid card information. settle failed.");
     } else {
         log_printf(ERR, "AJAX: charge task status machine crashed, errcode=104!!!!");
     }
     break;
    }

     output_len += sprintf(&output[output_len], "</start>\r\n");
     thiz->xml_len = output_len;
     return ERR_OK;
#endif
     return ERR_ERR;
}

/* 充电确认
 * 最先获得刷卡事件的应该是监控程序，而非UI程序
 * 因此在这里需要注意一下：
 * 确认充电应该在监控程序探测到正常的刷卡事件，并且UI程序主动获取到该事件后
 * 再次由UI程序发送开始充电命令
 *
 *                      充电确认页面
 *  UI --------- GET /confirm.xml?start=no ------->>> ontom  没有刷卡  没有开始充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  没有刷卡  没有开始充电
 *  UI --------- GET /confirm.xml?start=no ------->>> ontom  刷卡     没有开始充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  有刷卡    没有开始充电
 *  UI ----GET /confirm.xml?start=yes&bm=auto ---->>> ontom  有刷卡    开始自动充电
 *  or UI ---GET /confirm.xml?start=yes&bm=money-->>> ontom  有刷卡    开始按金钱充电
 *  or UI ---GET /confirm.xml?start=yes&bm=time -->>> ontom  有刷卡    开始时间充电
 *  or UI ---GET /confirm.xml?start=yes&bm=cap --->>> ontom  有刷卡    开始容量充电
 *  UI <<<----------RETURN /confirm.xml ------------- ontom  有刷卡    开始充电
 *                    页面跳转至充电页面
 */
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz)
{
    thiz->ct = "text/xml";
    return ERR_ERR;
}

// 返回当前交易信息 deal.xml
int ajax_deal_xml_proc(struct ajax_xml_struct *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    thiz->ct = "text/xml";
    return ERR_ERR;
}

// 返回充电状态信息 chargestatus.xml
int ajax_charge_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    thiz->ct = "text/xml";
    return ERR_ERR;
}

// 返回电池状态信息 batterystatus.xml
int ajax_battery_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    thiz->ct = "text/xml";
    return ERR_ERR;
}

// 返回告警信息 alarm.xml
int ajax_alarm_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    thiz->ct = "text/xml";
    return ERR_ERR;
}

// 返回版本信息 version.xml
int ajax_version_xml_proc(struct ajax_xml_struct *thiz)
{
    thiz->ct = "text/xml";
    thiz->xml_len = sprintf(thiz->iobuff,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
        "<VERSION>"
        "<THTTPD>%s</THTTPD>"
        "<BROWSER>%s</BROWSER>"
        "<TOM>%s</TOM>"
        "<GOD>%s</GOD>\r\n"
        "</VERSION>\r\n"
        "\r\n",
        config_read("version_httpd"),
        config_read("version_browser"),
        config_read("version_tomd"),
        config_read("version_godd")
    );
    log_printf(DBG, "gen done");
	return ERR_OK;
}

// 操作认证 autheticate.xml
int ajax_autheticate_xml_proc(struct ajax_xml_struct *thiz)
{
    int auth_ok = 1, param_check_ok = 1;
    char passwd[46] = {0}, passwd_const[46] ={0}, reason[16] = {0};
    const char *solt_head = "PzsWmAPT3G";
    const char *solt_tail = "a7BY5tzfdX";

    mg_get_var(thiz->xml_conn, "passwd", passwd, 46);
    mg_get_var(thiz->xml_conn, "reason", reason, 16);

    log_printf(DBG, "%s-%s-%s",
        config_read("manual_passwd"),
        config_read("system_passwd"),
        config_read("manufacturer_passwd"));

    if ( strcmp(reason, "user") == 0 ) {
        sprintf(passwd_const, "%s%s%s",
                solt_head, config_read("manual_passwd"), solt_tail);
    } else if ( strcmp(reason, "system") == 0 ) {
        sprintf(passwd_const, "%s%s%s",
                solt_head, config_read("system_passwd"), solt_tail);
    } else if ( strcmp(reason, "manufacturer") == 0 ) {
        sprintf(passwd_const, "%s%s%s",
                solt_head, config_read("manufacturer_passwd"), solt_tail);
    } else {
        // 非法验证
        auth_ok = 0;
        param_check_ok = 0;
    }

    if ( strcmp(passwd, passwd_const) != 0 ) {
        auth_ok = 0;
    }
    log_printf(DBG, "%s<===>%s", passwd, passwd_const);

    thiz->ct = "text/xml";
    thiz->xml_len = sprintf(thiz->iobuff,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
        "<start>\r\n"
        "  <ok>%s</ok>\r\n"
        "</start>\r\n"
        "\r\n",
        auth_ok && param_check_ok ? "permit" : "reject"
    );
    thiz->xml_len --;

    log_printf(INF, "for %s autheticate result: %s",
               reason, auth_ok && param_check_ok ? "permit" : "reject");

    return ERR_OK;
}

/*
 * 通过URL传参参数长度限制为2048字节，为了调试方便当前选用URL方式传参
 * JSON 返回数据包含的内容有： 故障，刷卡鉴权，状态，任务信息
 */
int ajax_query_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_ERR;
    thiz->ct = "application/json";
    return ret;
}

/*
 * 查询系统状态
 * ------------------------------------------
 * @ 版本 V1.0
 * @ 充电机是否故障
 * @ 母线电池绝缘故障
 * @ 1# 充电枪是否链接
 * @ 2# 充电枪是否链接
 * -------------------------------------------
 */
int ajax_system_query_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    static int doreset = 0;
    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 版本
            "\"version\":\"V1.0\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 版本
            "\"doreset\":\"%s\",", doreset ? "false" : "true");
    doreset = 1;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 系统状态
            "\"system_status\":\"正常\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 充电机状态
            "\"charger_status\":\"正常\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线绝缘状态
            "\"bus0_institude\":\"正常\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线绝缘状态
            "\"bus1_institude\":\"N/A\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电压
            "\"bus0_V\":\"%.1f V\",", b2l(task->chargers[0]->chargers.charger_v_out) / 10.0f);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电压
            "\"bus1_V\":\"N/A\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电流
            "\"bus0_I\":\"%.1f A\",", b2l(task->chargers[0]->chargers.charger_i_out) / 10.0f);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电流
            "\"bus1_I\":\"N/A\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 当前刷卡序列号
            "\"card_sn\":\"%s\",", config_read("triger_card_sn"));
    config_write("triger_card_sn", "N/A");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 如果刷卡了，当前卡内余额
            "\"card_remain\":\"0.00 V\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 1# 充电枪连接状态
            "\"gun0\":\"%s\",", bit_read(task, F_GUN_1_PHY_CONN_STATUS)?"已连接":"未连接");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 2# 充电枪连接状态
            "\"gun1\":\"%s\",", bit_read(task, F_GUN_2_PHY_CONN_STATUS)?"已连接":"未连接");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 3# 充电枪连接状态
            "\"gun2\":\"N/A\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"gun3\":\"N/A\",");
    thiz->iobuff[--thiz->xml_len] = '\0';
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "}");
    return ret;
}

// 充电任务操作接口
int ajax_job_create_json_proc(struct ajax_xml_struct *thiz)
{
#if 0
    int ret = ERR_ERR, i = S_ERROR;
    char buff[32] = "";
    //if ( task->this_job[0] == NULL ) {
    task->this_job[0] = & task->jobs[0];
    task->this_job[0]->job_status = JOB_STANDBY;
    ret = ERR_OK;
    thiz->xml_len = sprintf(&thiz->iobuff[thiz->xml_len],
            "\"jobs\":{\"nr\":1}");
    for (; i < S_ERR_END; i ++) {
        if ( bit_read(task->this_job[0], i) ) {
            sprintf(buff, "E%04X", i);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "%s ", config_read(buff));
        }
    }
    task->this_job[0]->job_gun_sn = GUN_SN0;
   // }
    return ret;
#else
    struct job_commit_data jc;
    char timestamp[32] = {0};
    char cid[32] = {0};
    char gun[8] = {0};
    char c_mode[16] = {0};
    char set_V[8] = {0};
    char set_I[8] = {0};
    char b_mode[16] = {0};
    char b_kwh[8] = {0};
    char b_money[8] = {0};
    char b_time[8] = {0};

    mg_get_var(thiz->xml_conn, "t", timestamp, 32);
    mg_get_var(thiz->xml_conn, "cid", cid, 32);
    mg_get_var(thiz->xml_conn, "gun", gun, 8);

    mg_get_var(thiz->xml_conn, "c_mode", c_mode, 16);
    if ( strlen(c_mode) ) {
        mg_get_var(thiz->xml_conn, "set_V", set_V, 8);
        mg_get_var(thiz->xml_conn, "set_I", set_I, 8);
    }

    mg_get_var(thiz->xml_conn, "b_mode", b_mode, 16);
    if ( strlen(b_mode) ) {
        mg_get_var(thiz->xml_conn, "b_kwh", b_kwh, 8);
        mg_get_var(thiz->xml_conn, "b_money", b_money, 8);
        mg_get_var(thiz->xml_conn, "b_time", b_time, 8);
    }

    jc.url_commit_timestamp = atoll(timestamp);
    jc.ontom_commit_date_time = time(NULL);
    jc.biling_mode = BILLING_MODE_AS_AUTO;
    jc.charge_mode = CHARGE_AUTO;

    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"id\":\"%08X\",", jc.url_commit_timestamp);

    if ( strlen(gun) <= 0 ) {
        log_printf(DBG_LV3, "充电枪编号错误");
        goto reject;
    }

    if ( strlen(cid) <= 0 ) {
        log_printf(DBG_LV3, "卡编号错误");
        goto reject;
    }

    strcpy(jc.card_sn, cid);

    switch (atoi(gun)) {
    case 0:
        jc.charge_gun = GUN_SN0;
        break;
    case 1:
        jc.charge_gun = GUN_SN1;
        break;
    case 2:
        jc.charge_gun = GUN_SN2;
        break;
    case 3:
        jc.charge_gun = GUN_SN3;
        break;
    default:
        goto reject;
    }
    log_printf(INF, "atoi(gun) jc.charge_gun %x", jc.charge_gun);

    if ( strlen(b_mode) > 0 ) {
        if ( 0 == strcmp(b_mode, "auto") ) {
            jc.biling_mode = BILLING_MODE_AS_AUTO;
        } else if ( 0 == strcmp(b_mode, "kwh") ) {
            jc.biling_mode = BILLING_MODE_AS_CAP;
        } else if ( 0 == strcmp(b_mode, "cap") ) {
            jc.biling_mode = BILLING_MODE_AS_CAP;
        } else if ( 0 == strcmp(b_mode, "time") ) {
            jc.biling_mode = BILLING_MODE_AS_TIME;
        } else if ( 0 == strcmp(b_mode, "money") ) {
            jc.biling_mode = BILLING_MODE_AS_MONEY;
        } else {
            log_printf(DBG_LV3, "错误的计费模式");
            goto reject;
        }
    }

    if ( strlen(c_mode) > 0 ) {
        if (  0 == strcmp(c_mode, "auto") ) {
            jc.charge_mode = CHARGE_AUTO;
        } else if ( 0 == strcmp(c_mode, "BV") ) {
            jc.charge_mode = CHARGE_BV;
        } else if ( 0 == strcmp(c_mode, "BI") ) {
            jc.charge_mode = CHARGE_BI;
        } else {
            log_printf(DBG_LV3, "错误的充电模式");
            goto reject;
        }
    }

    switch ( jc.biling_mode ) {
    case BILLING_MODE_AS_AUTO:
        break;
    case BILLING_MODE_AS_CAP:
        if ( 0 == strlen(b_kwh) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        jc.as_kwh = atof(b_kwh);
        if ( jc.as_kwh < 0.09999999f ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        break;
    case BILLING_MODE_AS_MONEY:
        if ( 0 == strlen(b_money) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        jc.as_money = atof(b_money);
        if ( jc.as_money < 0.09999999f ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        break;
    case BILLING_MODE_AS_TIME:
        if ( 0 == strlen(b_time) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        jc.as_time = atoi(b_time);
        if ( jc.as_time < 1 ) {
            log_printf(DBG_LV3, "错误的计费参数");
            goto reject;
        }
        break;
    default:
        break;
    }

    //log_printf(INF, "jc.charge_mode == CHARGE_MANUAL %x", jc.charge_mode);
    if ( jc.charge_mode != CHARGE_AUTO ) {
        if ( strlen(set_V) <= 0 ) {
            log_printf(DBG_LV3, "1错误的充电参数");
            goto reject;
        }
        if ( strlen(set_I) <= 0 ) {
            log_printf(DBG_LV3, "2错误的充电参数");
            goto reject;
        }
        jc.manual_set_charge_volatage = atof(set_V);
        jc.manual_set_charge_current = atof(set_I);
        if ( jc.manual_set_charge_volatage < 50.0f ) {
            log_printf(DBG_LV3, "3错误的充电参数");
            goto reject;
        }
        if ( jc.manual_set_charge_current < 1.0f ) {
            log_printf(DBG_LV3, "4错误的充电参数");
            goto reject;
        }
    }

    if ( task->wait_job_nr < 16 ) {
        if ( job_exsit(jc.url_commit_timestamp) ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":\"OK\"}");
        } else {
            job_commit(task, &jc, COMMIT_CMD_FORK);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":\"PENDING\"}");
        }
    } else {
        log_printf(DBG_LV3, "作业数太多");
        goto reject;
    }
    return ERR_OK;
reject:
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"status\":\"REJECTED\"}");
    return ERR_OK;
#endif
}

int ajax_job_delete_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_ERR;
    thiz->ct = "application/json";

    return ret;
}

void job_query_json_fromat(struct ajax_xml_struct *xml, struct charge_job *job)
{
    char *status_string[] = {
        "空闲",
        "设置",
        "等待",
        "就绪",
        "在充",
        "因故暂停",
        "人工暂停",
        "正在恢复",
        "正在中止",
        "作业完成",
        "正在退出",
        "正在清除"
    };
    char *cmode_string[] = {
        "自动",
        "手动"
    };
    char *bmode_string[] = {
        "无效",
        "自动",
        "按金额",
        "按时间",
        "按电量",
        "自由"
    };
    CHARGE_GUN_SN g = __is_gun_phy_conn_ok(job);
    char buff[16] = {0};
    if ( g == GUN_SN0 ) {
        sprintf(buff, "枪%d已连接", job->job_gun_sn);
    } else if ( g == GUN_SN1 ) {
        sprintf(buff, "枪%d已连接", job->job_gun_sn);
    } else {
        sprintf(buff, "枪%d未连接", job->job_gun_sn);
    }

    xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
            "{\"status\":\"%s\","    // 状态
            "\"id\":\"%08X\","       // 作业ID，序号
            "\"port\":\"%ld#\","     // 充电端口
            "\"cmode\":\"%s\","       // 充电模式
            "\"bmode\":\"%s\","       // 计费方式
            "\"cid\":\"%s\","       // 卡号ID
            "\"cremain\":\"%.2f 元\","       // 余额
            "\"CV\":\"%.1f V\","       // 充电电压
            "\"CI\":\"%.1f A\","       // 充电电流
            "\"ycdl\":\"%s %%\","       // 已充电量
            "\"gun_stat\":\"%s\""       // 充电枪连接状态
            "},",
            status_string[job->job_status],
            job->job_url_commit_timestamp,
            job->job_gun_sn,
            cmode_string[job->charge_mode],
            bmode_string[job->charge_billing.mode],
            job->card.triger_card_sn,
            0.00f,
            0.0f,
            0.0f,
            "N/A",
            buff
            );
}

int ajax_job_query_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK, i;
    struct list_head *h;
    struct charge_job *job;
    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"jobs\":[");
    for ( i = 0; i < CONFIG_SUPPORT_CHARGE_JOBS; i ++ ) {
        if ( task->job[i] == NULL ) continue;
        job_query_json_fromat(thiz, task->job[i]);
    }
    h = task->wait_head;
    if ( h ) {
        pthread_mutex_lock(&task->wait_lck);
        do {
            job = list_load(struct charge_job, job_node, h);
            job_query_json_fromat(thiz, job);
            h = h->next;
        } while ( h != task->wait_head );
        thiz->iobuff[--thiz->xml_len] = '\0';
        pthread_mutex_unlock (&task->wait_lck);
    }
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    return ret;
}

int ajax_job_edit_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_ERR;
    thiz->ct = "application/json";
    return ret;
}

int ajax_job_abort_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char id[16] = {0};
    struct charge_job * j = NULL;
    thiz->ct = "application/json";

    mg_get_var(thiz->xml_conn, "id", id, 16);
    j = job_search(__atoh(id));

    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"id\":\"%s\"", id);

    if ( j == NULL ) {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"REJECTED\"", id);
    } else {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"OK\"", id);
        j->job_status = JOB_ABORTING;
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

    return ret;
}

int ajax_job_manpause_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    thiz->ct = "application/json";
    thiz->xml_len = 1;
    thiz->iobuff[0] = 'P';
    return ret;
}

int ajax_job_resume_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    thiz->ct = "application/json";
    thiz->xml_len = 1;
    thiz->iobuff[0] = 'R';
    return ret;
}

int ajax_debug_bit_read(struct ajax_xml_struct *thiz)
{
    char var[32]={0};
    int index = 0, i;
    int ret = ERR_OK;

    mg_get_var(thiz->xml_conn, "var", var, 32);
    thiz->ct = "application/json";
    index = atoi(var);
    if ( index >=0 && index < FLAG_END ) {
        for (i=0;i<512;i++)
         thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "%d", bit_read(task, i));
    } else {
        ret = ERR_ERR;
    }
    return ret;
}

int ajax_debug_bit_write(struct ajax_xml_struct *thiz)
{
    char var[32]={0}, val[32] = {0};
    int index = 0;
    int ret = ERR_OK;

    mg_get_var(thiz->xml_conn, "var", var, 32);
    mg_get_var(thiz->xml_conn, "val", val, 32);
    thiz->ct = "application/json";

    index = atoi(var);
    if ( index >=0 && index < FLAG_END ) {
        if ( val[0] == '0' ) {
            bit_clr(task, index);
        } else if ( val[0] == '1') {
            bit_set(task, index);
        }
    } else {
        ret =  ERR_ERR;
    }

    thiz->xml_len = sprintf(&thiz->iobuff[thiz->xml_len],
            "%d", bit_read(task, index));
    return ret;
}

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
    int result = MG_FALSE, err;
    struct ajax_xml_struct thiz;
    memset(&thiz, 0, sizeof(thiz));
    thiz.xml_conn = conn;
    if (ev == MG_REQUEST) {
        log_printf(DBG_LV0, "%s&%s", conn->uri, conn->query_string);
        strncpy(thiz.xml_name, conn->uri, 31);
        thiz.ct = "text/xml";
        err = ajax_gen_xml( & thiz );
        if ( err == ERR_OK ) {
            log_printf(DBG_LV0, "prepare stage1 done...");
            mg_printf(conn,
                      "HTTP/1.1 200 HTTP\r\n"
                      "Server: thttpd/2.21b PHP/20030920\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Content-Type: %s\r\n"
                      "Date: Wed, 20 Aug 2014 03:29:12 GMT\r\n"
                      "Last-Modified: Tue, 19 Aug 2014 09:23:50 GMT\r\n"
                      "Accept-Ranges: bytes\r\n"
                      "Content-Length: %d\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n", thiz.ct, thiz.xml_len);
            log_printf(DBG_LV0, "prepare stage2 done...");
            mg_write(conn, thiz.iobuff, thiz.xml_len);
            log_printf(DBG_LV0, "send file done...");
            //log_printf(DBG, "done %d", thiz.xml_len);
        } else {
            mg_printf(conn,
                      "HTTP/1.1 404 Not Found\r\n"
                      "Date: Fri, 22 Aug 2014 06:38:28 GMT\r\n"
                      "Server: Apache\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Keep-Alive: timeout=3\r\n"
                      "Connection: Keep-Alive\r\n"
                      "Content-Type: text/html\r\n"
                      "Content-Length: 328\r\n"
                      "\r\n"
                      "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                      "<html><head>\r\n"
                      "<title>404 Not Found</title>\r\n"
                      "</head><body>\r\n"
                      "<h1>Not Found</h1>\r\n"
                      "<p>The requested URL was not found on this server.</p>\r\n"
                      "<p>Additionally, a 404 Not Found\r\n"
                      "error was encountered while trying to use an ErrorDocument to handle the request.</p>\r\n"
                      "</body></html>\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
        }
        result = MG_TRUE;
    } else if (ev == MG_AUTH) {
        result = MG_TRUE;
    }

  return result;
}

// xml 服务线程
// 提供xml文件输出，设置参数输入服务
void *thread_xml_service(void *arg) ___THREAD_ENTRY___
{
    int *done = (int *)arg;
    int mydone = 0;
    struct mg_server *server;
    if ( done == NULL ) done = &mydone;

    log_printf(DBG_LV1, "%s running...", __FUNCTION__);
    server = mg_create_server(NULL, ev_handler);
    mg_set_option(server, "listening_port", "8081");

    //printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
    for (; ! *done; ) {
        mg_poll_server(server, 500);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}
