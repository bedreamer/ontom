#include <stdio.h>
#include "../../ontom/error.h"

int nr = 0;

int exso_demo_init(void *p)
{
    printf("******  Plugin  default  initialized  ******");
    return ERR_OK;
}

int exso_demo_main_loop(void *p)
{
    nr ++;
    if ( 0 == (nr % 1000) )
    printf("*****   Plugin default say: Hello %d", nr);
    return ERR_OK;
}

int exso_demo_exit(void *p)
{
    printf("******  Plugin  default  exited  ******");
    return ERR_OK;
}
