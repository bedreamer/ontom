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
    {"/system/hotrestart.json", ajax_hot_restart_proc},

    // 充电作业调用接口
    {"/job/create.json",        ajax_job_create_json_proc},
    {"/job/delete.json",        ajax_job_delete_json_proc},
    {"/job/query.json",         ajax_job_query_json_proc},
    {"/job/detail.json",        ajax_job_detail_json_proc},
    {"/job/edit.json",          ajax_job_edit_json_proc},
    {"/job/abort.json",         ajax_job_abort_json_proc},
    {"/job/manpause.json",      ajax_job_manpause_json_proc},
    {"/job/resume.json",        ajax_job_resume_json_proc},
    {"/job/auth.json",          ajax_job_auth_json_proc},
    {"/job/request.json",       ajax_job_request_json_proc},

    // 调试接口
    {"/debug/list.html",        ajax_debug_list},
    {"/debug/list.json",        ajax_debug_json_list},
    {"/debug/commit.html",      ajax_debug_commit},
    {"/debug/singlebit/read.json",    ajax_debug_bit_read},
    {"/debug/singlebit/write.json",   ajax_debug_bit_write},
    {"", NULL}
};

int ajax_search_file(const char *fn)
{
    struct xml_generator *cursor = xmls;

    for ( ; cursor->xml_gen_proc ; cursor ++ ) {
        if ( 0 != strcmp(fn, cursor->xml_name) ) continue;
        return ERR_OK;
    }
    return ERR_ERR;
}

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

    if ( bit_read(task, S_BAT_0_INSTITUDE) ||
         bit_read(task, S_BAT_1_INSTITUDE) ) {
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
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线绝缘状态
            "\"bus1_institude\":\"%s\",", p);

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Va\":\"%.1f V\",", task->meter[0].Va);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Vb\":\"%.1f V\",", task->meter[0].Vb);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"Vc\":\"%.1f V\",", task->meter[0].Vc);

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电压
            "\"bus0_V\":\"%.1f V\",", __bytes2double(b2l(task->measure[0]->measure.VinKM0)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电压
            "\"bus1_V\":\"%.1f V\",", __bytes2double(b2l(task->measure[0]->measure.VinKM1)));
    do {
        double current = task->meter[0].current_ok[0] ?
                    task->meter[0].current[0] : __bytes2double(b2l(task->measure[0]->measure.IoutBAT0));
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 一段母线电流
            "\"bus0_I\":\"%.1f A\",", current);
        current = task->meter[0].current_ok[1] ?
                            task->meter[0].current[1] : __bytes2double(b2l(task->measure[0]->measure.IoutBAT1));
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 二段母线电流
            "\"bus1_I\":\"%.1f A\",", current);
    } while (0);
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

    // {{
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 后台通讯状态
            "\"pcomm_status\":\"%s\",", bit_read(task, S_BACKCOMM_DOWN)?"离线":"在线");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"charger_name\":\"ON-DC-%05d\",", task->charger_sn);
    // }}

    // {{ 母线对地电阻
    do {
        char unit_str[32] = {0};
        if ( b2l(task->measure[0]->measure.VinBAT0RESP) >= 10000 ) {
            strncpy(unit_str, "> 1MΩ", 16);
        } else {
            sprintf(unit_str, "%.1f KΩ",
                    __bytes2double(b2l(task->measure[0]->measure.VinBAT0RESP)));
        }
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 电池1正对地电阻
                "\"bat1_R_P\":\"%s\",", unit_str);

        if ( b2l(task->measure[0]->measure.VinBAT0RESN) >= 10000 ) {
            strncpy(unit_str, "> 1MΩ", 16);
        } else {
            sprintf(unit_str, "%.1f KΩ",
                    __bytes2double(b2l(task->measure[0]->measure.VinBAT0RESN)));
        }
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 电池1负对地电阻
                "\"bat1_R_N\":\"%s\",", unit_str);

        if ( b2l(task->measure[0]->measure.VinBAT1RESP) >= 10000 ) {
            strncpy(unit_str, "> 1MΩ", 16);
        } else {
            sprintf(unit_str, "%.1f KΩ",
                    __bytes2double(b2l(task->measure[0]->measure.VinBAT1RESP)));
        }
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 电池2正对地电阻
                "\"bat2_R_P\":\"%s\",", unit_str);

        if ( b2l(task->measure[0]->measure.VinBAT1RESN) >= 10000 ) {
            strncpy(unit_str, "> 1MΩ", 16);
        } else {
            sprintf(unit_str, "%.1f KΩ",
                    __bytes2double(b2l(task->measure[0]->measure.VinBAT1RESN)));
        }
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 电池2负对地电阻
                "\"bat2_R_N\":\"%s\",", unit_str);
    } while (0);
    // }}

    //{{ 干湿度
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"temprature\":\"%.1f C°\",",
            __bytes2double(b2l(task->measure[0]->measure.Temp/10.0f)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], // 4# 充电枪连接状态
            "\"wet_rate\":\"%.1f %%\",",
            __bytes2double(b2l(task->measure[0]->measure.Humi/10.0f)));
    //}}

    thiz->iobuff[--thiz->xml_len] = '\0';
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "}");
    return ret;
}

