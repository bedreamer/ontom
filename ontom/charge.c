#include "stdafx.h"
#include "charge.h"
#include "blueprint.h"

struct charge_task tom;

// 充电任务结构
struct charge_task *task = &tom;

int sql_db_config_result(void *param, int nr, char **text, char **name)
{
    int i = 0;
    if ( nr > 0 && text ) {
        log_printf(DBG_LV1, "ZEUS: DB READ [%s=%s]", text[1], text[3]);
        if ( 0 == strcmp(text[1], "sys_conflict_map") ) {
            int x = 0, y = 0;
            i = 0;
            while (text[3][i]) {
                if ( x > CONFIG_SUPPORT_BMS_NR ||
                     y > CONFIG_SUPPORT_BMS_NR ) {
                    task->sys_conflict_map[0][0] = true;
                    task->sys_conflict_map[0][1] = false;
                    task->sys_conflict_map[1][0] = false;
                    task->sys_conflict_map[1][1] = true;
                    log_printf(WRN, "ZEUS: 输出据初始化错误，充电枪冲突配置异常. 使用默认值");
                    break;
                }
                if ( text[3][i] == 'C' ) {
                    task->sys_conflict_map[y][x++] = false;
                }
                if ( text[3][i] == 'N' ) {
                    task->sys_conflict_map[y][x++] = true;
                }
                if ( text[3][i] == ';' ) {
                    y ++;
                    x = 0;
                }
                i ++;
            }
        }else if( 0 == strcmp(text[1], "sys_config_gun_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，充电枪个数异常，使用默认值");
                task->sys_config_gun_nr = 2;
                goto _done;
            }
            task->sys_config_gun_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_can_name") ){
            int x = 0, y = 0;
            i = 0;
            memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
            while ( text[3][i] && i < (int)sizeof(task->sys_can_name) ) {
                if ( text[3][i] == ';' ) {
                    x = 0;
                    y ++;
                } else {
                    task->sys_can_name[y][x++] = text[3][i];
                }
                i ++;
            }
            if ( ! task->sys_can_name[0][0] ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，CAN设备文件名异常，使用默认值");
                memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
                strcpy(task->sys_can_name[0], "can0");
            }
        }else if( 0 == strcmp(text[1], "sys_simple_box_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，采样盒个数异常，使用默认值");
                task->sys_simple_box_nr = 1;
                goto _done;
            }
            task->sys_simple_box_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_charge_group_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，充电机组数异常，使用默认值");
                task->sys_charge_group_nr = 1;
                goto _done;
            }
            task->sys_charge_group_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_rs485_dev_nr") ){
            int nr = atoi(text[3]);
            int ma = atoi(text[5]);
            int mi = atoi(text[6]);
            if ( ma < 0 || mi < 0 || ma < mi || nr > ma || nr < mi ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，RS485设备个数异常，使用默认值");
                task->sys_rs485_dev_nr = 2;
                goto _done;
            }
            task->sys_rs485_dev_nr = nr;
        }else if( 0 == strcmp(text[1], "sys_uart_name") ){
            int x = 0, y = 0;
            i = 0;
            memset(task->sys_can_name, 0, sizeof(task->sys_can_name));
            while ( text[3][i] && i < (int)sizeof(task->sys_can_name) ) {
                if ( text[3][i] == ';' ) {
                    x = 0;
                    y ++;
                } else {
                    task->sys_uart_name[y][x++] = text[3][i];
                }
                i ++;
            }
            if ( ! task->sys_uart_name[0][0] ) {
                log_printf(WRN, "ZEUS: 数据库初始化失败，RS485设备文件名异常，使用默认值");
                memset(task->sys_uart_name, 0, sizeof(task->sys_uart_name));
                strcpy(task->sys_uart_name[0], "/dev/ttyO4");
                strcpy(task->sys_uart_name[1], "/dev/ttyO5");
            }
        } else if ( 0 == strcmp(text[1], "system_type") ) {
            task->sys_type = SYSTEM_FENTISHI;
            if ( 0 == strcmp(text[3], "分体式") ) {
                task->sys_type = SYSTEM_FENTISHI;
            } else if ( 0 == strcmp(text[3], "一体式") ) {
                task->sys_type = SYSTEM_YITISHI;
            } else {}
        } else if( 0 == strcmp(text[1], "limit_output_I") ) {
            task->limit_output_I = atof(text[3]);
        } else if ( 0 == strcmp(text[1], "limit_max_V") ) {
            task->limit_max_V = atof(text[3]);
        } else if( 0 == strcmp(text[1], "limit_min_V") ) {
            task->limit_min_V = atof(text[3]);
        } else if ( 0 == strcmp(text[1], "running_V") ) {
            task->running_V = atof(text[3]);
        } else if( 0 == strcmp(text[1], "modules_nr") ) {
            task->modules_nr = (unsigned short)atoi(text[3]);
        }
    }
_done:
    (*(int *)param) ++;
    return 0;
}

void print_POST_configure()
{
    unsigned int x = 0, y;
    printf("读取数据库配置数据完成， 清单如下：\n");
    printf("-----------------------BEGIN--------------------------\n");
    printf("  * 充电枪个数:  %4d  把\n", task->sys_config_gun_nr);
    printf("  * 充电冲突对照表:\n");
    if ( task->sys_config_gun_nr ) {
        printf("                 ");
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            printf("  %02d#   ", x + 1);
        }
        printf("\n");
    }
    for ( y = 0; y < task->sys_config_gun_nr; y ++ ) {
        printf("            %02d#  ", y + 1);
        for ( x = 0; x < task->sys_config_gun_nr; x ++ ) {
            if ( task->sys_conflict_map[y][x] ) {
                printf("  "GRN("兼容")"  ");
            } else {
                printf("  "RED("冲突")"  ");
            }
        }
        printf("\n");
    }
    printf("  * %-16s  %4d  个\n", "采样盒个数:", task->sys_simple_box_nr);
    printf("  * %-16s  %4d  组\n", "充电机组数:", task->sys_charge_group_nr);
    printf("  * %-16s  %4d  个\n", "RS485个数:", task->sys_rs485_dev_nr);
    printf("  * %-16s  %s\n"     , "充电方式:",
           task->sys_charge_group_nr==1?
               task->sys_config_gun_nr>1?"分时充电":"独立充电":
                "独立充电");
    printf("-----------------------END---------------------------\n");
}

