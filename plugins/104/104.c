#include "../../ontom/error.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../thirdpart/extso/exso.h"
#include "../../lib/lj/stdafx.h"
#include "../../lib/bms/father.h"
#include "../../ontom/charge.h"

#include "104.h"

void *fuck_p104_service(void *arg);

int exso_p104_init(void *p)
{
    log_printf(INF, "******  Plugin  p104  initialized  ******");
    return ERR_OK;
}

int exso_p104_exit(void *p)
{
    printf("******  Plugin  default  exited  ******\n");
    return ERR_OK;
}

/*套接字配置服务程序*/
void *fuck_p104_service(void *arg)
{
    int ok = 1;
    int srv_port;
    //    2404 TCP iec-104 IEC 60870-5-104 process control over IP
    //    2404 UDP iec-104 IEC 60870-5-104 process control over IP
    const char *pport = "2404";
    int s_srv = -1, s_cli = -1;
    struct sockaddr_in si_me, si_other;
    int slen;

    srv_port = atoi(pport);
    if ( srv_port <= 2048 ) {
        log_printf(ERR, "socket config service port %d invalid. abort!",
                   srv_port);
        goto thread_die;
    }

    s_srv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( -1 == s_srv ) {
        log_printf(ERR, "create socket confige server faile!");
        goto thread_die;
    }

    fcntl(s_srv, F_SETFL, FASYNC);
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(srv_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s_srv , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
        log_printf(ERR, "bind to port %d faile, abort.", srv_port);
        goto thread_die;
    }

    if ( -1 == listen( s_srv, 1) ) {
        log_printf(ERR, "listen to port %d faile, abort.", srv_port);
        goto thread_die;
    }

    while ( ok ) {
        usleep(5000);
        slen = sizeof(si_other);
        s_cli = accept( s_srv, (struct sockaddr *)&si_other, &slen);
        if ( s_cli == -1 ) {
            log_printf(WRN, "P104: invalid socket cnnnection.");
            continue;
        }
   }

thread_die:
    if ( s_srv != -1  ) {
        close(s_srv);
    }
    return NULL;
}
