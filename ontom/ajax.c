#include "stdafx.h"
#include "ajax.h"
#include "charge.h"

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
    {"/system/error.json",      ajax_system_error_proc},
    {"/system/history.json",    ajax_system_history_proc},
    {"/system/about.json",      ajax_system_about_proc},
    {"/system/modules.json",    ajax_module_query_proc},
    {"/system/config.json",     ajax_system_config_proc},
    {"/system/options.json",    ajax_system_config_options_proc},
    {"/system/save.json",       ajax_system_config_save_proc},
    {"/system/detail.json",     ajax_system_detail_proc},
    {"/system/card.json",       ajax_card_init_proc,},
    {"/system/jiaozhun.json",   ajax_jiaozhun_proc,},
    {"/system/auth.json",       ajax_auth_proc,},
    {"/system/update.json",     ajax_update_proc,},
    {"/system/export.json",     ajax_export_proc,},

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
    char *p;
    thiz->ct = "application/json";
    char page_id[64], job_id[64];

    mg_get_var(thiz->xml_conn, "p", page_id, 64);
    mg_get_var(thiz->xml_conn, "j", job_id, 64);

    if ( 0 == strcmp(page_id, "id_mainpage") ) {
        task->uipage = UI_PAGE_MAIN;
    } else if (  0 == strcmp(page_id, "id_job_working") ) {
        if ( 0 != strcmp(job_id, "N/A") ) {
            task->ui_job_id = __atoh(job_id);
            if ( task->uipage != UI_PAGE_JOBS ) {
                log_printf(DBG_LV1, "AJAX: UI界面在任务详情页面");
            }
            task->uipage = UI_PAGE_JOBS;
        } else {
            task->uipage = UI_PAGE_OTHER;
        }
    } else {
        task->uipage = UI_PAGE_OTHER;
    }

    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 版本
            "\"version\":\"V%d\",", VERSION);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 版本
            "\"doreset\":%s,", doreset ? "false" : "true");
    doreset = 1;
    if ( bit_read(task, S_ERROR) ) {
        p = "故障";
    } else {
        p = "正常";
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 系统状态
            "\"system_status\":\"%s\",", p);
    if ( bit_read(task, S_CHARGE_GROUP_ERR) ) {
        p = "故障";
    } else {
        p = "正常";
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 充电机状态
            "\"charger_status\":\"%s\",", p);
    if ( bit_read(task, S_BAT_0_INSTITUDE) ) {
        p = "故障";
    } else {
        p = "正常";
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线绝缘状态
            "\"bus0_institude\":\"%s\",", p);
    if ( bit_read(task, S_BAT_1_INSTITUDE) ) {
        p = "故障";
    } else {
        p = "正常";
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Va\":\"%.1f V\",", task->meter[0].Va);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Vb\":\"%.1f V\",", task->meter[0].Vb);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Vc\":\"%.1f V\",", task->meter[0].Vc);

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线绝缘状态
            "\"bus1_institude\":\"%s\",", p);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电压
            "\"bus0_V\":\"%.1f V\",", __bytes2double(b2l(task->measure[0]->measure.VinKM0)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电压
            "\"bus1_V\":\"%.1f\",", __bytes2double(b2l(task->measure[0]->measure.VinKM1)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电流
            "\"bus0_I\":\"%.1f A\",", __bytes2double(b2l(task->measure[0]->measure.IoutBAT0)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电流
            "\"bus1_I\":\"%.1f\",", __bytes2double(b2l(task->measure[0]->measure.IoutBAT1)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 当前刷卡序列号
            "\"card_sn\":\"%s\",", config_read("triger_card_sn"));
    config_write("triger_card_sn", "N/A");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 如果刷卡了，当前卡内余额
            "\"card_remain\":\"%s\",", config_read("card_remaind_money"));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 如果刷卡了，当前卡内余额
            "\"card_status\":\"%s\",", config_read("card_status"));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 如果刷卡了，当前卡内余额
            "\"card_passwd\":\"%s\",", config_read("card_passwd"));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 1# 充电枪连接状态
            "\"gun0\":\"%s\",",
            bit_read(task, F_GUN_1_PHY_CONN_STATUS)?
                bit_read(task, CMD_GUN_1_OUTPUT_ON) ? "在充":"已连接":"未连接");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 2# 充电枪连接状态
            "\"gun1\":\"%s\",",
            bit_read(task, F_GUN_2_PHY_CONN_STATUS)?
                bit_read(task, CMD_GUN_2_OUTPUT_ON) ? "在充":"已连接":"未连接");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 3# 充电枪连接状态
            "\"gun2\":\"N/A\",");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"gun3\":\"N/A\",");

    // {{ 开关状态
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"zdcrd\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x01?"熔断":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"zdctrip\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x02?"跳闸":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"dc1trip\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x04?"跳闸":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"dc2trip\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x08?"跳闸":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"actrip\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x20?"跳闸":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"flq\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x10?"故障":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"halt\":\"%s\",", task->measure[0]->measure.Flag_prtc6&0x40?"急停":"正常");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"aczt\":\"%s\",", task->measure[0]->measure.Flag_run1&0x01?"合闸":"分闸");
    // }}

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
    char c_remain[16] = {0};
    char c_passwd[16] = {0};

    mg_get_var(thiz->xml_conn, "t", timestamp, 32);
    mg_get_var(thiz->xml_conn, "cid", cid, 32);
    mg_get_var(thiz->xml_conn, "gun", gun, 8);
    mg_get_var(thiz->xml_conn, "remain", c_remain, 32);
    mg_get_var(thiz->xml_conn, "passwd", c_passwd, 8);

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

    jc.card_remain = atof(c_remain);
    strncpy(jc.card_passwd, c_passwd, 16);
    jc.url_commit_timestamp = atoll(timestamp);
    jc.ontom_commit_date_time = time(NULL);
    jc.biling_mode = BILLING_MODE_AS_AUTO;
    jc.charge_mode = CHARGE_AUTO;

    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"id\":\"%08X\",", (unsigned int)jc.url_commit_timestamp);

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
        jc.as_time = atoi(b_time) * 60;
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