// 充电任务操作接口
int ajax_job_create_json_proc(struct ajax_xml_struct *thiz)
{
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
    char c_assit[16] = {0};
    int code = 0;

    mg_get_var(thiz->xml_conn, "t", timestamp, 32);
    mg_get_var(thiz->xml_conn, "cid", cid, 32);
    mg_get_var(thiz->xml_conn, "gun", gun, 8);
    mg_get_var(thiz->xml_conn, "remain", c_remain, 32);
    mg_get_var(thiz->xml_conn, "passwd", c_passwd, 8);
    mg_get_var(thiz->xml_conn, "assit", c_assit, 8);

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
        code = 1;
        goto reject;
    }

    if ( strlen(cid) <= 0 ) {
        log_printf(DBG_LV3, "卡编号错误");
        code = 2;
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
    default:
        log_printf(DBG_LV3, "无效的充电枪");
        code = 3;
        goto reject;
    }
    log_printf(INF, "atoi(gun) jc.charge_gun %x", jc.charge_gun);

    if ( strlen(c_assit) > 0 ) {
        if ( 0 == strcmp("auto", c_assit) ) {
            jc.assit_power_level = 0;
        } else if ( 0 == strcmp("bbus", c_assit) ) {
            if ( jc.charge_gun == GUN_SN0 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN1_ASSIT_24] ) {
                    log_printf(DBG_LV3, "系统中不存在24V辅助电源");
                    code = 4;
                    goto reject;
                }
            } else if ( jc.charge_gun == GUN_SN1 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN2_ASSIT_24] ) {
                    log_printf(DBG_LV3, "系统中不存在24V辅助电源");
                    code = 4;
                    goto reject;
                }
            } else {
                log_printf(DBG_LV3, "无效的充电枪");
            }
            jc.assit_power_level = 24;
        } else if ( 0 == strcmp("mbus", c_assit) ) {
            if ( jc.charge_gun == GUN_SN0 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN1_ASSIT_12] ) {
                    log_printf(DBG_LV3, "系统中不存在12V辅助电源");
                    code = 5;
                    goto reject;
                }
            } else if ( jc.charge_gun == GUN_SN1 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN2_ASSIT_12] ) {
                    log_printf(DBG_LV3, "系统中不存在12V辅助电源");
                    code = 5;
                    goto reject;
                }
            } else {
                log_printf(DBG_LV3, "无效的充电枪");
            }
            jc.assit_power_level = 12;
        } else if ( 0 == strcmp("car", c_assit) ) {
            if ( jc.charge_gun == GUN_SN0 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN1_ASSIT_12] ) {
                    log_printf(DBG_LV3, "系统中不存在12V辅助电源");
                    code = 5;
                    goto reject;
                }
            } else if ( jc.charge_gun == GUN_SN1 ) {
                if ( ! task->kaichu_mask[KAICHU_GUN2_ASSIT_12] ) {
                    log_printf(DBG_LV3, "系统中不存在12V辅助电源");
                    code = 5;
                    goto reject;
                }
            } else {
                log_printf(DBG_LV3, "无效的充电枪");
            }
            jc.assit_power_level = 12;
        } else {
            log_printf(DBG_LV3, "错误的充电参数.");
            code = 6;
            goto reject;
        }
    }

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
            code = 7;
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
            code = 8;
            goto reject;
        }
    }

    switch ( jc.biling_mode ) {
    case BILLING_MODE_AS_AUTO:
        break;
    case BILLING_MODE_AS_CAP:
        if ( 0 == strlen(b_kwh) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
            goto reject;
        }
        jc.as_kwh = atof(b_kwh);
        if ( jc.as_kwh < 0.09999999f ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
            goto reject;
        }
        break;
    case BILLING_MODE_AS_MONEY:
        if ( 0 == strlen(b_money) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
            goto reject;
        }
        jc.as_money = atof(b_money);
        if ( jc.as_money < 0.09999999f ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
            goto reject;
        }
        break;
    case BILLING_MODE_AS_TIME:
        if ( 0 == strlen(b_time) ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
            goto reject;
        }
        jc.as_time = atoi(b_time) * 60;
        if ( jc.as_time < 1 ) {
            log_printf(DBG_LV3, "错误的计费参数");
            code = 9;
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
            code = 10;
            goto reject;
        }
        if ( strlen(set_I) <= 0 ) {
            log_printf(DBG_LV3, "2错误的充电参数");
            code = 10;
            goto reject;
        }
        jc.manual_set_charge_volatage = atof(set_V);
        jc.manual_set_charge_current = atof(set_I);
        if ( jc.manual_set_charge_volatage < 50.0f ) {
            log_printf(DBG_LV3, "3错误的充电参数");
            code = 10;
            goto reject;
        }
        if ( jc.manual_set_charge_current < 1.0f ) {
            log_printf(DBG_LV3, "4错误的充电参数");
            code = 10;
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
        code = 11;
        goto reject;
    }
    return ERR_OK;
reject:
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"code\":\"%d\",", code);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"status\":\"REJECTED\"}");
    return ERR_OK;
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
    char tp[64];
    int i;

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

            sprintf(tp, "%s", te->error_begin);
            i = 0;
            while ( tp[i] != '.' && tp[i] && i < 64 ) i ++;
            tp[i] = '\0';
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "{\"eid\":\"%d\","
                    "\"ebt\":\"%s\",",
                    te->error_id,
                    tp);
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
    char tp[64] = {0};
    int i;
    struct ajax_xml_struct *thiz = (struct ajax_xml_struct *)param;
    if ( nr <=0 ) return 0;

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"hid\":\"%s\",", text[0]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"eid\":\"%s\",", text[1]);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"estr\":\"%s\",", text[5]);
    sprintf(tp, "%s", text[2]);
    i = 0;
    while ( tp[i] != '.' && tp[i] && i < 64 ) i ++;
    tp[i] = '\0';
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"ebt\":\"%s\",", tp);
    sprintf(tp, "%s", text[3]);
    i = 0;
    while ( tp[i] != '.' && tp[i] && i < 64 ) i ++;
    tp[i] = '\0';
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "\"rbt\":\"%s\"},", tp);

    return 0;
}