// 重新获得设置值
int sql_db_settings_result(void *param, int nr, char **text, char **name)
{
    if ( nr <= 0 ) return 0;

    if ( 0 == strcmp(text[0], "system_type") ) {
        task->sys_type = atoi(text[1]);
    } else if ( 0 == strcmp(text[0], "bcd_auth_code") ) {
        strncpy((char *)task->bcd_auth_code, text[1], 16);
        printf("产品激活码: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "mac_addr") ) {
        strncpy((char *)task->mac_addr, text[1], 32);
        printf("产品MAC地址: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "sys_uart_name") ) {
        printf("通讯用串口: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "sys_can_name") ) {
        printf("通讯用CAN接口: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "module_kind") ) {
        task->module_model = atoi(text[1]);
        printf("模块型号: %d\n", task->module_model);
    } else if ( 0 == strcmp(text[0], "module_count") ) {
        task->modules_nr = atoi(text[1]);
        printf("模块个数: %d 个\n", task->modules_nr);
    } else if ( 0 == strcmp(text[0], "kwh_price") ) {
        task->kwh_price = atof(text[1]);
        printf("单位电价: %.1f 元/度\n", task->kwh_price);
    }  else if ( 0 == strcmp(text[0], "sys_config_gun_nr") ) {
        task->sys_config_gun_nr = atoi(text[1]);
        printf("充电枪: %s 把\n", text[1]);
    } else if ( 0 == strcmp(text[0], "sys_simple_box_nr") ) {
        task->sys_simple_box_nr = atoi(text[1]);
        printf("采样盒个数: %s 个\n", text[1]);
    } else if ( 0 == strcmp(text[0], "sys_charge_group_nr") ) {
        task->sys_charge_group_nr = atoi(text[1]);
        printf("充电机组数: %s 组\n", text[1]);
    } else if ( 0 == strcmp(text[0], "kwh_meter_addr") ) {
        memcpy(task->meter[0].addr, text[1], 12);
        task->meter[0].addr[12] = '\0';
        printf("电表地址: %s\n", task->meter[0].addr);
    } else if ( 0 == strcmp(text[0], "work_mode") ) {
        strncpy(task->sys_work_mode, text[1], 32);
        printf("工作模式: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "fenliuqi_xishu") ) {
        task->flq_xishu = atoi(text[1]);
        printf("分流器系数: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bus_1_v_hi") ) {
        task->bus_1_v_hi = atof(text[1]);
        printf("一段母线过压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bus_1_v_lo") ) {
        task->bus_1_v_lo = atof(text[1]);
        printf("一段母线欠压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_1_v_hi") ) {
        task->bat_1_v_hi = atof(text[1]);
        printf("一组电池过压值: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_1_v_lo") ) {
        task->bat_1_v_lo = atof(text[1]);
        printf("一组电池欠压值: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_1_I_hi") ) {
        task->bat_1_I_hi = atof(text[1]);
        printf("一组电池过流值: %s A\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_1_insti_ohm_v") ) {
        task->bat_1_insti_ohm_v = atof(text[1]);
        printf("一组电池绝缘告警值: %s ohm/V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bus_2_v_hi") ) {
        task->bus_2_v_hi = atof(text[1]);
        printf("二段母线过压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bus_2_v_lo") ) {
        task->bus_2_v_lo = atof(text[1]);
        printf("二段母线欠压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_2_v_hi") ) {
        task->bat_2_v_hi = atoi(text[1]);
        printf("二组电池过压值: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_2_v_lo") ) {
        task->bat_2_v_lo = atof(text[1]);
        printf("二组电池欠压值: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_2_I_hi") ) {
        task->bat_2_I_hi = atof(text[1]);
        printf("二组电池过流值: %s A\n", text[1]);
    } else if ( 0 == strcmp(text[0], "bat_2_insti_ohm_v") ) {
        task->bat_2_insti_ohm_v = atof(text[1]);
        printf("二组电池绝缘告警值: %s ohm/V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "meter_V_xishu") ) {
        task->meter_V_xishu = atof(text[1]);
        printf("电能表电压系数比: %s \n", text[1]);
    } else if ( 0 == strcmp(text[0], "meter_I_xishu") ) {
        task->meter_I_xishu = atof(text[1]);
        printf("电能表电流系数比: %s \n", text[1]);
    } else if ( 0 == strcmp(text[0], "module_power_fact") ) {
        task->module_power_fact = atof(text[1]);
        printf("模块功率因数: %s\n", text[1]);
    } else if ( 0 == strcmp(text[0], "single_module_A") ) {
        task->single_module_A = atof(text[1]);
        printf("单台模块额定电流: %s A\n", text[1]);
    } else if ( 0 == strcmp(text[0], "single_module_max_I") ) {
        task->single_module_max_I = atof(text[1]);
        printf("单台模块最高允许电流: %s A\n", text[1]);
    } else if ( 0 == strcmp(text[0], "charge_triger_V") ) {
        task->charge_triger_V = atof(text[1]);
        printf("充电触发电压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "default_bms") ) {
        strncpy((char *)task->bms_vendor_version, text[1], 16);
        printf("默认车载BMS驱动: %s\n", text[1]);
    }  else if ( 0 == strcmp(text[0], "module_max_V") ) {
        task->module_max_V = atof(text[1]);
        printf("模块最高输出电压: %s V\n", text[1]);
    }  else if ( 0 == strcmp(text[0], "module_min_V") ) {
        task->moudle_min_V = atof(text[1]);
        printf("模块最低输出电压: %s V\n", text[1]);
    } else if ( 0 == strcmp(text[0], "need_billing") ) {
        if ( text[1][0] == '0' ) {
            // 不计费
            bit_clr(task, F_NEED_BILLING);
            task->deal_methord = DEAL_NONE;
            printf("是否需要刷卡扣费: 不需要\n");
        } else if ( text[1][0] == '1' ) {
            // 卡片计费
            bit_set(task, F_NEED_BILLING);
            task->deal_methord = DEAL_USE_CARD;
            printf("是否需要刷卡扣费: 离线式计费\n");
        } else if ( text[1][0] == '2' ) {
            // 在线计费
            bit_set(task, F_NEED_BILLING);
            task->deal_methord = DEAL_USE_NET;
            printf("是否需要刷卡扣费: 在线式计费\n");
        } else if ( text[1][0] == '3' ) {
            // 在线,卡片计费
            bit_set(task, F_NEED_BILLING);
            task->deal_methord = DEAL_USE_BOTH;
            printf("是否需要刷卡扣费: 在线式,卡片计费\n");
        }
    } else if ( 0 == strcmp(text[0], "manneed_assister") ) {
        if ( text[1][0] == '0' ) {
            bit_clr(task, F_MANUL_CHARGE_NEED_ASSITER);
        } else {
            bit_set(task, F_MANUL_CHARGE_NEED_ASSITER);
        }
        printf("手动充电需要辅助电源: %s\n",
               bit_read(task, F_MANUL_CHARGE_NEED_ASSITER) ? "需要" : "不需要");
    } else if ( 0 == strcmp(text[0], "sys_need_kwh_meter") ) {
        if ( text[1][0] == '0' ) {
            bit_clr(task, F_NEED_KWH_METER);
        } else {
            bit_set(task, F_NEED_KWH_METER);
        }
    } else if ( 0 == strcmp(text[0], "kwh_meter_type") ) {
        if ( text[1][0] == '1' ) {
            task->meter_type = KWH_METER_HUALI_3AC;
        } else if ( text[1][0] == '2 ' ) {
            task->meter_type = KWH_METER_YADA_DC;
        } else {
            task->meter_type = KWH_METER_NONE;
        }
    } else if ( 0 == strcmp(text[0], "kaichu_syschg") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_SYSCHG] = 1 << shift;
            task->kaichu_map[KAICHU_SYSCHG] = shift - 1;
            printf("充电灯开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_SYSCHG] = 0;
            task->kaichu_map[KAICHU_SYSCHG] = 0xFF;
            printf("充电灯开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_syserr") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_SYSERR] = 1 <<shift;
            task->kaichu_map[KAICHU_SYSERR] = shift - 1;
            printf("系统故障开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_SYSERR] = 0;
            task->kaichu_map[KAICHU_SYSERR] = 0xFF;
            printf("系统故障开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun1_sw") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN1_SW] = 1 << shift;
            task->kaichu_map[KAICHU_GUN1_SW] = shift - 1;
            printf("1#枪接触器开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN1_SW] = 0;
            task->kaichu_map[KAICHU_GUN1_SW] = 0xFF;
            printf("1#枪接触器开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun2_sw") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN2_SW] = 1 << shift;
            task->kaichu_map[KAICHU_GUN2_SW] = shift - 1;
            printf("2#枪接触器开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN2_SW] = 0;
            task->kaichu_map[KAICHU_GUN2_SW] = 0xFF;
            printf("2#枪接触器开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun1_assit12") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN1_ASSIT_12] = 1 << shift;
            task->kaichu_map[KAICHU_GUN1_ASSIT_12] = shift - 1;
            printf("1#枪12V辅助电源开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN1_ASSIT_12] = 0;
            task->kaichu_map[KAICHU_GUN1_ASSIT_12] = 0xFF;
            printf("1#枪12V辅助电源开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun1_assit24") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN1_ASSIT_24] = 1 << shift;
            task->kaichu_map[KAICHU_GUN1_ASSIT_24] = shift - 1;
            printf("1#枪24V辅助电源开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN1_ASSIT_24] = 0;
            task->kaichu_map[KAICHU_GUN1_ASSIT_24] = 0xFF;
            printf("1#枪24V辅助电源开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun2_assit12") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN2_ASSIT_12] = 1 << shift;
            task->kaichu_map[KAICHU_GUN2_ASSIT_12] = shift - 1;
            printf("2#枪12V辅助电源开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN2_ASSIT_12] = 0;
            task->kaichu_map[KAICHU_GUN2_ASSIT_12] = 0xFF;
            printf("2#枪12V辅助电源开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun2_assit24") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN2_ASSIT_24] = 1 << shift;
            task->kaichu_map[KAICHU_GUN2_ASSIT_24] = shift - 1;
            printf("2#枪24V辅助电源开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN2_ASSIT_24] = 0;
            task->kaichu_map[KAICHU_GUN2_ASSIT_24] = 0xFF;
            printf("2#枪24V辅助电源开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun1_lock") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN1_LOCK] = 1 << shift;
            task->kaichu_map[KAICHU_GUN1_LOCK] = shift - 1;
            printf("1#枪电子锁开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN1_LOCK] = 0;
            task->kaichu_map[KAICHU_GUN1_LOCK] = 0xFF;
            printf("1#枪电子锁开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_gun2_lock") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_GUN2_LOCK] = 1 << shift;
            task->kaichu_map[KAICHU_GUN2_LOCK] = shift - 1;
            printf("2#枪电子锁开出口: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_GUN2_LOCK] = 0;
            task->kaichu_map[KAICHU_GUN2_LOCK] = 0xFF;
            printf("2#枪电子锁开出口: 无\n");
        }
    } else if ( 0 == strcmp(text[0], "kaichu_ac_flag") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_AC_LED] = 1 << shift;
            task->kaichu_map[KAICHU_AC_LED] = shift - 1;
            printf("市电灯: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_AC_LED] = 0;
            task->kaichu_map[KAICHU_AC_LED] = 0xFF;
            printf("市电灯: 无\n");
        }
    }  else if ( 0 == strcmp(text[0], "kaichu_light_sw") ) {
        int shift = atoi(text[1]);
        if ( shift ) {
            task->kaichu_mask[KAICHU_LIGHT] = 1 << shift;
            task->kaichu_map[KAICHU_LIGHT] = shift - 1;
            printf("照明灯板: %d\n", shift);
        } else {
            task->kaichu_mask[KAICHU_LIGHT] = 0;
            task->kaichu_map[KAICHU_LIGHT] = 0xFF;
            printf("照明灯板: 无\n");
        }
    }  else if ( 0 == strcmp(text[0], "ac_input_hi") ) {
        task->ac_input_hi = atof(text[1]);
    } else if ( 0 == strcmp(text[0], "ac_input_lo") ) {
        task->ac_input_lo = atof(text[1]);
    } else if ( 0 == strcmp(text[0], "assit_12v_24v_methord") ) {
        int methord = atoi(text[1]);
        if ( 1 == methord ) {
            task->assit_select_methord = ASSIT_SELECT_AUTO;
        } else {
            task->assit_select_methord = ASSIT_SELECT_MANUAL;
        }
    } else if ( 0 == strcmp(text[0], "server_addr") ) {
        task->ac_input_lo = atof(text[1]);
        strncpy(task->server_addr, text[1], 32);
        printf("服务器地址: %s\n", text[1]);
    }  else if ( 0 == strcmp(text[0], "server_port") ) {
        task->server_port = atoi(text[1]);
        printf("服务器端口: %d\n", task->server_port);
    }  else if ( 0 == strcmp(text[0], "local_addr") ) {
        task->charger_sn = atoi(text[1]);
        printf("本机地址: %d\n", task->charger_sn);
    } else if ( 0 == strcmp(text[0], "comm_mode") ) {
        if ( text[1][0] == '0' ) {
            bit_clr(task, F_SYSTEM_COMM_MODE);
        } else {
            bit_set(task, F_SYSTEM_COMM_MODE);
        }
        printf("通讯模式: %s\n", bit_read(task, F_SYSTEM_COMM_MODE)?"在线式":"离线式");
    } else if ( 0 == strcmp(text[0], "lightup_time") ) {
        printf("照明开始于: %s\n", text[1]);
        strncpy(task->lightup_time, text[1], 16);
    } else if ( 0 == strcmp(text[0], "lightup_hours") ) {
        task->lightup_hours = atof(text[1]);
        printf("照明时长: %s\n", text[1]);
    }
    return 0;
}

// 添加串口使用模组
int sql_rs485_result(void *param, int nr, char **text, char **name) {
    const struct binder_proc {
        char *id;
        int (*user_evt_handle)(struct bp_uart *self, struct bp_user *me, BP_UART_EVENT evt,
                               struct bp_evt_param *param);
    }plugins[] = {
        {"00000001", simple_box_read_evt_handle},
        {"C000000D", simple_box_correct_write_evt_handle},
        {"0000000E", simple_box_write_evt_handle},
        {"00000016", simple_box_configwrite_evt_handle},
        {"C0000012", simple_box_correct_read_evt_handle},
        {"C0000011", simple_box_correct_refer_V_evt_handle},
        {"00000002", card_reader_handle},
        {"00000003", card_init_handle},
        {"I000000F", cardreader_install_handle},
        {"00000004", uart4_charger_config_evt_handle},
        {"00000005", uart4_charger_yaoce_0_49_handle},
        {"00000006", uart4_charger_yaoce_50_100_handle},
        {"00000019", uart4_charger_module_evt_handle},
        // {{ 这个顺序不要调换，为了规避读卡器导致的模块转换盒通信中断问题。
        {"00000007", ANC01_convert_box_write_evt_handle},
        {"00000008", ANC01_convert_box_read_evt_handle},
        {"00000013", ANC01_convert_box_module_on_evt_handle},
        {"00000014", ANC01_convert_box_module_off_handle},

        // }}
        {"00000009", Increase_convert_box_write_evt_handle},
        {"M000000A", Increase_module_read_evt_handle},
        {"M0000015", Increase_module_write_evt_handle},
        {"0000000B", huali_3AC_kwh_meter_read_evt_handle},
        {"0000000C", huali_3AV_voltage_meter_read_evt_handle},
        {"I0000010", huali_3AC_kwh_meter_install_evt_handle},

        {"00000017", yada_DC_kwh_meter_read_evt_handle},
        {"00000018", yada_DC_current_meter_read_evt_handle},
        {NULL, NULL}
    };
    struct bp_user u;
    struct bp_uart *bp = NULL;
    int i = 0;
    struct charge_task *t = (struct charge_task *)param;
    int d, p, s, rs485;

    if ( nr <= 0 ) return 0;
    // "id, name, private, RS485, bps, parity, datalen, stop_bit, frame_freq, "
    // "died_line, swap2rx_time, ttw"
    memset(&u, 0, sizeof(struct bp_user));

    for ( i = 0; plugins[i].id != NULL; i ++ ) {
        if ( 0 != strcmp(plugins[i].id, text[0]) ) continue;
        u.user_evt_handle = plugins[i].user_evt_handle;
        break;
    }
    if ( u.user_evt_handle == NULL ) {
        log_printf(WRN, "ZEUS: 不能绑定串口模组， 名称为%s 类为%s的模组找到.", text[1], text[0]);
        return 0;
    }
    strncpy(u.name, text[1], 32);
    u._private = (void*)atoi(text[2]);
    rs485 = atoi(text[3]);
    u.hw_bps = atoi(text[4]);
    d = atoi(text[6]);
    p = text[5][0];
    s = atoi(text[7]);
    u.hw_other = MAKE_UART_CFG(d, p, s);
    u.frame_freq = atoi(text[8]);
    u.died_line = atoi(text[9]);
    u.swap_time_modify = atoi(text[10]);
    u.ttw = atoi(text[11]);

    for ( i = 0; i < 2; i ++ ) {
        if ( rs485 == 4 && t->uarts[i]->hw_port == SERIAL4_CTRL_PIN ) {
            u.uart = t->uarts[i];
            bp = t->uarts[i];
        } else if ( rs485 ==5 && t->uarts[i]->hw_port == SERIAL5_CTRL_PIN) {
            u.uart = t->uarts[i];
            bp = t->uarts[i];
        }
    }
    if ( bp == NULL ) {
        log_printf(WRN, "ZEUS: 不能绑定串口模组[%s]到串口%d.", text[1], rs485);
        return 0;
    }


    u.chargers = t->chargers[0];
    u.measure = t->measure[0];

    if ( text[0][0] == 'M' ) {
        // 整流模块
        for (i = 0; i < t->modules_nr; i ++ ) {
            u._private = i + 1;
            log_printf(INF, "ZEUS: 绑定串口模组(模块)[ %s.%s:%d ] <==>  %s.--- %s",
                       text[0], text[1], i +1, bp->dev_name,
                    bp_user_bind(bp, &u) ? RED("ERR"):GRN("OK"));
        }
        return 0;
    } else {
        log_printf(INF, "ZEUS: 绑定串口模组[ %s.%s ] <==>  %s.--- %s",
                   text[0], text[1], bp->dev_name,
                bp_user_bind(bp, &u) ? RED("ERR"):GRN("OK"));
    }

    return 0;
}

#define true_express(i) (keyerr[i] == '1' || keyerr[i] == 't' || keyerr[i] =='T'|| keyerr[i] == 'y' || keyerr[i] == 'Y')
#define false_express(i) (keyerr[i] == '0' || keyerr[i] == 'f' || keyerr[i] =='F'|| keyerr[i] == 'n' || keyerr[i] == 'N')
/* 充电任务服务线程
 * 充电状态的转换触发条件都由UI转发过来，这样做的目的是为了保证触发唯一和触发条件的同步。
 *
 * 1. 触发信号的采集过程由服务器完成；
 * 2. 将该信号传递（同步）给浏览器，浏览器确认后向服务器；
 * 3. 发送状态转换触发条件，完成状态转换。
 */
void *thread_charge_task_service(void *arg) ___THREAD_ENTRY___
{
    char sql[256];
    char buff[32] = {0};
    int bms_vendor_id;
    char bms_version[32] = {0};
    int ret, done = 0;
    char *errmsg;
    int tsp = 0;

    log_printf(DBG_LV1, "ZUES: %s running...sizeof(struct charge_task)=%d",
            __FUNCTION__, sizeof(struct charge_task));
    // 恢复默认配置
    task->max_output_I = 10.0f; // 10.0A
    task->limit_output_I = 0.0f;
    task->limit_max_V = 750.0f;
    task->limit_min_V = 400.0f;
    task->running_V = 450.0f;
    task->running_I = 0.0;
    task->modules_nr = 14;
    task->charge_stat = 0x0000; // 不充电
    task->flq_xishu = 150;
    task->bus_1_v_hi = 750.1;
    task->bus_1_v_lo = 190.0;
    task->bus_2_v_hi = 750.1;
    task->bus_2_v_lo = 190.0;
    task->bat_1_v_hi = 750.1;
    task->bat_1_v_lo = 190.0;
    task->bat_2_v_hi = 750.1;
    task->bat_2_v_lo = 199.5;
    task->bat_1_I_hi = 750.1;
    task->bat_2_I_hi = 240.0;
    task->bat_1_insti_ohm_v = 100.0;
    task->bat_2_insti_ohm_v = 100.0;
    task->meter_I_xishu = 100.0f;
    task->meter_V_xishu = 1.0f;
    bit_clr(task, F_NEED_KWH_METER); // 默认不使用电能表
    task->meter_type = KWH_METER_NONE;
    task->charge_triger_V = 5.0f;
    strcpy(task->bms_vendor_version, "0_1.0");

    memset(task->modules_on_off, 0x80, sizeof(task->modules_on_off)); // 全开机
    task->uipage = UI_PAGE_MAIN;
    bit_read(task, CMD_MODULE_ON);
    __bit_set(task->h_s_ware_config, HW_CMD_BUZZER_BEEP_STARTUP);

    sprintf(sql, "SELECT * FROM configs");
    ret = sqlite3_exec(task->database, sql, sql_db_config_result, &done, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }
    sprintf(sql, "SELECT key,current_value FROM settings");
    ret = sqlite3_exec(task->database, sql, sql_db_settings_result, &done, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }

    log_printf(DBG_LV1, "ZEUS: 数据库初始化完成....%d", done);
    print_POST_configure();

    // 启动八公定时器
    Hachiko_init(config_read("HachikoTTL"));

    // 初始化系统操作日志
    initlize_system_log(task->database, 16, 8);

    // 加载默认的扩展程序
    if ( ! exso_load(&task->exsos, "default", "/usr/zeus/plugins/exso_default.so", task) ) {
        log_printf(WRN, "ZEUS: load default plugin faile.");
    } else {
        log_printf(INF, "ZEUS: default plugin loaded.");
    }

    if ( bit_read(task, F_SYSTEM_COMM_MODE) ) {
        // 在线式启用初始化
        pandora_init(task);
    }

    /* 方案1：
     *   一组充电机， 一个采样盒，两把枪
     *   两把枪分时充电
     *
     *  串口服务线程:   2 个{电度表,[采样盒,充电机] }
     *    暂时先忽略电度表线程,后期可能会添加串口IO方式的读卡器
     */
    if ( task->sys_charge_group_nr == 1 &&
         task->sys_simple_box_nr == 1   &&
         (task->sys_config_gun_nr == 2||task->sys_config_gun_nr == 1) ) {
        struct bp_uart * bp = (struct bp_uart*)malloc(sizeof(struct bp_uart));
        if ( NULL == bp ) {
            ret = ERR_LOW_MEMORY;
            log_printf(ERR, "ZEUS: 分配系统内存失败");
            goto __panic;
        }
        task->uarts[0] = bp;
        memset(bp, 0, sizeof(struct bp_uart));
        bp->bp_evt_handle = uart4_bp_evt_handle;
        if ( ! task->sys_uart_name[0][0] ) {
            log_printf(WRN, "ZEUS: 未配置采样盒通信用RS485设备文件, 使用默认值.");
            strcpy(task->sys_uart_name[0], "/dev/ttyO4");
        }
        strcpy(bp->dev_name, task->sys_uart_name[0]);
        bp->dev_handle = -1;
        bp->status = BP_UART_STAT_INVALID;
        bp->hw_status = BP_UART_STAT_INVALID;
        bp->role = BP_UART_MASTER;
        bp->init_magic = 0;
        bp->hw_port = SERIAL4_CTRL_PIN;
        bp->users_nr = 0;
        bp->hw_bps = 9600;
        bp->hw_other = MAKE_UART_CFG(8, 'N', 1);
        bit_set(task, F_SYSTEM_CHARGE_ALLOW);

        task->chargers[0] = (struct charger_struct *)malloc(sizeof(struct charger_struct));
        memset(task->chargers[0], 0, sizeof(struct charger_struct));
        task->measure[0] = (struct measure_struct *)malloc(sizeof(struct measure_struct));
        memset(task->measure[0], 0, sizeof(struct measure_struct));
        task->sys_type = SYSTEM_YITISHI;

        log_printf(INF, "ZEUS.TSK: 作业结构初始化完成.");
    }

    do {
        struct bp_uart * bp = (struct bp_uart*)malloc(sizeof(struct bp_uart));
        if ( NULL == bp ) {
            ret = ERR_LOW_MEMORY;
            log_printf(ERR, "ZEUS: 分配系统内存失败");
            goto __panic;
        }
        task->uarts[1] = bp;
        memset(bp, 0, sizeof(struct bp_uart));
        bp->bp_evt_handle = uart4_bp_evt_handle;
        if ( ! task->sys_uart_name[1][0] ) {
            log_printf(WRN, "ZEUS: 未配置电表/读卡器通信用RS485设备文件, 使用默认值.");
            strcpy(task->sys_uart_name[1], "/dev/ttyO5");
        }
        strcpy(bp->dev_name, task->sys_uart_name[1]);
        bp->dev_handle = -1;
        bp->status = BP_UART_STAT_INVALID;
        bp->hw_status = BP_UART_STAT_INVALID;
        bp->role = BP_UART_MASTER;
        bp->init_magic = 0;
        bp->hw_port = SERIAL5_CTRL_PIN;
        bp->hw_bps = 2400;
        bp->hw_other = MAKE_UART_CFG(8, 'O', 1);
    } while ( 0 );

    sprintf(sql,
            "SELECT "
            "id, name, private, RS485, bps, parity, datalen, stop_bit, frame_freq, "
            "died_line, swap2rx_time, ttw"
            " FROM RS485_config WHERE disabled='false' AND class='%s' order by id",
            task->sys_work_mode
            );
    ret = sqlite3_exec(task->database, sql, sql_rs485_result, task, &errmsg);
    if ( ret ) {
        log_printf(ERR, "TOM: SQL error: %s", errmsg);
    }
    log_printf(INF, "ZEUS: 绑定串口模组完成%s: %s",
               task->uarts[0]->dev_name,
            task->uarts[1]->dev_name);

#if 1
    if ( task->uarts[0] && task->uarts[0]->users_nr ) {
        // 串口通信线程
        ret = pthread_create( & task->tid, &task->attr, thread_uart_service, (void*)task->uarts[0]);
        if ( 0 != ret ) {
            ret  = 0x1006;
            log_printf(ERR,
                       "UART framework start up.                       FAILE!!!!");
            goto __panic;
        }
    }

    if ( task->uarts[1] && task->uarts[1]->users_nr ) {
        // 串口通信线程
        ret = pthread_create( & task->tid, &task->attr, thread_uart_service, (void*)task->uarts[1]);
        if ( 0 != ret ) {
            ret  = 0x1006;
            log_printf(ERR,
                       "UART framework start up.                       FAILE!!!!");
            goto __panic;
        }
    }

    if ( task->uarts[2] && task->uarts[1]->users_nr ) {
        // 串口通信线程
        ret = pthread_create( & task->tid, &task->attr, thread_uart_service, (void*)task->uarts[2]);
        if ( 0 != ret ) {
            ret  = 0x1006;
            log_printf(ERR,
                       "UART framework start up.                       FAILE!!!!");
            goto __panic;
        }
    }
#endif

    task->commit_head = NULL;
    task->wait_head = NULL;
    task->wait_job_nr = 0;
    memset(task->job, 0, sizeof(task->job));
    pthread_mutex_init(&task->commit_lck, NULL);
    pthread_mutex_init(&task->wait_lck, NULL);
    pthread_mutex_init(&task->err_list_lck, NULL);

    task->bmsdriver = NULL;
    bms_vendor_id = __string_to_bms_vendor(task->bms_vendor_version);
    if ( 0 == __string_to_bms_version(task->bms_vendor_version, bms_version ) ) {
        task->bmsdriver = bmsdriver_search(task, bms_vendor_id, bms_version);
        log_printf(INF, "ZEUS: Load default BMS %d %s", bms_vendor_id, bms_version);
    }
    bmsdriver_init(task);

    //memset(task->single, 255, sizeof(task->single));

    system_log(0, "主程序启动");

    while ( 1 ) {
        // 清除无效的作业
        job_detach_wait(task);

        deal_with_system_protection(task, NULL);

        do {
            // 处理提交事件
            struct job_commit_data *thiz = NULL;
            thiz = job_select_commit(task);
            while ( thiz ) {
                switch ( thiz->cmd ) {
                case COMMIT_CMD_FORK:
                    job_fork(task, thiz);
                    break;
                case COMMIT_CMD_DONE:
                    break;
                case COMMIT_CMD_ABORT:
                    break;
                }
                memset(thiz, 0, sizeof(struct job_commit_data));
                thiz = job_select_commit(task);
                free(thiz);
            }
        } while ( 0 );

        do {
            // 执行作业
            int i = 0;
            for( i = 0; i < task->sys_config_gun_nr; i ++ ) {
                if ( task->job[ i ] == NULL ) continue;
                job_running(task, task->job[ i ]);
                if ( task->job[i]->job_status == JOB_DIZZY ) {
                    free(task->job[i]);
                    log_printf(INF, "ZEUS: 销毁完成..");
                    task->job[i] = NULL;
                    continue;
                }
            }
        } while ( 0 );

        do {
            // 选取可执行的作业并准备执行
            struct charge_job *job;
            if ( task->sys_config_gun_nr == 2 && task->sys_charge_group_nr == 1 ) {
	        if ( !task->job[0] && !task->job[1] ) {
		    char buff[16];
		    sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f) );
		    config_write("需求电压", buff);
		}
                if ( task->job[0] || task->job[1] ) break;

                job = job_select_wait(task, GUN_SN0);
                if ( job ) {
                    // ...
                    task->job[0] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                    break;
                }

                job = job_select_wait(task, GUN_SN1);
                if ( job ) {
                    task->job[1] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                }
            }
            if ( task->sys_config_gun_nr == 2 && task->sys_charge_group_nr == 2 ) {
                job = job_select_wait(task, GUN_SN0);
                if ( job ) {
                    task->job[0] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                    // ...
                }

                job = job_select_wait(task, GUN_SN1);
                if ( job ) {
                    task->job[1] = job;
                    log_printf(INF, "ZEUS: 开始执行作业 @ GUN%d:%d",
                               job->job_gun_sn, task->wait_job_nr);
                    // ...
                }
            }
            // {{{ 没实现
            if ( task->sys_config_gun_nr == 4 && task->sys_charge_group_nr == 2 ) {
            }
            if ( task->sys_config_gun_nr == 4 && task->sys_charge_group_nr == 4 ) {
            }
            //}}} 没实现

            exso_do_mainloop( & task->exsos );
        } while ( 0 );

        tsp ++;
        // 每30秒主动同步一次操作日志
#if 0
        if ( tsp >= 600 ) {
            sync_system_log();
            tsp = 0;
        } else if ( tsp % 100 == 0 ) {
            if ( task->job[0] && task->job[0]->job_status < JOB_DETACHING ) {
                job_export(task->job[0]);
            }
        }
#endif
        usleep(50000);
    }
__panic:
    log_printf(ERR, "ZEUS: 关键错误，系统退出 %d", ret);
    __get_timestamp(buff);
    sprintf(sql, "INSERT INTO log VALUES('%s', '关键错误，系统退出 %d", buff, ret);
    sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    sqlite3_close(task->database);
    exit(ret);
    return NULL;
}

void deal_with_system_protection(struct charge_task *tsk, struct charge_job *thiz)
{
    int ei, n = 0;

    // 判定充电机组故障
    // 当全部的充电机故障后才可判定充电机组故障
    ei = 0;
    for ( n = 0; n < tsk->modules_nr; n ++ ) {
        if ( bit_read(tsk, S_CHARGE_M_1_ERR + n) ) {
           ei ++;
        }
    }
    if ( ei >= tsk->modules_nr && tsk->modules_nr ) {
        bit_set(tsk, S_CHARGE_GROUP_ERR);
    } else {
        bit_clr(tsk, S_CHARGE_GROUP_ERR);
    }

    n = 0;
    for ( ei = S_ERROR + 1; ei < S_END; ei ++ ) {
        if ( bit_read(tsk, ei) ) {
            error_history_begin(thiz, ei, "N/A");
            n ++;
        } else {
            error_history_recover(thiz, ei);
        }
    }

    if ( n ) {
        bit_set(task, S_ERROR);
    } else {
        bit_clr(task, S_ERROR);
    }


    return;
}

int job_delay_exit(struct charge_job *job)
{
	time_t t = time(NULL);

	if (job->delay_exit_tsp == 0) {
		job->delay_exit_tsp = t;
		log_printf(INF, "JOB: 退出,开始15秒倒计时！");
		return 0;
	}

	if (t - job->delay_exit_tsp >= 15) {
		log_printf(INF, "JOB: 正式退出！");
		job->job_status = JOB_DIZZY;
		return 1;
	}

	return 0;
}

void job_running(struct charge_task *tsk, struct charge_job *job)
{
    int ret;
    char buff[64] = {0};
    char sql[256];
    int start  = 0, end = 0, fault = 0;

    if ( job == NULL ) return;

    if ( job->job_gun_sn == GUN_SN0 ) {
        if ( bit_read(task, S_BAT_0_SHORT) ) {
            fault ++;
        }
        if ( bit_read(task, S_BAT_0_REVERT) ) {
            fault ++;
        }
        if ( bit_read(task, S_BAT_0_INSTITUDE) ) {
	    unsigned int ohm = task->measure[0]->measure.VinBAT0RESP < 
	      task->measure[0]->measure.VinBAT0RESN ? task->measure[0]->measure.VinBAT0RESP :
	      task->measure[0]->measure.VinBAT0RESN;
	    double batv = task->measure[0]->measure.VinBAT0 / 10.0;
	    
	    double ohm_v = ohm / batv;
	    ohm = ohm * 100;
	    
	    if ( ohm_v < 500 ) {	  
		fault ++;
	    } else {
	        log_printf(WRN, "institude  protection.");
	    }
        }
        if ( bit_read(task, S_DC_OUTPUT_0_TRIP) ) {
            fault ++;
        }

        // 1路输出跳闸
        if ( bit_read(task, S_DC_OUTPUT_0_TRIP) ) {
            fault ++;
        }
    } else if ( job->job_gun_sn == GUN_SN1 ) {
        if ( bit_read(task, S_BAT_1_SHORT) ) {
            fault ++;
        }
        if ( bit_read(task, S_BAT_1_REVERT) ) {
            fault ++;
        }
        if ( bit_read(task, S_BAT_1_INSTITUDE) ) {
	    unsigned int ohm = task->measure[0]->measure.VinBAT0RESP < 
	      task->measure[0]->measure.VinBAT0RESN ? task->measure[0]->measure.VinBAT0RESP :
	      task->measure[0]->measure.VinBAT0RESN;
	    double batv = task->measure[0]->measure.VinBAT0 / 10.0;
	    
	    double ohm_v = ohm / batv;
	    ohm = ohm * 100;
	    
	    if ( ohm_v < 500 ) {	  
		fault ++;
	    } else {
	        log_printf(WRN, "institude  protection.");
	    }
	}
        if ( bit_read(task, S_DC_OUTPUT_1_TRIP) ) {
            fault ++;
        }

        // 2路输出跳闸
        if ( bit_read(task, S_DC_OUTPUT_1_TRIP) ) {
            fault ++;
        }
    }

    // 充电过程中充电枪脱落
    if ( job->job_gun_sn != __is_gun_phy_conn_ok(job) &&
         (
             job->job_status == JOB_WORKING ||
             job->job_status == JOB_ERR_PAUSE ||
             job->job_status == JOB_MAN_PAUSE
         )
    ) {
        fault ++;
        if ( job->job_gun_sn == GUN_SN0 ) {
            bit_set(task, S_GUN1_DROPED);
        } else if ( job->job_gun_sn == GUN_SN1 ) {
            bit_set(task, S_GUN2_DROPED);
        }
    } else {
        if ( job->job_gun_sn == GUN_SN0 ) {
            bit_clr(task, S_GUN1_DROPED);
        } else if ( job->job_gun_sn == GUN_SN1 ) {
            bit_clr(task, S_GUN2_DROPED);
        }
    }

    // 交流输出跳闸
    if ( bit_read(task, S_AC_SWITCH_TRIP) ) {
        fault ++;
    }
    // 直流总输出熔断器熔断
    if ( bit_read(task, S_DC_OUTPUT_RD) ) {
        fault ++;
    }
    // 直流总输出跳闸
    if ( bit_read(task, S_DC_OUTPUT_TRIP) ) {
        fault ++;
    }
    // 急停
    if ( bit_read(task, S_HALT) ) {
        fault ++;
    }

    // 综合采样盒通信故障
    if ( bit_read(task, S_MEASURE_1_COMM_DOWN) ) {
        fault ++;
    }
    // 电能表通信故障
    if ( bit_read(task, F_NEED_KWH_METER) &&
         bit_read(task, S_KWH_METER_COMM_DOWN) ) {
        fault ++;
    }
    // 充电机组通信故障
    if (bit_read(task, S_CONVERT_BOX_COMM_DOWN) ) {
        fault ++;
    }
    // 充电机组故障
    if ( bit_read(task, S_CHARGE_GROUP_ERR) ) {
        fault ++;
    }

    // 交流输入过压
    if ( bit_read(task, S_AC_INPUT_HI ) ) {
        fault ++;
    }
    // 交流输入欠压
    if ( bit_read(task, S_AC_INPUT_LO ) ) {
        fault ++;
    }

    // BMS 通讯故障
    if ( job->charge_mode == CHARGE_AUTO &&
         bit_read(job, JS_BMS_COMM_ERR) ) {
         fault ++;
    } else {
    }

    if ( fault ) {
        bit_clr(task, F_SYSTEM_CHARGE_ALLOW);
    } else {
        bit_set(task, F_SYSTEM_CHARGE_ALLOW);
    }

    switch ( job->job_status ) {
    case JOB_IDLE:
    case JOB_SETTING:
        job->bms.bms_read_init_ok = 0xFF;
        job->bms.bms_write_init_ok = 0xFF;
        job->bms.can_heart_beat.status = HACHIKO_INVALID;
        job->bms.can_tp_bomb.status = HACHIKO_INVALID;
        memset((void*)job->single, 0, sizeof(job->single));
        break;
    case JOB_12V_ASSIT_TEST:
    case JOB_24V_ASSIT_TEST:
    case JOB_SCANNING:
    case JOB_START:
    case JOB_WAITTING:
        job_wait(tsk, job);
        break;
    case JOB_STANDBY:
        task->chargers[0]->cstats = CHARGER_IDLE;
        bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        bit_clr(tsk, F_CHARGE_LED);
        if ( bit_read(job, JF_CMD_ABORT) ) {
            bit_clr(job, JF_CMD_ABORT);
            job->status_befor_fault = JOB_STANDBY;
            job->job_status = JOB_ABORTING;
            job->charge_exit_kwh_data = task->meter[0].kwh_zong;
            job->charge_stop_timestamp = time(NULL);
            end ++;
            log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
            break;
        }

        if ( bit_read(job, JF_BMS_TRM_CHARGE) ) {
            job->status_befor_fault = JOB_STANDBY;
            job->job_status = JOB_ABORTING;
            job->charge_exit_kwh_data = task->meter[0].kwh_zong;
            job->charge_stop_timestamp = time(NULL);
            end ++;
            log_printf(INF, "***** ZEUS(关键): 作业中止(BMS), 正在中止");
            break;
        }

        ret = __is_gun_phy_conn_ok(job);
        if ( ret == GUN_UNDEFINE || ret == GUN_INVALID ) {
            break;
        }

        // 连接完成 立即锁闭电子锁
        if ( job->job_gun_sn == GUN_SN0 ) {
            bit_set(tsk, CMD_GUN_1_LOCK_ON);
            if ( !bit_read(tsk, CMD_GUN_1_LOCK_ON) ) {
                log_printf(INF, "ZEUS: 电子锁闭合");
            }
        }
        if ( job->job_gun_sn == GUN_SN1 ) {
            bit_set(tsk, CMD_GUN_2_LOCK_ON);
            if ( !bit_read(tsk, CMD_GUN_2_LOCK_ON) ) {
                log_printf(INF, "ZEUS: 电子锁闭合");
            }
        }

        if (job->charge_implemention_timestamp == 0) {
            job->charge_implemention_timestamp = time(NULL);
        }

        // 充电模式为自动充电，需要和BMS通信，此时才需要将辅助电源合闸
        // 手动充电时，配置需要辅助电源时也将其合闸
        if ( job->charge_mode == CHARGE_AUTO ||
             (bit_read(task, F_MANUL_CHARGE_NEED_ASSITER) &&
              (job->charge_mode==CHARGE_BV||job->charge_mode==CHARGE_BI))) {
            if ( ret == GUN_SN0 ) {
                if ( 1 /*! bit_read(tsk, F_GUN_1_ASSIT_PWN_SWITCH_STATUS)*/ ) {
                    if ( !bit_read(tsk, S_ASSIT_PWN_ERR) ) {
                        if ( ! bit_read(tsk, CMD_GUN_1_ASSIT_PWN_ON) ) {
                            log_printf(INF, "ZEUS: 1# 充电枪辅助电源开始供电");
                        }
                    }
                    if ( task->kaichu_mask[KAICHU_GUN1_ASSIT_12] &&
                         task->kaichu_mask[KAICHU_GUN1_ASSIT_24] ) {
                        if ( job->assit_power_level == 0 ) {
                            /*
                             *  系统中包含12V和24V两种辅助电源
                             *  默认投入12V辅助电源, BMS无法识别后再投入24V电源
                             *
                             *  .NOTE:
                             *     注意这段代码的安全问题:
                             *     1. 通信物理通路可靠性是否足够。通讯线连接异常会导致
                             *        电源异常切换。
                             *     2. 软件可靠性是否足够。软件故障或操作系统故障均会导致
                             *        数据包无法接收和送达。
                             *
                             *  声明:
                             *     我不赞同这种切换方案!!!!
                             */
                            time_t running = time(NULL);
                            time_t ate = running - job->charge_implemention_timestamp;

                            if  (ate >= 0 && ate <= 10 ) {
                                if (!bit_read(tsk, F_GUN1_ASSIT_12V)) {
                                    log_printf(INF, "1#枪12V辅助电源投入.");
                                }
                                // 投入12V电源
                                bit_set(tsk, F_GUN1_ASSIT_12V);
                                bit_clr(tsk, F_GUN1_ASSIT_24V);
                            } else if ( ate > 10 && ate <= 15 ) {
                                if (bit_read(tsk, F_GUN1_ASSIT_12V)) {
                                    log_printf(INF, "关断1#枪12V辅助电源.");
                                }
                                // 关断12V电源
                                bit_clr(tsk, F_GUN1_ASSIT_12V);
                                bit_clr(tsk, F_GUN1_ASSIT_24V);
                            } else if ( ate > 15 && ate <= 25 ) {
                                if (!bit_read(tsk, F_GUN1_ASSIT_24V)) {
                                    log_printf(INF, "1#枪24V辅助电源投入.");
                                }
                                // 投入24V电源
                                bit_set(tsk, F_GUN1_ASSIT_24V);
                                bit_clr(tsk, F_GUN1_ASSIT_12V);
                            } else {
                                // 两种电源都无法使用, 无法充电
                                bit_clr(tsk, F_GUN1_ASSIT_12V);
                                bit_clr(tsk, F_GUN1_ASSIT_24V);
                                log_printf(INF, "BMS 无响应停止投切辅助电源.");
                            }
                        } else if ( job->assit_power_level == 12 ) {
                            // 手动选择12V 辅助电源
                            bit_set(tsk, F_GUN1_ASSIT_12V);
                            bit_clr(tsk, F_GUN1_ASSIT_24V);
                        } else if ( job->assit_power_level == 24 ) {
                            // 手动选择24V 辅助电源
                            bit_clr(tsk, F_GUN1_ASSIT_12V);
                            bit_set(tsk, F_GUN1_ASSIT_24V);
                        } else {
                            bit_clr(tsk, F_GUN1_ASSIT_12V);
                            bit_clr(tsk, F_GUN1_ASSIT_24V);
                        }
                    } else if ( task->kaichu_mask[KAICHU_GUN1_ASSIT_12]) {
                        bit_clr(tsk, F_GUN1_ASSIT_24V);
                        bit_set(tsk, F_GUN1_ASSIT_12V);
                        log_printf(DBG_LV1, "单独12V辅助电源.");
                    } else if ( task->kaichu_mask[KAICHU_GUN1_ASSIT_24]) {
                        bit_clr(tsk, F_GUN1_ASSIT_12V);
                        bit_set(tsk, F_GUN1_ASSIT_24V);
                        log_printf(DBG_LV1, "单独24V辅助电源.");
                    } else {
                        // 充电桩不需要辅助电源
                    }
                    bit_set(tsk, CMD_GUN_1_ASSIT_PWN_ON);
                }
            } else {
                if ( 1 /*! bit_read(tsk, F_GUN_2_ASSIT_PWN_SWITCH_STATUS)*/ ) {
                    if ( !bit_read(tsk, S_ASSIT_PWN_ERR) ) {
                        if ( ! bit_read(tsk, CMD_GUN_2_ASSIT_PWN_ON) ) {
                            log_printf(INF, "ZEUS: 2# 充电枪辅助电源开始供电");
                        }
                    }
                    if ( task->kaichu_mask[KAICHU_GUN2_ASSIT_12] &&
                         task->kaichu_mask[KAICHU_GUN2_ASSIT_24] ) {
                        if ( job->assit_power_level == 0 ) {
                            // 系统中包含12V和24V两种辅助电源
                            time_t running = time(NULL);
                            time_t ate = running - job->charge_implemention_timestamp;

                            if  (ate >= 0 && ate <= 10 ) {
                                if (!bit_read(tsk, F_GUN2_ASSIT_12V)) {
                                    log_printf(INF, "2#枪12V辅助电源投入.");
                                }
                                // 投入12V电源
                                bit_set(tsk, F_GUN2_ASSIT_12V);
                                bit_clr(tsk, F_GUN2_ASSIT_24V);
                            } else if ( ate > 10 && ate <= 15 ) {
                                if (bit_read(tsk, F_GUN2_ASSIT_12V)) {
                                    log_printf(INF, "关断2#枪12V辅助电源.");
                                }
                                // 关断12V电源
                                bit_clr(tsk, F_GUN2_ASSIT_12V);
                                bit_clr(tsk, F_GUN2_ASSIT_24V);
                            } else if ( ate > 15 && ate <= 25 ) {
                                if (!bit_read(tsk, F_GUN2_ASSIT_24V)) {
                                    log_printf(INF, "2#枪24V辅助电源投入.");
                                }
                                // 投入24V电源
                                bit_set(tsk, F_GUN2_ASSIT_24V);
                                bit_clr(tsk, F_GUN2_ASSIT_12V);
                            } else {
                                // 两种电源都无法使用
                                bit_clr(tsk, F_GUN2_ASSIT_12V);
                                bit_clr(tsk, F_GUN2_ASSIT_24V);
                            }
                        } else if ( job->assit_power_level == 12 ) {
                            // 手动选择12V 辅助电源
                            bit_set(tsk, F_GUN2_ASSIT_12V);
                            bit_clr(tsk, F_GUN2_ASSIT_24V);
                        } else if ( job->assit_power_level == 24 ) {
                            // 手动选择24V 辅助电源
                            bit_clr(tsk, F_GUN2_ASSIT_12V);
                            bit_set(tsk, F_GUN2_ASSIT_24V);
                        } else {
                            bit_clr(tsk, F_GUN2_ASSIT_12V);
                            bit_clr(tsk, F_GUN2_ASSIT_24V);
                        }
                    } else if ( task->kaichu_mask[KAICHU_GUN2_ASSIT_12]) {
                        bit_clr(tsk, F_GUN2_ASSIT_24V);
                        bit_set(tsk, F_GUN2_ASSIT_12V);
                    } else if ( task->kaichu_mask[KAICHU_GUN2_ASSIT_24]) {
                        bit_clr(tsk, F_GUN2_ASSIT_12V);
                        bit_set(tsk, F_GUN2_ASSIT_24V);
                    } else {
                        // 充电桩不需要辅助电源
                    }
                    bit_set(tsk, CMD_GUN_2_ASSIT_PWN_ON);
                }
            }

            // 自动充电模式下，以BMS握手成功为投切成功标识
            // 手动充电模式下，以母线上有电池高压信号为投切成功标识

            double bat_v = 0.0f;
            if ( job->job_gun_sn == GUN_SN0 ) {
                bat_v = __bytes2double(b2l(task->measure[0]->measure.VinBAT0));
            } else if ( job->job_gun_sn == GUN_SN1 ) {
                bat_v = __bytes2double(b2l(task->measure[0]->measure.VinBAT1));
            } else {

            }

            if (
                  ((job->charge_mode == CHARGE_AUTO) && (! bit_read(job, JF_BMS_BRM_OK)))||
                    (
                        (job->charge_mode == CHARGE_BI||job->charge_mode == CHARGE_BV) &&
                        bat_v < task->moudle_min_V
                    )
               ) {
                break;
            }
        }

        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
            job->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(%X)",
                       job->job_status);
            break;
        }

        job->job_status = JOB_WORKING;
        sprintf(sql, "UPDATE jobs set job_status='%d' where job_id='%ld'",
                job->job_status, job->jid);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
        log_printf(INF, "***** ZEUS(关键): 作业转为正式开始执行, 正在执行.");

        break;

    case JOB_WORKING:
        bit_set(tsk, F_CHARGE_LED);
        task->chargers[0]->cstats = CHARGER_WORK;
        sprintf(buff, "%d", (unsigned int)(job->need_V * 10.0f) );
        config_write("需求电压", buff);
        config_write("初始电压", buff);
        if ( job->job_gun_sn == GUN_SN0 ) {
            if ( job->need_I > task->bat_1_I_hi || 
	       job->need_I > task->single_module_A * task->modules_nr
	    ) {
                sprintf(buff, "%d", task->single_module_A * task->modules_nr);
            } else {
                sprintf(buff, "%d", (unsigned int)(job->need_I * 10.0f) );
            }
        } else if ( job->job_gun_sn == GUN_SN1 ) {
            if ( job->need_I > task->bat_2_I_hi || 
	       job->need_I > task->single_module_A * task->modules_nr ) {
                sprintf(buff, "%d", task->single_module_A * task->modules_nr);
            } else {
                sprintf(buff, "%d", (unsigned int)(job->need_I * 10.0f) );
            }
        } else {
            sprintf(buff, "0");
        }
        config_write("需求电流", buff);

        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
            bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
            bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
            job->status_befor_fault = JOB_WORKING;
            job->job_status = JOB_ERR_PAUSE;
            log_printf(WRN, "ZEUS: 系统发生关键故障, 自动暂停作业(JOB_WORKING)");
            break;
        } else {
            if ( job->job_gun_sn == GUN_SN0 ) {
                bit_set(task, F_GUN1_CHARGE);
            } else if ( job->job_gun_sn == GUN_SN1 ) {
                bit_set(task, F_GUN2_CHARGE);
            } else {

            }

            bit_set(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            ret = __is_gun_phy_conn_ok(job);
            if ( ret  == GUN_SN0 ) {
                bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);

                if ( ! bit_read(tsk, CMD_GUN_1_OUTPUT_ON) ) {
                    job->charge_begin_kwh_data = task->meter[0].kwh_zong;
                    job->charge_begin_timestamp = time(NULL);
                    start ++;
                }
                if ( job->charge_mode == CHARGE_AUTO ) {
                    bit_set(tsk, CMD_GUN_1_ASSIT_PWN_ON);
                }
                bit_set(tsk, CMD_GUN_1_OUTPUT_ON);
            } else if ( ret  == GUN_SN1 ) {
                bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);

                if ( ! bit_read(tsk, CMD_GUN_2_OUTPUT_ON) ) {
                    job->charge_begin_kwh_data = task->meter[0].kwh_zong;
                    job->charge_begin_timestamp = time(NULL);
                    start ++;
                }
                if ( job->charge_mode == CHARGE_AUTO ) {
                    bit_set(tsk, CMD_GUN_2_ASSIT_PWN_ON);
                }
                bit_set(tsk, CMD_GUN_2_OUTPUT_ON);
            } else {
                // 充电枪脱落
                if (job->job_gun_sn == GUN_SN0) {
                    bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
                } else if ( job->job_gun_sn == GUN_SN1 ) {
                    bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
                }
                job->status_befor_fault = JOB_WORKING;
                job->job_status = JOB_ERR_PAUSE;
                log_printf(WRN, "ZEUS: 充电枪脱落, 自动暂停作业(JOB_WORKING)");
                break;
            }

            job->section_kwh = task->meter[0].kwh_zong - job->charge_begin_kwh_data;
            job->section_seconds = time(NULL) - job->charge_begin_timestamp;
            job->charged_money = (job->charged_kwh + job->section_kwh) * task->kwh_price;

            // 有新的充电状态变化
            if ( start ) {
                sprintf(sql, "INSERT INTO job_billing VALUES("
                        "'%ld','%ld','0','0','%.2f','0.00','0.00','%.2f')",
                        job->jid,
                        job->charge_begin_timestamp,
                        job->charge_begin_kwh_data,
                        task->kwh_price);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
            }

            //{{ 在这做是否充完判定
            if (job->charge_billing.mode == BILLING_MODE_AS_CAP ) {
                if ( job->charged_kwh + job->section_kwh >= job->charge_billing.option.set_kwh ||
                     bit_read(job, JF_BMS_TRM_CHARGE) || bit_read(job, JF_CHG_TRM_CHARGE) ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始电量: %.2f KWH, "
                               "终止电量: %.2f KWH, 充电电量: %.2f KWH",
                               job->charge_begin_kwh_data,
                               task->meter[0].kwh_zong,
                               job->charged_kwh + job->section_kwh);
                    job->job_status = JOB_ABORTING;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
                double used_kwh = job->charged_kwh + job->section_kwh;
                if ( used_kwh * task->kwh_price >= job->charge_billing.option.set_money ||
                     bit_read(job, JF_BMS_TRM_CHARGE) || bit_read(job, JF_CHG_TRM_CHARGE) ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始电量: %.2f KWH, "
                               "终止电量: %.2f KWH, 充电电量: %.2f KWH",
                               job->charge_begin_kwh_data,
                               task->meter[0].kwh_zong,
                               used_kwh);
                    job->job_status = JOB_ABORTING;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_TIME ) {
                unsigned int used_seconds = job->charged_seconds + job->section_seconds;
                if ( used_seconds >= job->charge_billing.option.set_time ||
                     bit_read(job, JF_BMS_TRM_CHARGE) || bit_read(job, JF_CHG_TRM_CHARGE) ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始时戳: %ld, "
                               "终止时戳: %ld, 充电时长: %ld 秒",
                               job->charge_begin_timestamp,
                               job->charge_stop_timestamp,
                               used_seconds);
                    job->job_status = JOB_ABORTING;
                    end ++;
                }
            } else if ( job->charge_billing.mode == BILLING_MODE_AS_FREE ) {

            } else if ( job->charge_billing.mode == BILLING_MODE_AS_AUTO ) {
                if ( bit_read(job, JF_BMS_TRM_CHARGE) || bit_read(job, JF_CHG_TRM_CHARGE) ) {
                    job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                    job->charge_stop_timestamp = time(NULL);
                    log_printf(INF,
                               "ZEUS: 充电结束, 起始电量: %.2f KWH, "
                               "终止电量: %.2f KWH, 充电电量: %.2f KWH",
                               job->charge_begin_kwh_data,
                               task->meter[0].kwh_zong,
                               job->charged_kwh + job->section_kwh);
                    //job->job_status = JOB_DONE;
                    bit_set(job, JF_CMD_ABORT);
                    end ++;
                }
            } else {

            }

            //}}
            if ( bit_read(job, JF_CMD_ABORT) ) {
                bit_clr(job, JF_CMD_ABORT);
                job->status_befor_fault = JOB_WORKING;
                job->job_status = JOB_ABORTING;
                job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                job->charge_stop_timestamp = time(NULL);
                bit_set(job, JF_CHG_TRM_CHARGE);
                end ++;
                log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
            }

            if ( bit_read(job, JF_BILLING_DONE) ) {
                job->status_befor_fault = JOB_WORKING;
                job->job_status = JOB_ABORTING;
                job->charge_exit_kwh_data = task->meter[0].kwh_zong;
                job->charge_stop_timestamp = time(NULL);
                bit_set(job, JF_CHG_TRM_CHARGE);
                end ++;
                log_printf(INF, "***** ZEUS(关键): 作业终止，计费完成！");
            }

            // 充电作业发生状态变化
            if ( end ) {
                job->charged_kwh = job->charged_kwh + job->section_kwh;
                job->charged_seconds = job->charged_seconds + job->section_seconds;
                sprintf(sql,
                        "UPDATE job_billing SET b_end_timestamp='%ld',"
                        "b_end_kwh='%.2f' WHERE job_id='%ld' AND b_begin_timestamp='%ld'",
                        job->charge_stop_timestamp,
                        job->charge_exit_kwh_data,
                        job->jid,
                        job->charge_begin_timestamp);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);

                sprintf(sql,
                        "UPDATE jobs SET jos_status='%d' WHERE job_id='%ld'",
                        job->job_status,
                        job->jid);
                (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
                job->section_kwh = 0;
                job->section_seconds = 0;
            }

        }
        break;
    case JOB_ERR_PAUSE:
        sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f));
        config_write("需求电压", buff);
        config_write("初始电压", buff);
        config_write("需求电流", "0");
        bit_clr(tsk, F_CHARGE_LED);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);

        if ( bit_read(job, JF_CMD_ABORT) ) {
            bit_clr(job, JF_CMD_ABORT);
            log_printf(INF, "ZEUS: 充电任务中止(%X)",
                       job->status_befor_fault);
            job->job_status = JOB_ABORTING;
        }
        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {
        } else {
            ret = __is_gun_phy_conn_ok(job);
            if ( ret == GUN_UNDEFINE || ret == GUN_INVALID ) {
                break;
            }
            job->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 故障消除, 充电作业继续进行(%X)",
                       job->status_befor_fault);
        }
        break;
    case JOB_MAN_PAUSE:
        config_write("需求电流", "0");
        bit_clr(tsk, F_CHARGE_LED);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        if ( bit_read(job, JF_CMD_ABORT) ) {
            bit_clr(job, JF_CMD_ABORT);
            log_printf(INF, "ZEUS: 充电任务中止(%X:JOB_MAN_PAUSE)",
                       job->status_befor_fault);
            job->job_status = JOB_ABORTING;
        }
        if ( bit_read(job, CMD_JOB_RESUME) ) {
            bit_clr(job, CMD_JOB_RESUME);
            job->job_status = JOB_RESUMING;
            log_printf(INF, "ZEUS: 人工恢复作业(%X), 正在恢复",
                       job->status_befor_fault);
        }
        if ( ! bit_read(tsk, F_SYSTEM_CHARGE_ALLOW) ) {

        } else {
        }
        break;
    case JOB_RESUMING:
        bit_clr(tsk, F_CHARGE_LED);
        job->job_status = JOB_WORKING;
        break;
    case JOB_ABORTING:
        config_write("需求电流", "0");
        bit_clr(tsk, F_CHARGE_LED);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        bit_set(job, JF_CHG_TRM_CHARGE);

        job->job_status = JOB_DONE;
        log_printf(INF, "作业中止");

        break;
    case JOB_DONE:
        sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f));
        config_write("需求电压", buff);
        config_write("初始电压", buff);
        config_write("需求电流", "0");
        bit_clr(tsk, F_CHARGE_LED);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        if ( job->charge_abort_timestamp == 0 ) {
            job->charge_abort_timestamp = time(NULL);
        }

        if ( bit_read(task, F_NEED_BILLING) ) {
            if ( bit_read(job, JF_BILLING_DONE) ) {
                job->job_status = JOB_EXITTING;
                log_printf(INF, "JOB: 计费完成, 作业退出!");
            } else if ( bit_read(job, JS_BILLING_TIMEOUT) ) {
                job->job_status = JOB_EXITTING;
                log_printf(INF, "JOB: 计费超时, 交易数据暂存后退出作业!");
            } else {
                if ( time(NULL) - job->charge_abort_timestamp >= 15 ) {
                    bit_set(job, JS_BILLING_TIMEOUT);
                }
            }
        } else {
	    bit_read(job, JF_BILLING_DONE);
            job->job_status = JOB_EXITTING;
        }
        break;
    case JOB_EXITTING:
        sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f));
        config_write("需求电压", buff);
        config_write("初始电压", buff);
        config_write("需求电流", "0");
        task->chargers[0]->cstats = CHARGER_IDLE;
        bit_clr(tsk, F_CHARGE_LED);
        bit_clr(tsk, CMD_GUN_1_ASSIT_PWN_ON);
        bit_clr(tsk, CMD_GUN_2_ASSIT_PWN_ON);
        bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
        bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        bit_clr(tsk, F_GUN1_ASSIT_12V);
        bit_clr(tsk, F_GUN1_ASSIT_24V);
        bit_clr(tsk, F_GUN2_ASSIT_12V);
        bit_clr(tsk, F_GUN2_ASSIT_24V);
        if ( job->job_gun_sn == GUN_SN0 ) {
            bit_clr(tsk, F_GUN1_CHARGE);
            bit_clr(tsk, F_VOL1_SET_OK);
        }
        if ( job->job_gun_sn == GUN_SN1 ) {
            bit_clr(tsk, F_GUN2_CHARGE);
            bit_clr(tsk, F_VOL2_SET_OK);
        }
        job->job_status = JOB_DETACHING;
        break;
    case JOB_DETACHING:
        sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f));
        config_write("需求电压", buff);
        config_write("初始电压", buff);
        config_write("需求电流", "0");
        task->chargers[0]->cstats = CHARGER_IDLE;
        bit_clr(tsk, F_CHARGE_LED);
        if ( job->job_gun_sn == GUN_SN0 ) {
            if ( bit_read(tsk, CMD_GUN_1_LOCK_ON) ) {
                log_printf(INF, "ZEUS: 电子锁已经断开，等待充电枪拔出.");
            }
            bit_clr(tsk, CMD_GUN_1_LOCK_ON);
        }
        if ( job->job_gun_sn == GUN_SN1 ) {
            if ( bit_read(tsk, CMD_GUN_2_LOCK_ON) ) {
                log_printf(INF, "ZEUS: 电子锁已经断开，等待充电枪拔出.");
            }
            bit_clr(tsk, CMD_GUN_2_LOCK_ON);
        }
        if ( job->job_status >= JOB_WORKING ){
            bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
            bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
            bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
        }

        // 自动充电需要释放资源
        // * BMS驱动 超时,心跳定时器
        // * BMS驱动多包传输定时器
        // * BMS驱动 读数据线程引用关系
        // * BMS驱动 写数据线程引用关系
        // * BMS驱动 绑定关系
        // * BMS驱动 数据包发送器
        // * 作业控制结构本身
        // * 任务和作业之间的绑定关系
        if ( job->charge_mode == CHARGE_AUTO ) {
            if ( job->bms.can_heart_beat.status == HACHIKO_INVALID &&
                 job->bms.can_tp_bomb.status == HACHIKO_INVALID &&
                 job->bms.bms_read_init_ok == 0xFF &&
                 job->bms.bms_write_init_ok == 0xFF ) {
				job->bms.driver = NULL;
				if (job_delay_exit(job)) {
					free(job->bms.generator);
					job->job_status = JOB_DIZZY;
					log_printf(INF, "ZEUS.job: 作业清除引用完成.");
				}
            } else {
              log_printf(DBG_LV1, "%d  %d %x %x", job->bms.can_heart_beat.status,
                     job->bms.can_tp_bomb.status,
                     job->bms.bms_read_init_ok,
                     job->bms.bms_write_init_ok);
              Hachiko_kill(&job->bms.can_heart_beat);
              Hachiko_kill(&job->bms.can_tp_bomb);
              job->bms.can_heart_beat.status = HACHIKO_INVALID;
              job->bms.can_tp_bomb.status = HACHIKO_INVALID;
            }
        } else {
            //job->job_status = JOB_DIZZY;
			job_delay_exit(job);
        }
        break;
    case JOB_DIZZY:
        return;
    }

    // 充电作业发生状态变化
    if ( end ) {
        sprintf(sql,
                "UPDATE job_billing SET b_end_timestamp='%ld',"
                "b_end_kwh='%.2f' WHERE job_id='%ld' AND b_begin_timestamp='%ld'",
                job->charge_stop_timestamp,
                job->charge_exit_kwh_data,
                job->jid,
                job->charge_begin_timestamp);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);

        sprintf(sql,
                "UPDATE jobs SET jos_status='%d' WHERE job_id='%ld'",
                job->job_status,
                job->jid);
        (void)sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    }
}

