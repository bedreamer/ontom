#include <stdio.h>
#include "../../ontom/error.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../thirdpart/extso/exso.h"
#include "../../lib/lj/stdafx.h"
#include "../../lib/bms/father.h"
#include "../../ontom/charge.h"

struct charge_task *zeus;

static int automatic_load_plugins(struct charge_task *t, const char *exso_name)
{
    char exso_path[256];
    struct exso_struct *thiz;
    const char *plugins_path = config_read("exso_path");

    sprintf(exso_path, "%sexso_%s.so", plugins_path, exso_name);
    thiz = exso_load( &(t->exsos), exso_name, exso_path, t);
    if ( thiz == NULL ) {
        log_printf(WRN, "EXSO: load <%s:%s> faile!!!!", exso_name, exso_path);
        return ERR_ERR;
    } else {
        log_printf(INF, "EXSO: <%s:%s> loaded.", exso_name, exso_path);
        return ERR_OK;
    }
}

int sql_db_settings_result(void *param, int nr, char **text, char **name)
{
    if ( nr <= 0 ) return 0;
    automatic_load_plugins(zeus, text[0]);
    return 0;
}

int exso_default_init(void *p)
{
    char sql[512];
    printf("******  Plugin  default  initialized  ******\n");
    const char *exso_path = config_read("exso_path");
    if ( exso_path == NULL ) {
        exso_path = config_write("exso_path", "/usr/zeus/plugins/");
        if ( exso_path == NULL ) return ERR_ERR;
    }

    zeus = (struct charge_task *)p;
    sprintf(sql, "SELECT name,so_name FROM plugins WHERE disabled='FALSE'");

    return ERR_OK;
}

int exso_default_main_loop(void *p)
{
    int ret = ERR_OK;
    struct charge_task *t = (struct charge_task *)p;

    if ( t == NULL ) return ERR_ERR;
    const char *load_exso_name = config_read("load_exso");
    const char *unload_exso_name = config_read("unload_exso");

    if ( load_exso_name != NULL && 0 != strcmp("N/A", load_exso_name) ) {
        ret = automatic_load_plugins(t, load_exso_name);
        config_write("load_exso", "N/A");
        config_write("load_name", "N/A");
    } else if ( unload_exso_name != NULL && 0 != strcmp("N/A", unload_exso_name)  ) {
    } else {}

    return ret;
}

int exso_default_exit(void *p)
{
    printf("******  Plugin  default  exited  ******\n");
    return ERR_OK;
}