// 返回历史故障
int ajax_system_history_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK;
    int lf = 0, nr = 12, n;
    char sql[512] = {0}, *errmsg, buff[128];
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
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"about\":[");

    getifaddrs(&ifaddr);
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)  continue;
        if (ifa->ifa_addr->sa_family==AF_INET) { // check it is IP4 is a valid IP4 Address
            getnameinfo(ifa->ifa_addr,
                        sizeof(struct sockaddr_in),
                        host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"k\":\"网口%s\",\"v\":\"%s\"},",
                    ifa->ifa_name, host);
        }
    }
    freeifaddrs(ifaddr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"k\":\"版本\",\"v\":\"%d\"},", VERSION);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"系统类型\",\"v\":\"%s\"},",
            task->sys_type == SYSTEM_YITISHI ? "一体式":
            task->sys_type == SYSTEM_FENTISHI ? "分体式" : "未知");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"充电枪\",\"v\":\"%d 把\"},", task->sys_config_gun_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"整流模块\",\"v\":\"%d 个\"},", task->modules_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"母线\",\"v\":\"%d 段\"},", task->sys_charge_group_nr);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"授权\",\"v\":\"%s\"},", "已激活");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"授权码\",\"v\":\"%s\"},", task->bcd_auth_code);

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电能(总)\",\"v\":\"%.1f KW.H\"},",
            task->meter[0].kwh_zong);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电能(尖)\",\"v\":\"%.1f KW.H\"},",
            task->meter[0].kwh_jian);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电能(峰)\",\"v\":\"%.1f KW.H\"},",
            task->meter[0].kwh_feng);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电能(平)\",\"v\":\"%.1f KW.H\"},",
            task->meter[0].kwh_ping);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电能(谷)\",\"v\":\"%.1f KW.H\"},",
            task->meter[0].kwh_gu);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电池1电压\",\"v\":\"%.1f V\"},",
            __bytes2double(b2l(task->measure[0]->measure.VinBAT0)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"电池2电压\",\"v\":\"%.1f V\"},",
            __bytes2double(b2l(task->measure[0]->measure.VinBAT1)));
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"需求电压\",\"v\":\"%.1f V\"},",
            atof(config_read("需求电压"))/10.0f);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"需求电流\",\"v\":\"%.1f A\"},",
            atof(config_read("需求电流"))/10.0f);
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"1#枪开关\",\"v\":\"%s(%s)\"},",
            bit_read(task, CMD_GUN_1_OUTPUT_ON)?"合闸":"分闸",
            bit_read(task, F_VOL1_SET_OK)?"可信":"不可信");
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
            "{\"k\":\"2#枪开关\",\"v\":\"%s(%s)\"},",
            bit_read(task, CMD_GUN_2_OUTPUT_ON)?"合闸":"分闸",
            bit_read(task, F_VOL2_SET_OK)?"可信":"不可信");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "]}");
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

    if ( 0 == strcmp(text[3], "options") ||
         0 == strcmp(text[3], "advance") ) {
        char *p = text[4];
        for ( ; *p; p ++ ) {
            if ( *p == '\"' ) *p = '@';
            if ( *p == '}' ) *p = '%';
            if ( *p == '{' ) *p = '#';
        }
    }

    thiz->xml_len += fprintf(thiz->ofile,
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
    thiz->ofile = tmpfile();
    thiz->xml_len = fprintf(thiz->ofile, "{\"configs\":[");

    sprintf(sql, "SELECT * FROM settings WHERE disable='false'");
    ret = sqlite3_exec(task->database, sql, sql_system_settings_result, thiz, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
        ret = ERR_ERR;
    }

    if (thiz->xml_len > 15 ) {
        fseek(thiz->ofile, thiz->xml_len - 1, SEEK_SET);
        thiz->xml_len --;
    }
    thiz->xml_len += fprintf(thiz->ofile, "]}");

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

void job_query_json_format(struct ajax_xml_struct *xml, struct charge_job *job)
{
    char *status_string[] = {
        "空闲",
        "设置",
        "等待",
        "系统检测",
        "车型测试",
        "车型确认",
        "作业启动",
        "就绪",
        "在充",
        "因故暂停",
        "人工暂停",
        "正在恢复",
        "正在中止",
        "作业完成",
        "正在退出",
        "正在清除",
        "即将销毁",
        "undefined"
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
    char *bat_kind[] = {
        "未知", "铅酸", "镍氢", "磷酸铁锂", "锰酸锂",
        "钴酸锂", "三元材料", "锂离子", "钛酸锂"
    };
    char bat_max_v[8] = {0};
    char bat_min_v[8] = {0};
    char bat_temprature[8] = {0};

    strcpy(bat_max_v, "N/A");
    strcpy(bat_min_v, "N/A");
    strcpy(bat_temprature, "N/A");

    char buff[16] = {0};
    if ( g == GUN_SN0 ) {
        sprintf(buff, "正常");
    } else if ( g == GUN_SN1 ) {
        sprintf(buff, "正常");
    } else {
        sprintf(buff, "断开");
    }
    double ycdl = 0.0;
    if ( job->job_status < JOB_DONE ) {
        switch (job->charge_billing.mode) {
        case BILLING_MODE_AS_CAP:
            ycdl = 100 * (task->meter[0].kwh_zong - job->charge_begin_kwh_data) /
                    job->charge_billing.option.set_kwh;
            break;
        case BILLING_MODE_AS_MONEY:
            break;
        case BILLING_MODE_AS_TIME:
            ycdl = 100.0 * (job->charged_seconds + job->section_seconds) /
                    job->charge_billing.option.set_time;
            break;
        default:
            ycdl = job->bms.bcs.spn3078_soc;
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
            "\"time\":\"%d 分钟\","  // 时间
            "\"money\":\"%.1f 元\","  // 金额
            "\"cid\":\"%s\","         // 卡号ID
            "\"cremain\":\"%.2f 元\"," // 余额
            "\"CV\":\"%.1f V\","       // 充电电压
            "\"CI\":\"%.1f A\","       // 充电电流
            "\"ycdl\":\"%.2f %%\","    // 充电进度
            "\"kwh_price\":\"%.2f\","   // 充电单价
            "\"used_kwh\":\"%.2f\","    // 已充电量
            "\"used_time\":\"%.2f\","   // 已充时长
            "\"used_money\":\"%.3f\","  // 已充电费
            "\"gun_stat\":\"%s\","      // 充电枪连接状态
            "\"bms_status\":\"%s\","    // BMS 通信状态
            ,
            status_string[job->job_status],
            (unsigned int)job->jid,
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
            g == GUN_SN0 ?
            __bytes2double(b2l(task->measure[0]->measure.VinKM0 )):
            __bytes2double(b2l(task->measure[0]->measure.VinKM1 )),
            g == GUN_SN0 ?
            __bytes2double(b2l(task->measure[0]->measure.IoutBAT0)):
            __bytes2double(b2l(task->measure[0]->measure.IoutBAT1)),
            ycdl,
            task->kwh_price,
            job->charged_kwh + job->section_kwh,
            (job->charged_seconds + job->section_seconds)/60.0,
            job->charged_money,
            buff,
            bit_read(job, JS_BMS_COMM_ERR) ? "通讯异常":"通讯正常"
            );
    if ( job->charge_mode == CHARGE_AUTO ) {
        // BRM
        if ( job->bms.charge_stage >= CHARGE_STAGE_HANDSHACKING ) {
            // 动力蓄电池类型
            if ( job->bms.brm.spn2566_battery_type > 0x08 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "\"BRM\":[{\"k\":\"电池材质\",\"v\":\"其他电池\"},");
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "\"BRM\":[{\"k\":\"电池材质\",\"v\":\"%s\"},",
                        bat_kind[job->bms.brm.spn2566_battery_type]);
            }
            // 蓄电池额定容量
            if ( job->bms.brm.spn2567_capacity <= 10000 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"额定容量\",\"v\":\"%.1f A.H\"},",
                        job->bms.brm.spn2567_capacity/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"额定容量\",\"v\":\"N/A\"},");
            }
            // 蓄电池额定电压
            if ( job->bms.brm.spn2568_volatage <= 7500 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"额定电压\",\"v\":\"%.1f V\"}],",
                        job->bms.brm.spn2568_volatage/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"额定电压\",\"v\":\"N/A\"}],");
            }
        }

        // BCP
        if ( job->bms.charge_stage >= CHARGE_STAGE_CONFIGURE ) {
            // 单体最高允许电压
            if ( job->bms.bcp.spn2816_max_charge_volatage_single_battery < 2400 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "\"BCP\":[{\"k\":\"单体最高允许电压\",\"v\":\"%.2f V\"},",
                        job->bms.bcp.spn2816_max_charge_volatage_single_battery/100.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "\"BCP\":[{\"k\":\"单体最高允许电压\",\"v\":\"N/A\"},");
            }
            // 最高允许充电电流
            if ( job->bms.bcp.spn2817_max_charge_current > -4000 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"单体最高允许电流\",\"v\":\"%.1f A\"},",
                        (job->bms.bcp.spn2817_max_charge_current-4000)/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"单体最高允许电流\",\"v\":\"N/A\"},");
            }
            // 蓄电池标称总能量
            if ( job->bms.bcp.spn2818_total_energy < 10000 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"标称总能量\",\"v\":\"%.1f KW.H\"},",
                        job->bms.bcp.spn2818_total_energy/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"标称总能量\",\"v\":\"N/A\"},");
            }
            // 最高允许充电电压
            if ( job->bms.bcp.spn2819_max_charge_voltage < 7500 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"最高允许充电电压\",\"v\":\"%.1f V\"},",
                        job->bms.bcp.spn2819_max_charge_voltage/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"最高允许充电电压\",\"v\":\"N/A\"},");
            }
            // 最高允许温度
            if ( job->bms.bcp.spn2820_max_temprature <= 250 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"最高允许温度\",\"v\":\"%d 度\"},",
                        (job->bms.bcp.spn2820_max_temprature-50));
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"最高允许温度\",\"v\":\"N/A\"},");
            }
            // 初始SOC
            if ( job->bms.bcp.spn2821_soc <= 1000 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"初始SOC\",\"v\":\"%.1f %%\"},",
                        job->bms.bcp.spn2821_soc/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"初始SOC\",\"v\":\"N/A\"},");
            }
            // 初始总电压
            if ( job->bms.bcp.spn2822_total_voltage <= 7500 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"初始总电压\",\"v\":\"%.1f V\"}],",
                        job->bms.bcp.spn2822_total_voltage/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"初始总电压\",\"v\":\"N/A\"}],");
            }
        }

        // BRO
        if ( job->bms.charge_stage >= CHARGE_STAGE_CONFIGURE ) {
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "\"BRO\":{\"k\":\"BMS充电状态\",\"v\":\"%s\"},",
                    bit_read(job, JF_BMS_BRO_OK) ? "就绪":"等待");
        }

        // BCS
        if ( job->bms.charge_stage >= CHARGE_STAGE_CHARGING ) {
            // 充电电压
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "\"BCS\":[{\"k\":\"BMS充电电压\",\"v\":\"%.1f V\"},",
                    job->bms.bcs.spn3075_charge_voltage/10.0);
            // 充电电流
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "{\"k\":\"BMS充电电流\",\"v\":\"%.1f A\"},",
                    (job->bms.bcs.spn3076_charge_current-4000)/-10.0f);
            // 最高单体电压电池电压
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "{\"k\":\"最高单体电压电池电压\",\"v\":\"%d\"},",
                    (job->bms.bcs.spn3077_max_v_g_number&0xFFF)/100);
            // SOC
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "{\"k\":\"SOC\",\"v\":\"%d %%\"},",
                    job->bms.bcs.spn3078_soc);
            // 剩余充电时间
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "{\"k\":\"剩余充电时间\",\"v\":\"%d 分\"}],",
                    job->bms.bcs.spn3079_need_time);
        }

        // BCL
        if ( job->bms.charge_stage >= CHARGE_STAGE_CHARGING ) {
            // 电压需求
            if ( job->bms.bcl.spn3072_need_voltage <= 7500 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "\"BCL\":[{\"k\":\"BMS电压需求\",\"v\":\"%.1f V\"},",
                        job->bms.bcl.spn3072_need_voltage/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "\"BCL\":[{\"k\":\"BMS电压需求\",\"v\":\"N/A\"},");
            }
            // 电流需求
            if ( job->bms.bcl.spn3073_need_current >= -4000 ) {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"BMS电流需求\",\"v\":\"%.1f A\"},",
                        (job->bms.bcl.spn3073_need_current-4000)/10.0f);
            } else {
                xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                        "{\"k\":\"BMS电流需求\",\"v\":\"N/A\"},");
            }
            // 充电模式
            xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],
                    "{\"k\":\"BMS充电模式\",\"v\":\"%s\"}]",
                    job->bms.bcl.spn3074_charge_mode==0x01?"恒压":
                    job->bms.bcl.spn3074_charge_mode==0x02?"恒流":"N/A");
        }

    }
    if (xml->iobuff[xml->xml_len-1] == ',') {
        xml->iobuff[--xml->xml_len] = '\0';
    }
    xml->xml_len+=sprintf(&xml->iobuff[xml->xml_len],"},");
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
        if ( task->job[i]->job_status >= JOB_EXITTING ) continue;
        job_query_json_format(thiz, task->job[i]);
    }
    h = task->wait_head;
    if ( h ) {
        pthread_mutex_lock(&task->wait_lck);
        do {
            job = list_load(struct charge_job, job_node, h);
            job_query_json_format(thiz, job);
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

int ajax_job_detail_json_proc(struct ajax_xml_struct *thiz)
{
    int ret = ERR_OK, i;
    struct list_head *h;
    struct charge_job *job;
    thiz->ct = "application/json";
    thiz->xml_len = 0;
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{\"jobs\":[");
    for ( i = 0; i < CONFIG_SUPPORT_CHARGE_JOBS; i ++ ) {
        if ( task->job[i] == NULL ) continue;
        job_query_json_format(thiz, task->job[i]);
    }
    h = task->wait_head;
    if ( h ) {
        pthread_mutex_lock(&task->wait_lck);
        do {
            job = list_load(struct charge_job, job_node, h);
            job_query_json_format(thiz, job);
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
            bit_set(j, JF_CMD_ABORT);
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
                //bit_set(j, CMD_JOB_MAN_PAUSE);
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
        //bit_set(j, CMD_JOB_RESUME);
    }

    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");

    return ret;
}

// 刷卡验证
int ajax_job_auth_json_proc(struct ajax_xml_struct *thiz)
{
    int i;
    int ret = ERR_OK;
    int id = 8; // 0: 无效，128: 创建查询， 256: 查询状态
    char cid[64] = {0};
    char aid[32] = {0};
    char passwd[32] = {0};

    mg_get_var(thiz->xml_conn, "id", aid, 16);
    mg_get_var(thiz->xml_conn, "cid", cid, 16);

    id = atoi(aid);

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    if ( id == 128 ) {
        // 创建查询
        if ( task->auth.auth_request_tsp ) {
            // 认证请求已经在进行
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":1,"
                    "\"msg\":\"认证请求已经在进行\"");
        } else {
            mg_get_var(thiz->xml_conn, "passwd", passwd, 16);
            task->auth.auth_id ++;
            strncpy(task->auth.card_sn, cid, 32);
            strncpy(task->auth.user_input_passwd, passwd, 32);
            task->auth.card_type = 0;
            task->auth.auth_request_tsp = time(NULL);
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":0,"
                    "\"msg\":\"正在认证\"");
        }
    } else if ( id == 256 ) {
        // 查询状态
        if ( 0 != strcmp(cid, task->auth.card_sn) ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":1,"
                    "\"msg\":\"无效的认证请求, ID错误!\"");
        } else {
            if ( task->auth.auth_return_tsp ) {
                thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                        "\"status\":%d,"
                        "\"msg\":\"%s\","
                        "\"allow\":%s,"
                        "\"remain\":\"%.2f 元\","
                        "\"kwh\":\"%.1f KW.h\"",
                        task->auth.auth_return_value,
                        task->auth.auth_msg,
                        task->auth.auth_allow_charge ? "true":"false",
                        task->auth.auth_return_remain/100.0f,
                        task->auth.auth_return_kwh/10.0f);
                task->auth.auth_request_tsp = 0;
                task->auth.auth_tx_tsp = 0;
                task->auth.auth_return_tsp = 0;
            } else {
                if ( task->auth.auth_request_tsp ) {
                    char msg[] = {'-', '\\', '|', '/'};
                    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                            "\"status\":2"
                            "\"msg\":\"正在认证 %c \"", msg[ (task->auth.verbose ++) % 4 ]);
                } else {
                    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                            "\"status\":1"
                            "\"msg\":\"无效操作\"");
                }
            }
        }
    } else {
        // 无效
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"status\":1,"
                "\"msg\":\"无效的认证请求\"");
    }

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");
    return ret;
}