int job_wait(struct charge_task *tsk, struct charge_job *job)
{
    int ret;
    char buff[64] = {0};
    char sql[256];
    int start  = 0, end = 0, fault = 0;

    sprintf(buff, "%d", (unsigned int)(task->moudle_min_V * 10.0f));
    config_write("需求电压", buff);
    config_write("初始电压", buff);
    bit_clr(tsk, CMD_DC_OUTPUT_SWITCH_ON);
    bit_clr(tsk, CMD_GUN_1_OUTPUT_ON);
    bit_clr(tsk, CMD_GUN_2_OUTPUT_ON);
    job->charge_abort_timestamp = 0;
    job->charge_implemention_timestamp = 0;

    if ( bit_read(job, JF_CMD_ABORT) ) {
        bit_clr(job, JF_CMD_ABORT);
        job->status_befor_fault = JOB_WAITTING;
        job->job_status = JOB_ABORTING;
        job->charge_exit_kwh_data = task->meter[0].kwh_zong;
        job->charge_stop_timestamp = time(NULL);
        end ++;
        log_printf(INF, "***** ZEUS(关键): 作业中止(人为), 正在中止");
        return 0;
    }

    if ( job->job_gun_sn == GUN_SN0 ) {
        if ( __bytes2double(b2l(task->measure[0]->measure.VinKM0)) > (task->moudle_min_V + 36.0f) ) {
            return 0;
        }
    } else if ( job->job_gun_sn == GUN_SN1 ) {
        if ( __bytes2double(b2l(task->measure[0]->measure.VinKM1)) > (task->moudle_min_V + 36.0f) ) {
            return 0;
        }
    }

    job->job_status = JOB_STANDBY;
    bit_clr(tsk, F_CHARGE_LED);
    if ( job->charge_mode == CHARGE_AUTO ) {
        int bms_vendor_id;
        char bms_version[32] = {0};

        bms_vendor_id = __string_to_bms_vendor(task->bms_vendor_version);
        if ( 0 == __string_to_bms_version(task->bms_vendor_version, bms_version ) ) {
            task->bmsdriver = bmsdriver_search(task, bms_vendor_id, bms_version);
            log_printf(INF, "ZEUS: Load default BMS %d %s", bms_vendor_id, bms_version);
        }
        bind_bmsdriver(bmsdriver_search(task, bms_vendor_id, bms_version), job);
    }
    return 0;
}

