#include <stdio.h>
#include "../../ontom/error.h"

int nr = 0;

int exso_demo_init(void *p)
{
    printf("******  Plugin  demo  initialized  ******");
    return ERR_OK;
}

int exso_demo_main_loop(void *p)
{
    nr ++;
    if ( 0 == (nr % 1000) )
    printf("*****   Plugin demo say: Hello %d", nr);
    return ERR_OK;
}

int exso_demo_exit(void *p)
{
    printf("******  Plugin  demo  exited  ******");
    return ERR_OK;
}
