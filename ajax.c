#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ajax.h"
#include "config.h"
#include "error.h"
#include "log.h"
#include "charge.h"
#include "mongoose.h"

// 生成xml文件，并返回文件大小，头部由调用者生成
int ajax_deal_xml_proc(struct ajax_xml_struct *);
int ajax_charge_status_xml_proc(struct ajax_xml_struct *);
int ajax_battery_status_xml_proc(struct ajax_xml_struct *);
int ajax_alarm_xml_proc(struct ajax_xml_struct *);
int ajax_version_xml_proc(struct ajax_xml_struct *);
int ajax_autheticate_xml_proc(struct ajax_xml_struct *);
int ajax_query_card_xml_proc(struct ajax_xml_struct *thiz);
int ajax_confirm_charge_xml_proc(struct ajax_xml_struct *thiz);

struct xml_generator {
	// xml 文件名
	char * xml_name;
	// xml 生成过程地址
    int (*xml_gen_proc)(struct ajax_xml_struct *);
}xmls[]={
    {"/deal.xml",               ajax_deal_xml_proc},
    {"/chargestatus.xml",       ajax_charge_status_xml_proc},
    {"/battrystatus.xml",       ajax_battery_status_xml_proc},
    {"/alarm.xml",              ajax_alarm_xml_proc},
    {"/version.xml",            ajax_version_xml_proc},
    {"/autheticate.xml",        ajax_autheticate_xml_proc},
    {"/querycard.xml",          ajax_query_card_xml_proc},
    {"/confirm.xml",            ajax_confirm_charge_xml_proc},
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
            log_printf(DBG, "%s:%s", thiz->xml_name, cursor->xml_name);
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
                   "  <remaind>%f</remaind>\r\n"
                   "</triger>\r\n",
                   "11111222",
                   "yes",
                   2453.87f
    );
}

// 生成触发充电任务的卡信息
static inline int xml_gen_confirm_card(char *buff)
{
    return sprintf(buff,
                   "<triger>\r\n"
                   "  <cardid>%s</cardid>\r\n"
                   "  <valid>%s</valid>\r\n"
                   "  <remaind>%f</remaind>\r\n"
                   "</triger>\r\n",
                   "11111222",
                   "yes",
                   2453.87f
    );
}

// 生成触发充电任务的卡信息
static inline int xml_gen_settle_card(char *buff)
{
    return sprintf(buff,
                   "<triger>\r\n"
                   "  <cardid>%s</cardid>\r\n"
                   "  <valid>%s</valid>\r\n"
                   "  <super>%s</super>\r\n"
                   "  <remaind>%f</remaind>\r\n"
                   "</triger>\r\n",
                   "11111222",
                   "yes",
                   "no",
                   2453.87f
    );
}