int job_standby(struct charge_task *, struct charge_job *);
int job_working(struct charge_task *, struct charge_job *);
int job_done(struct charge_task *, struct charge_job *);
int job_billing(struct charge_task *, struct charge_job *);

int job_commit(struct charge_task *tsk, const struct job_commit_data *jc, COMMIT_CMD cmd)
{
    struct job_commit_data *thiz = NULL;
    switch ( cmd ) {
    case COMMIT_CMD_FORK:
        thiz = (struct job_commit_data *)malloc(sizeof(struct job_commit_data));
        memcpy(thiz, jc, sizeof(struct job_commit_data));
        thiz->cmd = cmd;
        list_ini(thiz->job_node);
        pthread_mutex_lock(&task->commit_lck);
        if ( tsk->commit_head == NULL ) {
            tsk->commit_head = &thiz->job_node;
        } else {
            list_inserttail(tsk->commit_head, &thiz->job_node);
        }
        pthread_mutex_unlock (&task->commit_lck);
        break;
    case COMMIT_CMD_ABORT:
        break;
    }
    return 0;
}

void dump_commit_struct (struct job_commit_data *need)
{
    printf("\tcmd:%x\n", need->cmd);
    printf("\turl_commit_timestamp:%ld\n",need->url_commit_timestamp);
    printf("\tontom_commit_date_time:%ld\n", need->ontom_commit_date_time);
    printf("\tcharge_gun:%x\n", need->charge_gun);
    printf("\tcharge_mode:%x\n", need->charge_mode);
    printf("\tmanual_set_charge_volatage:%f\n", need->manual_set_charge_volatage);
    printf("\tmanual_set_charge_current:%f\n", need->manual_set_charge_current);
    printf("\tcard_passwd:%s\n", need->card_passwd);
    printf("\tcard_money_remain:%.2f\n", need->card_remain);
}