int sql_current_error_result(void *param, int nr, char **text, char **name)
{
    struct ajax_xml_struct *thiz = (struct ajax_xml_struct *)param;

    if ( nr <=0 ) return 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"estr\":\"%s\"},", text[0]);
    return 0;
}

// 返回当前故障
int ajax_system_error_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    struct error_history *te;
    struct list_head *head;
    char errname[32];
    char *errmsg;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"errors\":[");

    pthread_mutex_lock(&task->err_list_lck);
    if ( task->err_head != NULL ) {
        head = task->err_head;
        do {
            te = list_load(struct error_history, error_me, head);
            // ...
            sprintf(errname, "E%04X", te->error_id);
            char sql[256];

            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "{\"eid\":\"%d\","
                    "\"ebt\":\"%s\",",
                    te->error_id,
                    te->error_begin);
            sprintf(sql,
                    "select errordefine.comment from errordefine "
                    "where errordefine.dec_val='%d'", te->error_id);
            ret = sqlite3_exec(task->database, sql, sql_current_error_result, thiz, &errmsg);
            if ( ret ) {
                log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
                ret = ERR_ERR;
            }
            head = head->next;
        } while ( head != task->err_head );
    }
    pthread_mutex_unlock (&task->err_list_lck);
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    return ret;
}

int sql_history_result(void *param, int nr, char **text, char **name)
{
    struct ajax_xml_struct *thiz = (struct ajax_xml_struct *)param;
    if ( nr <=0 ) return 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"hid\":\"%s\",", text[0]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"eid\":\"%s\",", text[1]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"estr\":\"%s\",", text[5]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"ebt\":\"%s\",", text[2]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"rbt\":\"%s\"},", text[3]);

    return 0;
}