// 请求充电
int ajax_job_request_json_proc(struct ajax_xml_struct *thiz)
{
    int i;
    int ret = ERR_OK;
    char id[12] = {0};
    mg_get_var(thiz->xml_conn, "id", id, 11);

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");

    if ( id[0] == 'R' ) {
        if ( task->request.request_tsp ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":255,"
                    "\"msg\":\"已经发送了充电请求!\"}");
        } else {
            char v_gun[6]={0}, v_cid[33]={0}, v_passwd[32]={0}, v_mode[8]={0}, v_param[5]={0};
            int param_ok = 1;

            mg_get_var(thiz->xml_conn, "gun", v_gun, 6);
            mg_get_var(thiz->xml_conn, "cid", v_cid, 20);
            mg_get_var(thiz->xml_conn, "passwd", v_passwd, 32);
            mg_get_var(thiz->xml_conn, "mode", v_mode, 8);
            mg_get_var(thiz->xml_conn, "param", v_param, 5);

            task->request.gun = atoi(v_gun);
            task->request.card_type = 0x00;
            memcpy(task->request.account, v_cid, 32);
            memcpy(task->request.passwd, v_passwd, 16);
            if ( 0 == strcmp(v_mode, "auto") ) {
                task->request.service_mode = 0x10;
            } else if ( 0 == strcmp(v_mode, "kwh") ) {
                task->request.service_mode = 0x20;
                task->request.service_param = (unsigned short)atof(v_param);
            } else if ( 0 == strcmp(v_mode, "time") ) {
                task->request.service_mode = 0x30;
                task->request.service_param = (unsigned short)atof(v_param);
            } else if ( 0 == strcmp(v_mode, "money") ) {
                task->request.service_mode = 0x40;
                task->request.service_param = (unsigned short)atof(v_param);
            } else {
                param_ok = 0;
            }

            if ( param_ok ) {
                task->request.request_tsp = time(NULL);
                thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                        "\"status\":0,"
                        "\"msg\":\"正在请求\"}");
            } else {
                thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                        "\"status\":1,"
                        "\"msg\":\"参数错误!\"}");
            }
        }
    } else if ( id[0] =='Q' ) {
        if ( !task->request.request_tsp ) {
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":255,"
                    "\"msg\":\"没有发送充电请求!\"}");
        } else if ( !task->request.ack_tsp ){
            static int nr = 0;
            char msg[] = {'-', '\\', '|', '/'};
            thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                    "\"status\":0,"
                    "\"msg\":\"正在通信 %c\"}", msg[ (nr ++) % 4 ]);
        } else { // 结果
            if ( task->request.result == 0 ) {
                thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                        "\"status\":0,"
                        "\"msg\":\"开始充电!\","
                        "\"action\":\"%d\""
                        "\"pre_pay\":\"%.1f\""
                        "\"remain\":\"%.1f\""
                        "\"allow_time\":\"%d\","
                        "\"allow_kwh\":\"%d\","
                        "\"bs\":\"%s\"}",
                        task->request.action,
                        task->request.pre_pay,
                        task->request.remian,
                        task->request.allow_time,
                        task->request.allow_kwh,
                        task->request.billing_sn
                        );
            } else {
                thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                        "\"status\":1,"
                        "\"msg\":\"认证错误,无法充电!\"}");
            }
            task->request.ack_tsp = 0;
        }
    } else {
        thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
                "\"status\":255,"
                "\"msg\":\"无效参数\"}");
    }
    return ret;
}