struct charge_job * job_fork(struct charge_task *tsk, struct job_commit_data *need)
{
    struct charge_job* thiz = NULL;
    char sql[512] = {0};
    char str_cmode[32], str_bmode[32];
    int ret, nr_gen = 0, s = 0, i;
    char *errmsg = NULL;

    log_printf(INF, "开始创建作业");

    thiz = (struct charge_job *)malloc(sizeof(struct charge_job));
    if (thiz == NULL) {
        log_printf(ERR, "ZEUS: LOW memory, job create faile, aborted.");
        // 中止作业
    }
    memset(thiz, 0, sizeof(struct charge_job));
    thiz->bms.can_dev = "can0";
    thiz->bms.can_bms_status = CAN_INVALID;
    thiz->bms.job = thiz;
    thiz->charged_kwh = 0.0f;
    thiz->section_kwh = 0.0f;
    thiz->charged_money = 0.0f;
    thiz->charged_seconds = 0;
    thiz->section_seconds = 0;
    thiz->assit_power_level = need->assit_power_level;
    thiz->tsk = tsk;
    list_ini(thiz->job_node);
    thiz->jid = need->url_commit_timestamp;
    thiz->charge_billing.mode = need->biling_mode;
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_CAP ) {
       thiz->charge_billing.option.set_kwh = need->as_kwh;
    }
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
        thiz->charge_billing.option.set_money = need->as_money;
    }
    if ( thiz->charge_billing.mode == BILLING_MODE_AS_TIME ) {
        thiz->charge_billing.option.set_time = need->as_time;
    }

    thiz->charge_mode = need->charge_mode;
    thiz->job_gun_sn = need->charge_gun;
    strcpy(thiz->card.triger_card_sn, need->card_sn);
    __card_write_passwd(&thiz->card, need->card_passwd);
    __card_write_remain(&thiz->card, need->card_remain);
    thiz->card.card.sector_4.data.magic = 0x4F4E5057;
    thiz->card.card.sector_4.data.reserved = 0;
    thiz->card.card.sector_4.data.unuesed = 0;

    if ( thiz->charge_mode != CHARGE_AUTO ) {
        thiz->need_I = need->manual_set_charge_current;
        thiz->need_V = need->manual_set_charge_volatage;
    }

    thiz->job_status = JOB_WAITTING;

    pthread_mutex_lock(&task->wait_lck);
    if (task->wait_head == NULL) {
        task->wait_head = &thiz->job_node;
        task->wait_job_nr = 1;
    } else {
        list_inserttail(task->wait_head, &thiz->job_node);
        task->wait_job_nr ++;
    }
    pthread_mutex_unlock (&task->wait_lck);

    sprintf(str_cmode, "auto");
    sprintf(str_bmode, "auto");
    if ( thiz->charge_mode == CHARGE_BV ) {
        sprintf(str_cmode, "BV;%.1f V;%.1f A;",
                thiz->need_V, thiz->need_I);
    } else if ( thiz->charge_mode == CHARGE_BI ) {
        sprintf(str_cmode, "BI;%.1f A;%.1f V;",
                thiz->need_I, thiz->need_V);
    }

    if ( thiz->charge_billing.mode == BILLING_MODE_AS_MONEY ) {
        sprintf(str_bmode, "AS_MONEY;%.1f 元;",
                thiz->charge_billing.option.set_money);
    } else if ( thiz->charge_billing.mode == BILLING_MODE_AS_TIME ) {
        sprintf(str_bmode, "AS_TIME;%d 秒;",
                thiz->charge_billing.option.set_time);
    } else if ( thiz->charge_billing.mode == BILLING_MODE_AS_CAP ) {
        sprintf(str_bmode, "AS_CAP;%.1f 度;",
                thiz->charge_billing.option.set_kwh);
    }
    sprintf(sql, "INSERT INTO jobs VALUES("
            "\'%ld\',\'%d\',\'%d\',\'%s\',\'%s\',\'%s\')",
            thiz->jid,
            thiz->job_status,
            thiz->job_gun_sn,
            thiz->card.triger_card_sn,
            str_cmode,
            str_bmode
            );
    ret = sqlite3_exec(tsk->database, sql, NULL, NULL, &errmsg);
    if ( ret ) {
        log_printf(ERR, "ZEUS: DATABASE error: %s", errmsg);
    }

    dump_commit_struct(need);

    thiz->charge_job_create_timestamp = time(NULL);

    log_printf(INF, "ZEUS: 作业创建完成(%p:%d:%ld).",
               thiz, task->wait_job_nr, thiz->jid);
    return thiz;