// 返回历史故障
int ajax_system_history_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    int lf = 0, nr = 12, n;
    char sql[256] = {0}, *errmsg, buff[32];
    thiz->ct = "application/json";

    mg_get_var(thiz->xml_conn, "p", buff, 8);
    n = atoi(buff);
    if ( n >= 0 ) {
        lf = n;
    }
    mg_get_var(thiz->xml_conn, "n", buff, 8);
    n = atoi(buff);
    if ( n > 0 ) {
        nr = n;
    }
    mg_get_var(thiz->xml_conn, "clean", buff, 8);
    if ( 0 == strcmp("true", buff) ) {
        // 清除历史故障
        sprintf(sql, "delete from errors");
        ret = sqlite3_exec(task->database, sql, NULL, NULL, &errmsg);
        if ( ret ) {
            log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
            ret = ERR_ERR;
        }

        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"history\":[");
        if ( ret ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "{\"result\":\"error\"}]}");
        } else {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "{\"result\":\"ok\"}]}");
            task->err_seq_id_next = 0;

            do {
                struct error_history *te;
                struct list_head *head;
                pthread_mutex_lock(&task->err_list_lck);
                if ( task->err_head != NULL ) {
                    head = task->err_head;
                    do {
                        te = list_load(struct error_history, error_me, head);
                        char sql[256];

                        te->error_seqid = task->err_seq_id_next ++;
                        sprintf(sql,
                                "INSERT INTO errors VALUES('%d','%d','%s','%s','ERROR')",
                                te->error_seqid,
                                te->error_id,
                                te->error_begin,
                                te->error_recover);
                        ret = sqlite3_exec(task->database, sql, sql_current_error_result, thiz, &errmsg);
                        if ( ret ) {
                            log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
                        }
                        head = head->next;
                    } while ( head != task->err_head );
                }
                pthread_mutex_unlock (&task->err_list_lck);
            } while (0);
        }

        ret = ERR_OK;
        return ret;
    }

    sprintf(sql,
            "select errors.*,errordefine.comment from errors,errordefine "
            "where errors.error_id=errordefine.dec_val limit %d,%d", lf, nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"history\":[");
    ret = sqlite3_exec(task->database, sql, sql_history_result, thiz, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
        ret = ERR_ERR;
    }
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    return ret;
}

int ajax_system_about_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    struct ifaddrs * ifa=NULL, *ifaddr = NULL;
    char host[128];

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"about\":{");

    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)  continue;
        if (ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4 is a valid IP4 Address
            getnameinfo(ifa->ifa_addr,
                        sizeof(struct sockaddr_in),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"%s\":\"%s\",",
                    ifa->ifa_name, host);
        }
    }
    freeifaddrs(ifaddr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"ver\":\"%d\",", VERSION);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"sys\":\"%s\",",
            task->sys_type == SYSTEM_YITISHI ? "一体式":
            task->sys_type == SYSTEM_FENTISHI ? "分体式" : "未知");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"gun_nr\":\"%d 把\",", task->sys_config_gun_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"module_nr\":\"%d 个\",", task->modules_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"section_nr\":\"%d 段\",", task->sys_charge_group_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"auth_stat\":\"%s\",", "已激活");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"auth_id\":\"%s\",", "ONAE-7655-7FD0-12EF-0999");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}}");
    return ret;
}

int ajax_module_query_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    int n;
    unsigned short kn;
    char buff[8];
    char *p = NULL;

    mg_get_var(thiz->xml_conn, "op", buff, 8);
    if ( 0 == strcmp(buff, "ON") ) {
        mg_get_var(thiz->xml_conn, "sn", buff, 8);
        n = atoi(buff);
        if ( n >= 0 && n < CONFIG_SUPPORT_CHARGE_MODULE) {
            task->modules_on_off[n] =  0x80;
            bit_set(task, CMD_MODULE_ON);
        }
    } else if ( 0 == strcmp(buff, "OFF") ) {
        mg_get_var(thiz->xml_conn, "sn", buff, 8);
        n = atoi(buff);
        if ( n >= 0 && n < CONFIG_SUPPORT_CHARGE_MODULE) {
            task->modules_on_off[n] =  0x81;
            bit_set(task, CMD_MODULE_OFF);
        }
    }

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"modules\":[");

    for ( n = 0; n < CONFIG_SUPPORT_CHARGE_MODULE && n < task->modules_nr; n ++ ) {

        kn = b2l(task->chargers[0]->chargers.charge_module_status[n/2]);
        if ( n % 2 ) {
            kn = kn >> 8;
        } else {
            kn = kn & 0xFF;
        }
        p = NULL;
        if ( task->modules_on_off[n] == 0x81 && !(kn>>4) ) {
            p = "正在关机";
        } else if (task->modules_on_off[n] == 0x80 && (kn>>4) ) {
             p = "正在开机";
        }


        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "{\"V\":\"%.1f V\","
                 "\"I\":\"%.1f A\","
                 "\"T\":\"%.1f ℃\","
                 "\"N\":\"\%d%d%d%d%d%d\","
                 "\"S\":\"%s\","
                 "\"OF\":\"%s\"},",
                b2l(task->chargers[0]->chargers.charge_module_v[n])/10.0f,
                b2l(task->chargers[0]->chargers.charge_module_i[n])/10.0f,
                b2l(task->chargers[0]->chargers.charge_module_t[n])/10.0f,
                task->chargers[0]->chargers.charge_module_sn[n][0]>>8,
                task->chargers[0]->chargers.charge_module_sn[n][0]&0xFF,
                task->chargers[0]->chargers.charge_module_sn[n][1]>>8,
                task->chargers[0]->chargers.charge_module_sn[n][1]&0xFF,
                task->chargers[0]->chargers.charge_module_sn[n][2]>>8,
                task->chargers[0]->chargers.charge_module_sn[n][2]&0xFF,
                (kn & 0xF) ? "故障":"正常" ,
                p ? p : (kn >> 4) ? "已关机" : "已开机"
                );
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    return ret;
}

