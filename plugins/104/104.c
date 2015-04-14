#include "../../ontom/error.h"
#include "../../thirdpart/sqlite/sqlite3.h"
#include "../../thirdpart/extso/exso.h"
#include "../../lib/lj/stdafx.h"
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
    int s_config = -1;
    struct sockaddr_in si_me, si_other;
    struct socket_config_request request;
    struct socket_config_ack ack;
    int recv_len, slen;

    srv_port = atoi(pport);
    if ( srv_port <= 2048 ) {
        log_printf(ERR, "socket config service port %d invalid. abort!",
                   srv_port);
        goto thread_die;
    }

    s_config = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( -1 == s_config ) {
        log_printf(ERR, "create socket confige server faile!");
        goto thread_die;
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(srv_port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s_config , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1) {
        log_printf(ERR, "bind to port %d faile, abort.", srv_port);
        goto thread_die;
    }

    while ( ok ) {
        usleep(5000);
        slen = sizeof(si_other);
        memset(&ack, 0, sizeof(ack));
        recv_len = recvfrom(s_config, &request,
                 sizeof(struct socket_config_request),
                 0, (struct sockaddr *) &si_other, (socklen_t * __restrict__)&slen);
        if ( recv_len <= 0 ) continue;
   }

thread_die:
    if ( s_config != -1  ) {
        close(s_config);
    }
    return NULL;
}