die:
    log_printf(ERR, "ZEUS: 创建作业失败");
    free(thiz);
    thiz = NULL;
    return thiz;
}

// 检查作业是否存在
int job_exsit(time_t id)
{
    int i = 0;
    struct list_head *thiz;
    struct charge_job *j = NULL;
    struct job_commit_data *c = NULL;

    for ( i = 0; i < sizeof(task->job)/sizeof(struct charge_job*); i ++) {
        if ( task->job[i] == NULL ) continue;
        if ( task->job[i]->jid == id ) {
            return (int)task->job[i];
        }
    }

    pthread_mutex_lock(&task->commit_lck);
    if ( task->commit_head ) {
        thiz = task->commit_head;
        do {
            c = list_load(struct job_commit_data, job_node, thiz);
            if ( c->url_commit_timestamp == id ) {
                break;
            }
            thiz = thiz->next;
            c = NULL;
        } while ( thiz != task->commit_head );
    }
    pthread_mutex_unlock (&task->commit_lck);
    if ( c ) return (int)c;

    pthread_mutex_lock(&task->wait_lck);
    if ( task->wait_head ) {
        thiz = task->wait_head;
        do {
            j = list_load(struct charge_job, job_node, thiz);
            if ( j->jid == id ) {
                break;
            }
            thiz = thiz->next;
            j = NULL;
        } while ( thiz != task->wait_head );
    }
    pthread_mutex_unlock (&task->wait_lck);

    return (int)j;
}