int sql_system_settings_result(void *param, int nr, char **text, char **name)
{
    struct ajax_xml_struct *thiz = (struct ajax_xml_struct *)param;

    if ( 0 == strcmp(text[3], "options") ) {

        char *p = text[4];
        for ( ; *p; p ++ ) {
            if ( *p == '\"' ) *p = '@';
            if ( *p == '}' ) *p = '%';
            if ( *p == '{' ) *p = '#';
        }
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"cat\":\"%s\","
            "\"name\":\"%s\","
            "\"key\":\"%s\","
            "\"type\":\"%s\","
            "\"rv_1_name\":\"%s\","
            "\"rv_1_value\":\"%s\","
            "\"rv_2_name\":\"%s\","
            "\"rv_2_value\":\"%s\","
            "\"default_value\":\"%s\","
            "\"current_value\":\"%s\"},",
            text[0], text[1], text[2], text[3], text[4],
            text[5], text[6], text[7], text[8], text[9]
            );
    return 0;
}

int ajax_system_config_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char sql[256];
    char *errmsg = NULL;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"configs\":[");

    sprintf(sql, "SELECT * FROM settings WHERE disable='false'");
    ret = sqlite3_exec(task->database, sql, sql_system_settings_result, thiz, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
        ret = ERR_ERR;
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    return ret;
}

int sql_system_settings_options_result(void *param, int nr, char **text, char **name)
{
    struct ajax_xml_struct *thiz = (struct ajax_xml_struct *)param;

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"face\":\"%s\",\"val\":\"%s\"},", text[1], text[2]);
    return 0;
}

int ajax_system_config_options_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char name[32], sql[256];
    char *errmsg = NULL;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"options\":[");
    if ( ! mg_get_var(thiz->xml_conn, "name", name, 32) ) {
        ret = ERR_ERR;
        goto die;
    }

    sprintf(sql, "SELECT * FROM settings_options where key='%s'", name);
    ret = sqlite3_exec(task->database, sql, sql_system_settings_options_result, thiz, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
        ret = ERR_ERR;
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
die:
    return ret;
}

int ajax_system_config_save_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char s[512]={0}, sql[256];
    char *p = NULL;
    char *errmsg = NULL;
    char key[33], value[33];
    int keyok = 0, valok = 0;
    int keylen = 0, vallen = 0;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"save\":[");
    if ( ! mg_get_var(thiz->xml_conn, "s", s, 512) ) {
        ret = ERR_ERR;
        goto die;
    }
    p = & s[0];
    memset(key, 0, sizeof(key));
    memset(value, 0, sizeof(value));
    log_printf(INF, "[%s]", s);
    while ( *p ) {

        if ( *p == '=' ) {
            keyok = 1;
        } else if ( *p == ';' ) {
            valok = 1;
        } else {
            if ( ! keyok ) {
                if ( keylen < 32 )
                    key[ keylen ++ ] = *p;
            } else if ( !valok ) {
                if ( vallen < 32 )
                    value[ vallen ++ ] = *p;
            } else {};
        }

        if ( keyok && valok ) {
            sprintf(sql, "UPDATE settings SET current_value='%s' WHERE key='%s'",
                    value, key);
            ret = sqlite3_exec(task->database, sql, sql_system_settings_options_result, thiz, &errmsg);
            if ( ret ) {
                log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
                ret = ERR_ERR;
            } else {
                if ( 0 == strcmp("module_count", key) ) {
                    task->modules_nr = atoi(value);
                } else if ( 0 == strcmp("system_type", key) ) {
                    if ( atoi(value) == 0 ) {
                        task->sys_type = SYSTEM_FENTISHI;
                    } else {
                        task->sys_type = SYSTEM_YITISHI;
                    }
                }
            }
            log_printf(INF, "%s = %s", key, value);
            memset(key, 0, sizeof(key));
            memset(value, 0, sizeof(value));
            keylen = 0;
            vallen = 0;
            keyok = 0;
            valok = 0;
        } else if ( valok && ! keyok ) {
            memset(key, 0, sizeof(key));
            memset(value, 0, sizeof(value));
            keylen = 0;
            vallen = 0;
            keyok = 0;
            valok = 0;
        }
        p++;
    }