int ajax_debug_bit_read(struct ajax_xml_struct *thiz)
{
    int i;
    int ret = ERR_OK;

    thiz->ct = "application/json";
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "{");
    for (i=S_ERROR;i<S_END;i++) {
     thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len],
        "$%d:%d,", i, bit_read(task, i));
    }
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    thiz->xml_len += sprintf(&thiz->iobuff[thiz->xml_len], "}");
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
            log_printf(DBG, "%s:%s", tag, config_write(tag, val));
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
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
    system("/bin/sh /usr/zeus/script/upgrade.sh &");
    return ERR_OK;
}

// 导出系统日志
int ajax_export_proc(struct ajax_xml_struct *thiz)
{
        int output_len = 0;
    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "\"export\":{");

    system("/bin/sh /usr/zeus/script/export.sh &");

    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
    return ERR_OK;
}

// 热重启系统
int ajax_hot_restart_proc(struct ajax_xml_struct *thiz)
{
    exit(0);
    return ERR_OK;
}


// 提交并跳转
int ajax_debug_commit(struct ajax_xml_struct *thiz)
{
    thiz->ct = "text/html";
    return ERR_ERR;
}

// 监控系统
int ajax_zeus_json_list(struct ajax_xml_struct *thiz)
{
    int output_len = 0;
    thiz->ct = "application/json";
    output_len += sprintf(&thiz->iobuff[output_len], "\"task\":{");
    if (thiz->iobuff[thiz->xml_len-1] == ',') {
        thiz->iobuff[--thiz->xml_len] = '\0';
    }
    output_len += sprintf(&thiz->iobuff[output_len], "}");
    thiz->xml_len = output_len;
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
            if ( ! thiz.ofile ) {
                log_printf(DBG_LV0, "prepare stage2 done...");
                mg_write(conn, thiz.iobuff, thiz.xml_len);
                log_printf(DBG_LV0, "send file done...");
                //log_printf(DBG, "done %d", thiz.xml_len);
            } else {
                unsigned int s = 0, w;

                fseek(thiz.ofile, 0, SEEK_SET);
                while ( s < thiz.xml_len ) {
                    if ( thiz.xml_len - 512 >= 0 ) {
                        w = 512;
                    } else w = thiz.xml_len - s;

                    s += w;
                    if ( ! w ) break;
                    fread(thiz.iobuff, w, 1, thiz.ofile);
                    mg_write(conn, thiz.iobuff, w);
                }
                fclose(thiz.ofile);
                thiz.ofile = NULL;
            }
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
        usleep(5000);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}
