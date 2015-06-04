#include "../../ontom/error.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../thirdpart/extso/exso.h"
#include "../../lib/lj/stdafx.h"
#include "../../lib/bms/father.h"
#include "../../ontom/charge.h"

#include "buzzer.h"

int exso_buzzer_init(void *p)
{
    printf("******  Plugin  buzzer  initialized  ******\n");

    return ERR_OK;
}

int exso_buzzer_exit(void *p)
{
    printf("******  Plugin  default  exited  ******\n");
    return ERR_OK;
}