#if 0

#endif
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
    exit(0);
die:

    return ret;
}

int ajax_system_detail_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"VA\":\"%.1f V\",", task->meter[0].Va);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"VB\":\"%.1f V\",", task->meter[0].Vb);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"VC\":\"%.1f V\",", task->meter[0].Vc);

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

    return ret;
}

int ajax_system_do_active(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    struct ifaddrs * ifa=NULL, *ifaddr = NULL;
    char host[128];

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"active\":{");

    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)  continue;
        if (ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4 is a valid IP4 Address
            getnameinfo(ifa->ifa_addr,
                        sizeof(struct sockaddr_in),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"%s\":\"%s\",",
                    ifa->ifa_name, host);
        }
    }
    freeifaddrs(ifaddr);

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");
    return ret;
}


int ajax_card_init_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char money[16], passwd[16], op[16];

    mg_get_var(thiz->xml_conn, "op", op, 16);
    mg_get_var(thiz->xml_conn, "money", money, 16);
    mg_get_var(thiz->xml_conn, "passwd", passwd, 16);

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    if ( op[0] == '\0' ) {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"id\":\"%s\",", "N/A");
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"status\":\"%s\",", task->op_card.card.sector_4.data.magic == 0x4F4E5057 ? "正常":"未格式化");
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"money\":\"%.2f\",", task->op_card.card.sector_4.data.remain_money[0] / 100.0f);
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"passwd\":\"%d%d%d%d%d%d\",",
                (task->op_card.card.sector_4.data.passwd_code[0] & 0x0F) + '0',
                (task->op_card.card.sector_4.data.passwd_code[0] >> 4) + '0',
                (task->op_card.card.sector_4.data.passwd_code[1] & 0x0F) + '0',
                (task->op_card.card.sector_4.data.passwd_code[1] >> 4) + '0',
                (task->op_card.card.sector_4.data.passwd_code[2] & 0x0F) + '0',
                (task->op_card.card.sector_4.data.passwd_code[2] >> 4) + '0');
    } else {
        if ( 0 == strcmp(op, "format") ) {
            task->op_card.card.sector_4.data.magic = 0x4F4E5057;
            task->op_card.card.sector_4.data.remain_money[0] = 0;
            task->op_card.card.sector_4.data.remain_money[1] = 0;
            task->op_card.card.sector_4.data.remain_money[2] = 0;
            task->op_card.card.sector_4.data.remain_sum =
                    check_sum(task->op_card.card.sector_4.data.remain_money, 3);
            task->op_card.card.sector_4.data.passwd_code[0] = 0;
            task->op_card.card.sector_4.data.passwd_code[1] = 0;
            task->op_card.card.sector_4.data.passwd_code[2] = 0;
            task->op_card.card.sector_4.data.passwd_sum =
                    check_sum(task->op_card.card.sector_4.data.passwd_code, 3);
            task->op_card.card.sector_4.buff[15] =
                    check_sum(task->op_card.card.sector_4.buff, 15);

            bit_set(task, CMD_CARD_SET);
        } else if ( 0 == strcmp(op, "set") ) {
            unsigned int i_money = atof(money) * 100, i;

            task->op_card.card.sector_4.data.magic = 0x4F4E5057;
            task->op_card.card.sector_4.data.remain_money[0] = i_money&0xFF;
            task->op_card.card.sector_4.data.remain_money[1] = (i_money>>8)&0xFF;
            task->op_card.card.sector_4.data.remain_money[2] = (i_money>>16)&0xFF;
            task->op_card.card.sector_4.data.remain_sum =
                    check_sum(task->op_card.card.sector_4.data.remain_money, 3);
            for ( i = 0; i < 6; i ++ ) {
                if ( passwd[ i ] > '9' || passwd[i] < '0') {
                    passwd[ i ] = 0;
                }
                passwd[ i ] -= '0';
            }
            task->op_card.card.sector_4.data.passwd_code[0] = (passwd[1] << 4) | passwd[0];
            task->op_card.card.sector_4.data.passwd_code[1] = (passwd[3] << 4) | passwd[2];
            task->op_card.card.sector_4.data.passwd_code[2] = (passwd[5] << 4) | passwd[4];
            task->op_card.card.sector_4.data.passwd_sum =
                    check_sum(task->op_card.card.sector_4.data.passwd_code, 3);
            task->op_card.card.sector_4.buff[15] =
                    check_sum(task->op_card.card.sector_4.buff, 15);

            bit_set(task, CMD_CARD_SET);
        } else {

        }
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"result\":\"%s\",", "N/A");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");
    return ret;
}

