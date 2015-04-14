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
    int need_autheticate = 0, good_request = 0;
    struct sockaddr_in si_me, si_other;
    struct socket_config_request request;
    struct socket_config_ack ack;
    const char * autheticate, *username, *passwd, *value;
    struct config_struct *thiz;
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

        request.config_username[ 7 ] = '\0';
        request.config_passwd[15] = '0';
        request.config_item_name[31] = '\0';
        request.config_item_value[127] = '\0';

        need_autheticate = 0;
        autheticate = config_read("socket_config_autheticate");
        if ( 0 == strcmp( autheticate, "TRUE") || 0 == strcmp( autheticate, "true") ) {
            need_autheticate = 1;
            username = config_read("socket_config_username");
            passwd = config_read("socket_config_passwd");
        }
        if ( need_autheticate ) {
            if ( strcmp(username, request.config_username) == 0 &&
                 strcmp(passwd, request.config_passwd ) == 0 ) {
                good_request = 1;
            } else {
                ack.config_result = 300;
                good_request = 0;
                log_printf(INF, "autheticate faile, user: %s, passwd: %s",
                           request.config_username, request.config_passwd);
                goto bad_request;
            }
        }

        if ( request.config_cmd != CONFIG_RD &&
             request.config_cmd != CONFIG_WR ) {
            ack.config_result = 505;
            good_request = 0;
            goto bad_request;
        }

        thiz = NULL;
        // 读配置数据可以使用索引或是数据名，数据名优先
        if ( request.config_item_name[0] != '\0' ) {
            thiz = config_search(request.config_item_name);
        }

        if ( request.config_cmd == CONFIG_RD ) {
            if ( thiz == NULL ) {
                if ( request.config_item_index >=
                     sizeof(configs)/sizeof(struct config_struct)) {
                    ack.config_result = 404;
                    good_request = 0;
                    goto bad_request;
                } else {
                    thiz = &configs[ request.config_item_index ];
                }
            }
            if ( thiz->config_name == NULL ) {
                ack.config_result = 404;
                good_request = 0;
                log_printf(DBG, "read the last config item.");
                goto bad_request;
            }
            ack.config_cmd = CONFIG_RD;
            ack.config_result = 0;
            strncpy(ack.config_name, thiz->config_name, 31);
            ack.config_name[31] = '\0';
            ack.config_status = thiz->config_status;
            ack.config_type = thiz->config_type;
            ack.config_user = thiz->config_user;
            strncpy(ack.config_value, thiz->config_value, 127);
            ack.config_value[127] = '\0';
        }
        if ( request.config_cmd == CONFIG_WR ) {
            if ( thiz == NULL ) {
                ack.config_result = 404;
                log_printf(DBG, "not found %s to write",
                           request.config_item_name);
                good_request = 0;
                goto bad_request;
            }
            if ( thiz->config_type != request.config_item_type ) {
                ack.config_result = 404;
                log_printf(DBG, "type conflict.");
                good_request = 0;
                goto bad_request;
            }
            request.config_item_value[ 127 ] = '\0';
            request.config_item_name[ 31 ] = '\0';
            value =
             config_write(request.config_item_name, request.config_item_value);
            if ( NULL == value ) {
                ack.config_result = 303;
                good_request = 0;
                log_printf(DBG, "write value faile.");
                goto bad_request;
            }
            ack.config_cmd = CONFIG_WR;
            ack.config_result = 0;
            strncpy(ack.config_name, thiz->config_name, 31);
            ack.config_name[31] = '\0';
            ack.config_status = thiz->config_status;
            ack.config_type = thiz->config_type;
            ack.config_user = thiz->config_user;
            strncpy(ack.config_value, thiz->config_value, 127);
            ack.config_value[127] = '\0';
        }
        recv_len = sendto(s_config, &ack, sizeof(struct socket_config_ack),
                          0, (struct sockaddr*) &si_other, slen);
        //log_printf(DBG, "send %d bytes", recv_len);
        continue;
bad_request:
        log_printf(DBG, "bad request.user: %s, passwd: %s, name:%s, give: %s",
                   request.config_username, request.config_passwd,
                   request.config_item_name, request.config_item_value);
        if ( good_request  <= 0 ) {
            //now reply to server socket/the client with the same data
            ack.config_cmd = request.config_cmd;
            if ( ack.config_result == 0 ) {
                ack.config_result = 500;
            }
            recv_len = sendto(s_config, &ack, sizeof(struct socket_config_ack),
                              0, (struct sockaddr*) &si_other, slen);
            continue;
        }
    }

thread_die:
    if ( s_config != -1  ) {
        close(s_config);
    }
    return NULL;
}