struct charge_job* job_search(time_t ci_timestamp)
{
    int i = 0;
    struct list_head *thiz;
    struct charge_job *j = NULL;

    log_printf(INF, "search for id %08X", ci_timestamp);

    for ( i = 0; i < sizeof(task->job)/sizeof(struct charge_job*); i ++) {
        if ( task->job[i] == NULL ) continue;
        if ( task->job[i]->jid == ci_timestamp ) {
            return task->job[i];
        }
    }

    pthread_mutex_lock(&task->wait_lck);
    if ( task->wait_head ) {
        thiz = task->wait_head;
        do {
            j = list_load(struct charge_job, job_node, thiz);
            if ( j->jid == ci_timestamp ) {
                break;
            }
            thiz = thiz->next;
            j = NULL;
        } while ( thiz != task->wait_head );
    }
    pthread_mutex_unlock (&task->wait_lck);

    return j;
}

// 从等待链表中选出最先提交的作业
struct charge_job * job_select_wait(struct charge_task *tsk, CHARGE_GUN_SN gun)
{
    struct charge_job *thiz = NULL;
    struct list_head *p, *next;
    pthread_mutex_lock(&tsk->wait_lck);
    if ( tsk->wait_head != NULL ) {
        p = tsk->wait_head;
        do {
            next = p->next;
            thiz = list_load(struct charge_job, job_node, p);
            if ( thiz->job_gun_sn != gun ) {
                p = p->next;
                thiz = NULL;
                continue;
            }
            list_remove(p);
            if ( p == tsk->wait_head ) {
                if ( next == tsk->wait_head ) {
                    tsk->wait_head = NULL;
                    tsk->wait_job_nr = 0;
                } else {
                    tsk->wait_head = next;
                    tsk->wait_job_nr --;
                }
            }
            break;
        } while ( p != tsk->wait_head);
    }
    pthread_mutex_unlock (&tsk->wait_lck);