int ajax_jiaozhun_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    char p[16], op[16];

    mg_get_var(thiz->xml_conn, "op", op, 16);
    mg_get_var(thiz->xml_conn, "p", p, 16);
    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    if ( task->bus2_correct_V >= 99999.9 &&
         task->bus1_correct_V >= 99999.9 &&
         task->bus_correct_I  >= 99999.9 ) {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"saved\":\"yes\",");
    }

    if ( 0 == strcmp(op, "V1") ) {
        bit_clr(task, CMD_JIAOZHUN_BAT_I);
        bit_clr(task, CMD_JIAOZHUN_BUS2_V);
        task->bus1_correct_V = atof(p);
        task->bus2_correct_V = 0;
        task->bus_correct_I = 0;
        bit_set(task, CMD_JIAOZHUN_BUS1_V);
    } else if ( 0 == strcmp(op, "V2") ) {
        bit_clr(task, CMD_JIAOZHUN_BAT_I);
        bit_clr(task, CMD_JIAOZHUN_BUS1_V);
        task->bus2_correct_V = atof(p);
        task->bus1_correct_V = 0;
        task->bus_correct_I = 0;
        bit_set(task, CMD_JIAOZHUN_BUS2_V);
    } else if ( 0 == strcmp(op, "I") ) {
        bit_clr(task, CMD_JIAOZHUN_BUS1_V);
        bit_clr(task, CMD_JIAOZHUN_BUS2_V);
        task->bus_correct_I = atof(p);
        task->bus2_correct_V = 0;
        task->bus1_correct_V = 0;
        bit_set(task, CMD_JIAOZHUN_BAT_I);
    } else if ( 0 == strcmp(op, "done") ) {
        bit_set(task, CMD_JIAOZHUN_BUS1_V);
        bit_set(task, CMD_JIAOZHUN_BUS2_V);
        bit_set(task, CMD_JIAOZHUN_BAT_I);
        task->bus2_correct_V = 0;
        task->bus1_correct_V = 0;
        task->bus_correct_I = 0;
    } else if ( 0 == strcmp(op, "CV1") ) {
        bit_clr(task, CMD_JIAOZHUN_BAT_I);
        bit_clr(task, CMD_JIAOZHUN_BUS1_V);
        bit_clr(task, CMD_JIAOZHUN_BUS2_V);
        task->bus2_correct_V = atof(p);
        task->bus1_correct_V = 0;
        task->bus_correct_I = 0;
    } else if ( 0 == strcmp(op, "CV2") ) {
        bit_clr(task, CMD_JIAOZHUN_BAT_I);
        bit_clr(task, CMD_JIAOZHUN_BUS1_V);
        bit_clr(task, CMD_JIAOZHUN_BUS2_V);
        task->bus2_correct_V = atof(p);
        task->bus1_correct_V = 0;
        task->bus_correct_I = 0;
    } else {
        task->bus2_correct_V = 0;
        task->bus1_correct_V = 0;
        task->bus_correct_I = 0;
    }


    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"V1\":\"%.1f\",",
            task->bus1_read_V);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"V2\":\"%.1f\",",
            task->bus2_read_V);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"I\":\"%.1f\",",
            task->bus_read_I);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"SCS\":\"%s\",",
            bit_read(task, S_MEASURE_1_COMM_DOWN)?"ERR":"OK");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"CCS\":\"%s\",",
            bit_read(task, S_CONVERT_BOX_COMM_DOWN)?"ERR":"OK");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

    return ret;
}