// 生成系统故障
static inline int xml_gen_system_error(char *buff)
{
    static int magic = 0;

    if ( magic++ % 3 == 0 ) {
        return sprintf(buff,
                       "<error>\r\n"
                       " <total>\r\n"
                       "   <fault>3</fault>\r\n"
                       "   <worning>0</worning>\r\n"
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
}

/* 充电刷卡事件查询
 * 不同的使用阶段需要检查不同的字段
 * <start>..</start>:
 *    刷卡触发充电服务响应。 可以是自动或是选择充电方式后获得
 * <confirm>..</confirm>:
 *    充电服务触发后确认开始充电服务。即将开始计费
 * <settle_accounts>..</settle_accounts>:
 *    结账刷卡。达到结束充电条件后触发。
 *               首页/按金钱/按时间/按容量充电页面
 *  UI ------- GET /querycard.xml?mode=auto ------->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=asmoney ---->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=astime ----->>> ontom   没有刷卡
 *  or UI ---- GET /querycard.xml?mode=ascap ------>>> ontom   没有刷卡
 *  UI <<<--------RETURN /querycard.xml -------------- ontom   没有刷卡
 *  UI ------- GET /querycard.xml?mode=auto ------->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=asmoney ---->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=astime ----->>> ontom   刷   卡
 *  or UI ---- GET /querycard.xml?mode=ascap ------>>> ontom   刷   卡
 *  UI <<<---------RETURN /querycard.xml ------------- ontom   有 刷卡
 *  UI ----GET /querycard.xml?start=yes&bm=auto --->>> ontom   有 刷卡
 *                    页面跳转至充电确认页面
 * 完整的定量充电刷卡流程如下：
 * UI定时向ontom发送 GET /qerycard.xml请求，并附带当前的请求模式（自动，按金额，时间，容量）
 * ontom返回当前的刷卡状态，如果有刷卡，则返回刷卡卡号，卡内余额等信息，没有则返回卡号为 N/A.
 *
 * UI 根据ontom返回的xml判定刷卡卡号是否有效，例如：
 *  自动模式下收到有效刷卡卡号后下一次请求URL为：
 *  GET /querycard.xml?mode=auto&triger=valid
 * 如果是按条件充电则现需要设定有效参数，并被ontom接受，因此，可以按如方式发送请求：
 *  GET /querycard.xml?mode=asmoney&money=100.0 或者
 *  GET /querycard.xml?mode=astime&time=503 或者
 *  GET /querycard.xml?mode=ascap&cap=89
 * 按上述方式发送请求收到querycard.xml后需要解析<param_accept>字段， 若该字段为yes,
 * 则表示ontom接受该设置请求，否则需要修改设置值，此时也可以刷卡，但不能触发triger字段设为valid
 * 当收到querycard.xml 解析的param_accept字段为yes后方可，根据刷卡的ID设定triger 是否为
 * valid, 当解析出param_accept字段为yes后，发送的请求可以如下：
 *  GET /querycard.xml?mode=asmoney&money=100.9&triger=valid 或者
 *  GET /querycard.xml?mode=astime&time=102&triger=valid 或者
 *  GET /querycard.xml?mode=ascap&cap=98&triger=valid
 */
int ajax_query_card_xml_proc(struct ajax_xml_struct *thiz)
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
#else
    // 充电模式声明
    char mode[ 8 + 1 ] = {0};
    // 刷卡是否有效
    char triger[ 8 + 1 ] = {0}, confirm[ 8 + 1 ] = {0}, endding[ 8 + 1 ] = {0};
    // 制定模式下的参数
    char themoney[ 8 + 1 ] = {0}, thetime[ 8 + 1 ] = {0}, thecap[ 8 + 1 ] ={0};
    // 刷卡有效
    char cardvalid = 0;
    // 输出缓冲指针
    char *output = thiz->iobuff;
    // 当前输出缓冲长度
    int output_len = 0;

    mg_get_var(thiz->xml_conn, "mode", mode, 8);
    mg_get_var(thiz->xml_conn, "triger", triger, 8);
    mg_get_var(thiz->xml_conn, "confirm", confirm, 8);
    mg_get_var(thiz->xml_conn, "end", endding, 8);

    if (  0 == strcmp("valid", triger) )
        cardvalid = 1;
    if ( 0 == strcmp("valid", confirm) )
        cardvalid = 1;
    if ( 0 == strcmp("valid", endding) )
        cardvalid = 1;

    output_len += sprintf(&output[output_len],
         "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<start>\r\n");
    output_len += xml_gen_system_error(&output[output_len]);

    if ( 0 == strcmp("auto", mode) ) {
        task->charge_billing.mode = BILLING_MODE_AS_AUTO;
    } else if ( 0 == strcmp("asmoney", mode) ) {
        task->charge_billing.mode = BILLING_MODE_AS_MONEY;
        if ( mg_get_var(thiz->xml_conn, "money", themoney, 8) ) {
            double cash = atof(themoney);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            output_len += sprintf(&output[output_len], "<asmoney>\r\n");

            output_len += xml_gen_triger_card(&output[output_len]);
            output_len += xml_gen_confirm_card(&output[output_len]);
            output_len += xml_gen_settle_card(&output[output_len]);

            if ( cash > 0.0f && cash < 999.99f ) {
                task->charge_billing.option.set_money = cash;
                output_len += sprintf(&output[output_len],
                                      "<param_accept>yes</param_accept>\r\n");
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
                output_len += sprintf(&output[output_len],
                                      "<param_accept>no</param_accept>\r\n");
            }
            output_len += sprintf(&output[output_len],
                                  "</asmoney>\r\n");
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
        }
    } else if ( 0 == strcmp("astime", mode) ) {
        task->charge_billing.mode = BILLING_MODE_AS_TIME;
        if ( mg_get_var(thiz->xml_conn, "time", thetime, 8) ) {
            unsigned int minits = atoi(thetime);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            if ( minits > 0 && minits <= 600 ) {
                task->charge_billing.option.set_time = minits;
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
            }
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
        }
    } else if ( 0 == strcmp("ascap", mode) ) {
        task->charge_billing.mode = BILLING_MODE_AS_CAP;
        if ( mg_get_var(thiz->xml_conn, "cap", thecap, 8) ) {
            unsigned int caps = atoi(thecap);
            // 已经刷过卡了，现在做参数检查，直到参数检查成功
            if ( caps > 0 && caps <= 100 ) {
                task->charge_billing.option.set_cap = caps;
            } else {
                // 给定了一个错误的参数，不能进入下一步操作, 返回提示信息
            }
        } else {
            // 只做了普通的查询，说明没有收到过刷卡事件
        }
    } else {
        // 如果出现不需要刷卡就能充电的需求，可以在这里作相应处理
    }
     output_len += sprintf(&output[output_len], "</start>\r\n");
     thiz->xml_len = output_len;
     return ERR_OK;
#endif
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
    return ERR_ERR;
}

// 返回当前交易信息 deal.xml
int ajax_deal_xml_proc(struct ajax_xml_struct *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回充电状态信息 chargestatus.xml
int ajax_charge_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回电池状态信息 batterystatus.xml
int ajax_battery_status_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回告警信息 alarm.xml
int ajax_alarm_xml_proc(struct ajax_xml_struct  *thiz)
{
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_ERR;
}

// 返回版本信息 version.xml
int ajax_version_xml_proc(struct ajax_xml_struct *thiz)
{
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
    log_printf(DBG, "get into %s", __FUNCTION__);
    return ERR_OK;
}

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
    int result = MG_FALSE, err;
    struct ajax_xml_struct thiz = {0};
    thiz.xml_conn = conn;
    if (ev == MG_REQUEST) {
        log_printf(DBG, "%s", conn->uri);
        strncpy(thiz.xml_name, conn->uri, 31);
        err = ajax_gen_xml( & thiz );
        if ( err == ERR_OK ) {
            log_printf(DBG, "prepare...");
            mg_printf(conn,
                      "HTTP/1.1 200 HTTP\r\n"
                      "Server: thttpd/2.21b PHP/20030920\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "Content-Type: text/xml\r\n"
                      "Date: Wed, 20 Aug 2014 03:29:12 GMT\r\n"
                      "Last-Modified: Tue, 19 Aug 2014 09:23:50 GMT\r\n"
                      "Accept-Ranges: bytes\r\n"
                      "Content-Length: %d\r\n"
                      "Connection: keep-alive\r\n"
                      "\r\n", thiz.xml_len);
            mg_write(conn, thiz.iobuff, thiz.xml_len);
            log_printf(DBG, "done");
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

    log_printf(INF, "%s running...", __FUNCTION__);
    server = mg_create_server(NULL, ev_handler);
    mg_set_option(server, "listening_port", "8081");

    //printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
    for (; ! *done; ) {
        mg_poll_server(server, 1000);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}