    return thiz;
}

// 从等待列表中删除
void job_detach_wait(struct charge_task *tsk)
{
    struct charge_job *thiz = NULL;
    struct list_head *p, *next;
    pthread_mutex_lock(&tsk->wait_lck);
    if ( tsk->wait_head != NULL ) {
        p = tsk->wait_head;
        do {
            next = p->next;
            thiz = list_load(struct charge_job, job_node, p);
            //log_printf(INF, "id: %08X %x", thiz->jid,
            //           thiz->job_status);
            if ( thiz->job_status != JOB_DETACHING ) {
                p = p->next;
                thiz = NULL;
                continue;
            }
            list_remove(p);
            if ( p == tsk->wait_head ) {
                if ( next == tsk->wait_head ) {
                    tsk->wait_head = NULL;
                    tsk->wait_job_nr = 0;
                } else {
                    tsk->wait_head = next;
                    tsk->wait_job_nr --;
                }
            }
            log_printf(INF, "ZEUS: 作业 %ld 被释放", thiz->jid);
            free(thiz);
            break;
        } while ( p != tsk->wait_head);
    }
    pthread_mutex_unlock (&tsk->wait_lck);
}

// 从提交链表中取出第一个提交事件
struct job_commit_data *job_select_commit(struct charge_task *tsk)
{
    struct job_commit_data *thiz = NULL;
    pthread_mutex_lock(&tsk->commit_lck);
    if ( tsk->commit_head ) {
        struct list_head *next = tsk->commit_head->next;
        thiz = list_load(struct job_commit_data, job_node, tsk->commit_head);
        if ( next = tsk->commit_head ) {
            next = NULL;
        }
        list_remove(tsk->commit_head);
        tsk->commit_head = next;
    }
    pthread_mutex_unlock (&tsk->commit_lck);
    return thiz;
}

#define EXPORT_JOB_SIGNAL(job, sig) do {      \
        sprintf(buff, "echo %-32s:%d >> /tmp/zeus/job_%08X_single", #sig, bit_read(job, sig), (unsigned int)job->jid);\
    system(buff);} while (0)
#define EXPORT_FIRST_JOB_SIGNAL(job, sig) do {      \
        sprintf(buff, "echo %-32s:%d > /tmp/zeus/job_%08X_single", #sig, bit_read(job, sig), (unsigned int)job->jid);\
    system(buff);} while (0)
#define EXPORT_JOB_HEX(job, key, value) do {\
        sprintf(buff, "echo %32s: %08X >> /tmp/zeus/job_%08X_single", key, value, (unsigned int)job->jid);\
    system(buff);} while (0)
#define EXPORT_JOB_DEC(job, key, value) do {\
        sprintf(buff, "echo %32s: %d >> /tmp/zeus/job_%08X_single", key, value, (unsigned int)job->jid);\
    system(buff);} while (0)

#define JSON_status(id, status)do {\
        sprintf(buff, "printf {\\\"id\\\":\\\"%d\\\",\\\"v\\\":\\\"%d\\\"}, >> /tmp/rcm.json", id, status);\
        system(buff);} while (0)
#define JSON_formatint(id, value) do {\
        sprintf(buff, "printf {\\\"id\\\":\\\"%d\\\",\\\"v\\\":\\\"%d\\\"},  >> /tmp/rcm.json", id, value);\
        system(buff);} while (0)
#define JSON_formatfloat(id, value) do {\
        sprintf(buff, "printf {\\\"id\\\":\\\"%d\\\",\\\"v\\\":\\\"%.4f\\\"},  >> /tmp/rcm.json", id, value);\
        system(buff);} while (0)
// 将作业信息导出至文件系统
int job_export(struct charge_job *job)
{
    char *server_addr = "122.224.90.74:10008";
    char buff[256];
    time_t timep;
    struct tm *p;
    timep = time(NULL);
    p =localtime(&timep);

    sprintf(buff, "printf device_sn=ON-DC-%04d > /tmp/rcm.json", random() % 4);
    system(buff);
    sprintf(buff, "printf \\&datetime=%d-%d-%d\\ %d:%d:%d\\&type=上报 >> /tmp/rcm.json",
            p->tm_year + 1900,
            p->tm_mon + 1,
            p->tm_mday,
            p->tm_hour,
            p->tm_min,
            p->tm_sec);
    system(buff);

    sprintf(buff, "printf \\&json_text={\\\"task\\\":[ >> /tmp/rcm.json");
    system(buff);
    // {{ 任务状态
    do {
        unsigned int ei, i = 0;
        for ( ei = S_ERROR; ei < S_END; ei ++, i ++ ) {
            if ( bit_read(task, ei) ) {
                JSON_status( i + 54, 1);
            } else {
                continue;
            }
        }
    } while ( 0 );
    // }}

    //{{ 任务数据
    JSON_formatfloat(155, __bytes2double(task->measure[0]->measure.VinKM0));
    JSON_formatfloat(156, __bytes2double(task->measure[0]->measure.IoutBAT0));
    JSON_formatfloat(157, __bytes2double(task->measure[0]->measure.VinBAT0));
    JSON_formatfloat(158, __bytes2double(task->measure[0]->measure.IoutBAT0));
    JSON_formatfloat(159, __bytes2double(task->measure[0]->measure.VinKM1));
    JSON_formatfloat(160, __bytes2double(task->measure[0]->measure.IoutBAT1));
    JSON_formatfloat(161, __bytes2double(task->measure[0]->measure.VinBAT1));
    JSON_formatfloat(162, __bytes2double(task->measure[0]->measure.IoutBAT1));

    JSON_formatfloat(155, __bytes2double(task->measure[0]->measure.VinBAT0RESP));
    JSON_formatfloat(155, __bytes2double(task->measure[0]->measure.VinBAT0RESN));
    JSON_formatfloat(155, __bytes2double(task->measure[0]->measure.VinBAT1RESP));
    JSON_formatfloat(155, __bytes2double(task->measure[0]->measure.VinBAT1RESN));
    //}}
    sprintf(buff, "printf {\\\"id\\\":\\\"-1\\\",\\\"v\\\":\\\"-1\\\"}], >> /tmp/rcm.json");
    system(buff);

    sprintf(buff, "printf \\\"job\\\":[ >> /tmp/rcm.json");
    system(buff);
    // {{ 作业状态
    do {
        unsigned int ei, i = 0;
        for ( ei = JF_AUTO_CHARGE; ei < J_END; ei ++, i ++ ) {
            if ( bit_read(job, ei) ) {
                JSON_status( ei, 1);
            } else {
                JSON_status( ei, 0);
                continue;
            }
        }
    } while ( 0 );
    // }}

    // {{ 作业数据
    // }}
    sprintf(buff, "printf {\\\"id\\\":\\\"-1\\\",\\\"v\\\":\\\"-1\\\"}]}\\&ttl=30\\&MM_insert=form1 >> /tmp/rcm.json");
    system(buff);

    system("/usr/zeus/script/feedback.sh");
    return 0;
}

unsigned int error_history_begin(struct charge_job *job, unsigned int error_id, char *error_string)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[512]={0}, timestamp[128]={0};
    int ret;

    pthread_mutex_lock(&task->err_list_lck);
    if ( task->err_head != NULL ) {
        head = task->err_head;
        do {
            thiz = list_load(struct error_history, error_me, head);
            if ( thiz->error_id == error_id ) {
                goto out;
            }
            head = head->next;
        } while ( head != task->err_head );
    }

    thiz = (struct error_history*)malloc(sizeof(struct error_history));
    if ( thiz == NULL ) goto out;
    list_ini(thiz->error_me);
    if ( task->err_head == NULL ) {
        task->err_head = & thiz->error_me;
    } else {
        list_inserttail(task->err_head, &thiz->error_me);
    }
    task->err_nr ++;
    thiz->error_seqid = task->err_seq_id_next ++;
    thiz->error_id = error_id;
    strncpy(thiz->error_string, error_string, 63);
    strcpy(thiz->error_recover, "0000-00-00 00:00:00.000");

    log_printf(INF, "ZEUS: 故障总数为: %d", task->err_nr);

    __get_timestamp(timestamp);
    strcpy(thiz->error_begin, timestamp);
    sprintf(sql, "INSERT INTO errors VALUES('%d','%d','%s','%s','ERROR')",
            thiz->error_seqid,
            thiz->error_id,
            thiz->error_begin,
            thiz->error_recover);
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
out:
    pthread_mutex_unlock (&task->err_list_lck);

    return error_id;
}

void error_history_recover(struct charge_job *job, unsigned int error_id)
{
    struct error_history *thiz;
    struct list_head *head;
    char sql[512]={0}, errname[128]={0}, timestamp[128]={0};
    int ret;

    pthread_mutex_lock(&task->err_list_lck);

    if ( task->err_head == NULL ) goto out;

    head = task->err_head;
    do {
        thiz = list_load(struct error_history, error_me, head);
        if ( thiz->error_id == error_id ) {
            goto del;
        }
        head = head->next;
    } while ( head != task->err_head );
    goto out;
del:
    if ( task->err_head == & thiz->error_me ) {
        task->err_head = thiz->error_me.next;
    }
    list_remove(&thiz->error_me);
    task->err_nr --;
    if ( task->err_nr == 0 ) {
        task->err_head = NULL;
    }
    __get_timestamp(timestamp);
    sprintf(sql,
            "UPDATE errors SET "
            "error_recover='%s',recover_reason='NORMAL' "
            "WHERE error_id='%d' AND error_begin='%s'",
            timestamp, thiz->error_id,
            thiz->error_begin);
    ret = sqlite3_exec(task->database, sql, NULL, NULL, NULL);
    log_printf(INF, "ZEUS: %s:%d", sql, ret);
    free(thiz);
out:
    pthread_mutex_unlock (&task->err_list_lck);
}