int ajax_auth_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;

    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"status\":\"CONFIRMED\",");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

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
        "恒压",
        "恒流"
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
        sprintf(buff, "正常");
    } else if ( g == GUN_SN1 ) {
        sprintf(buff, "正常");
    } else {
        sprintf(buff, "断开");
    }
    double ycdl = 0.0;
    if ( job->job_status == JOB_WORKING ) {
        switch (job->charge_billing.mode) {
        case BILLING_MODE_AS_CAP:
            ycdl = 100 * (task->meter[0].kwh_zong - job->charge_begin_kwh_data) /
                    job->charge_billing.option.set_kwh;
            break;
        case BILLING_MODE_AS_MONEY:
            break;
        case BILLING_MODE_AS_TIME:
            ycdl = 100.0 * ((time(NULL) - job->charge_begin_timestamp)*1.0f) /
                    job->charge_billing.option.set_time;
            break;
        default:
            break;
        }
    } else if ( job->job_status == JOB_DONE ) {
        ycdl = 100.0;
    } else {}

    xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
            "{\"status\":\"%s\","    // 状态
            "\"id\":\"%08X\","       // 作业ID，序号
            "\"port\":\"%d#\","     // 充电端口
            "\"cmode\":\"%s\","      // 充电模式
            "\"LV\":\"%.1f V\","     // 充电限压
            "\"LI\":\"%.1f A\","     // 充电限流
            "\"bmode\":\"%s\","      // 计费方式
            "\"hwk\":\"%.1f KW.H\"," // 电量
            "\"time\":\"%.d 分钟\","  // 时间
            "\"money\":\"%.1f 元\","  // 金额
            "\"cid\":\"%s\","          // 卡号ID
            "\"cremain\":\"%.2f 元\"," // 余额
            "\"CV\":\"%.1f V\","       // 充电电压
            "\"CI\":\"%.1f A\","       // 充电电流
            "\"ycdl\":\"%.2f %%\","    // 充电进度
            "\"kwh_price\":\"%.2f\","   // 充电单价
            "\"used_kwh\":\"%.2f\","    // 已充电量
            "\"used_time\":\"%.2f\","    // 已充时长
            "\"used_money\":\"%.3f\","    // 已充电费
            "\"gun_stat\":\"%s\""       // 充电枪连接状态
            "},",
            status_string[job->job_status],
            (unsigned int)job->job_url_commit_timestamp,
            job->job_gun_sn + 1,
            cmode_string[job->charge_mode],
            job->need_V,
            job->need_I,
            bmode_string[job->charge_billing.mode],
            job->charge_billing.option.set_kwh,
            job->charge_billing.option.set_time / 60,
            job->charge_billing.option.set_money,
            job->card.triger_card_sn,
            __card_read_remain(&job->card),
            __bytes2double(b2l(task->measure[0]->measure.VinKM0 )),
            __bytes2double(b2l(task->measure[0]->measure.IoutBAT0)),
            ycdl,
            task->kwh_price,
            job->charged_kwh + job->section_kwh,
            (job->charged_seconds + job->section_seconds)/60.0,
            job->charged_money,
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
#if 1
    mg_get_var(thiz->xml_conn, "id", id, 16);
    j = job_search(__atoh(id));

    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"id\":\"%s\"", id);

    if ( j == NULL ) {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"REJECTED\",");
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"reason\":\"没有该作业\"");
    } else {
        if ( j->charge_job_create_timestamp + 10 > time(NULL) ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"REJECTED\"");
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"reason\":\"作业保护\"");
        } else {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"OK\"");
            bit_set(j, CMD_JOB_ABORT);
            log_printf(INF, "UART: 作业运行时间: %ld - %ld = %ld",
                       time(NULL), j->charge_job_create_timestamp,
                       time(NULL) - j->charge_job_create_timestamp);
        }
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");
#endif
    return ret;
}

int ajax_job_manpause_json_proc(struct ajax_xml_struct *thiz)
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
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"REJECTED\",");
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"reason\":\"没有该作业\"");
    } else {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"OK\"");
        if ( j->job_status == JOB_WORKING ) {
                bit_set(j, CMD_JOB_MAN_PAUSE);
        }
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

    return ret;
}

int ajax_job_resume_json_proc(struct ajax_xml_struct *thiz)
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
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"REJECTED\",");
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"reason\":\"没有该作业\"");
    } else {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "\"status\":\"OK\"");
        bit_set(j, CMD_JOB_RESUME);
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

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

// 生成动态设置页面
int ajax_debug_list(struct ajax_xml_struct *thiz)
{
    struct config_struct *head = configs;
    int nr = 0, output_len = 0;
    char tag[128] = {0}, val[128] ={0}, de_val[128]={0};

    mg_get_var(thiz->xml_conn, "t", tag, 128);
    mg_get_var(thiz->xml_conn, "v", val, 128);

    if ( tag[0] && val[0] ) {
        mg_url_decode(val, 128, de_val, 128, 0);
        log_printf(DBG_LV1, "WEB SET %s: %s", tag, de_val);
        if ( de_val[0] ) {
            config_write(tag, val);
        }
    }

    thiz->ct = "text/html";
    output_len = sprintf(&thiz->iobuff[output_len],
                            "<html><title>直流充电桩内部调试接口</title>"
                            "<script>function c(e,t){"
                            "window.location.href=\"list.html?t=\""
                            "+document.getElementById(t).innerHTML+\"&v=\""
                            "+document.getElementById(e).value;"
                            "}</script>"
                            "<body><table border=\"1px\" align=\"center\">");
    for ( ; head && head->config_name != NULL && head->config_name[0] && nr < 37; head ++, nr ++ ) {
        log_printf(DBG_LV1, "get configure list from WEB. %d:%p:%d", nr, head, output_len);
        output_len += sprintf(&thiz->iobuff[output_len], "<tr><td id=\"t%d\">%s</td>",
                              nr,
                              head->config_name);
        output_len += sprintf(&thiz->iobuff[output_len],
                              "<td>%s</td>",
                              value_type[(unsigned int)(head->config_type)]);
        output_len += sprintf(&thiz->iobuff[output_len],
                              "<td><input id=\"e%d\" type=\"text\" value=\"%s\""
                              "/><a href=\"javascript:c(\'e%d\',\'t%d\')\">SET</a></td></tr>",
                              nr,
                              head->config_value, nr, nr);
        continue;
        output_len += sprintf(&thiz->iobuff[output_len],
        "<td><a href=\"/debug/list.html?mode=set&tag=%s&seed=%s\"></td>",
                                 head->config_name, head->config_value);
    }
    log_printf(DBG_LV0, "general done..");

    output_len += sprintf(&thiz->iobuff[output_len], "</table></body></html>");
    thiz->xml_len = output_len;

    return ERR_OK;
}

// 生成动态设置页面
int ajax_debug_json_list(struct ajax_xml_struct *thiz)
{
    struct config_struct *head = configs;
    int nr = 0, output_len = 0;
    char tag[128] = {0}, val[128] ={0}, de_val[128]={0};

    mg_get_var(thiz->xml_conn, "t", tag, 128);
    mg_get_var(thiz->xml_conn, "v", val, 128);

    if ( tag[0] && val[0] ) {
        mg_url_decode(val, 128, de_val, 128, 0);
        log_printf(DBG, "WEB SET %s: %s", tag, de_val);
        if ( de_val[0] ) {
            log_printf(DBG, config_write(tag, val));
        }
    }

    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "{\"configs\":[");
    for ( ; head && head->config_name != NULL && head->config_name[0]; head ++, nr ++ ) {
        if ( C_STRING == head->config_type ) {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"t\":\"%s\",\"v\":\"%s\"},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                                  head->config_value);
        } else if ( C_BOOL == head->config_type ) {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"t\":\"%s\",\"v\":\"%s\"},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                    (head->config_value[0] == 't' || head->config_value[0] == 'T') ?"true":"false");
        } else {
            output_len += sprintf(&thiz->iobuff[output_len],
                                  "{\"n\":\"%s\",\"t\":\"%s\",\"v\":%s},",
                                  head->config_name,
                                  value_type[(unsigned int)(head->config_type)],
                    head->config_value[0]=='N'?"0":head->config_value);
        }
    }
    thiz->iobuff[output_len--] = '\0';
    output_len += sprintf(&thiz->iobuff[output_len], "]}");
    thiz->xml_len = output_len;

    return ERR_OK;
}

// 更新系统文件
int ajax_update_proc(struct ajax_xml_struct *thiz)
{
        int output_len = 0;

    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "\"update\":{");

    if ( 0 == fork() ) {
        // 子进程
        system("/usr/zeus/script/update.sh");
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
    return ERR_OK;
}

// 导出系统日志
int ajax_export_proc(struct ajax_xml_struct *thiz)
{
        int output_len = 0;
    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "\"export\":{");

    if ( 0 == fork() ) {
        // 子进程
        system("/usr/zeus/script/export.sh");
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
    return ERR_OK;
}

// 提交并跳转
int ajax_debug_commit(struct ajax_xml_struct *thiz)
{
    thiz->ct = "text/html";
    return ERR_ERR;
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
