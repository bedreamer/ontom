#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include "mongoose.h"
#include "ajax.h"
#include "config.h"
#include "error.h"
#include "log.h"

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
                      "<p>The requested URL /version.xml was not found on this server.</p>\r\n"
                      "<p>Additionally, a 404 Not Found\r\n"
                      "error was encountered while trying to use an ErrorDocument to handle the request.</p>\r\n"
                      "</body></html>\r\n\r\n"
            );
        }
        result = MG_TRUE;
    } else if (ev == MG_AUTH) {
        result = MG_TRUE;
    }

  return result;
}

// xml 服务线程
// 提供xml文件输出，设置参数输入服务
static void *thread_xml_service(void *arg)
{
    int *done = (int *)arg;
    int mydone = 0;
    struct mg_server *server;
    if ( done == NULL ) done = &mydone;

    log_printf(INF, "%s running...", __FUNCTION__);
    server = mg_create_server(NULL, ev_handler);
    mg_set_option(server, "listening_port", "8081");

    printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
    for (; ! *done; ) {
        mg_poll_server(server, 1000);
    }
    mg_destroy_server(&server);

    log_printf(INF, "%s exit.", __FUNCTION__);
    return NULL;
}

// bms 服务线程
// 提供bms通信服务
static void *thread_bms_service(void *arg)
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;

    int s, ti = 0;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    int nbytes;

    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    log_printf(INF, "%s RX:TX = %X:%X",
               __FUNCTION__,
               addr.can_addr.tp.rx_id,
               addr.can_addr.tp.tx_id);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);
    while ( ! *done ) {
        frame.can_id  = 0x188 | CAN_EFF_FLAG;
        frame.can_dlc = 5;
        frame.data[0] = 0x11;
        frame.data[1] = 0x11;
        frame.data[2] = 0x11;
        frame.data[3] = 0x11;
        frame.data[4] = 0x11;
        frame.data[5] = 0x11;

        nbytes = write(s, &frame, sizeof(struct can_frame));

        log_printf(DBG, "TX%d---%X:%d %02X %02X %02X %02X %02X %02X %02X %02X",
                    ti++,
                    frame.can_id,
                    nbytes,
                    frame.data[0],
                    frame.data[1],
                    frame.data[2],
                    frame.data[3],
                    frame.data[4],
                    frame.data[5],
                    frame.data[6],
                    frame.data[7]
                );
        usleep(90000);
    }
}

// bms 服务线程
// 提供bms通信服务
static void *thread_bms1_service(void *arg)
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;

    int s, ti = 0;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    int nbytes;

    strcpy(ifr.ifr_name, "can0" );
    ioctl(s, SIOCGIFINDEX, &ifr);

    addr.can_family = PF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    log_printf(INF, "%s RX:TX = %X:%X",
               __FUNCTION__,
               addr.can_addr.tp.rx_id,
               addr.can_addr.tp.tx_id);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));

    log_printf(INF, "%s running...s=%d", __FUNCTION__, s);
    while ( ! *done ) {
        frame.can_id  = 0x133 | CAN_EFF_FLAG;
        frame.can_dlc = 2;
        frame.data[0] = 0x02;
        frame.data[1] = 0x02;

        nbytes = write(s, &frame, sizeof(struct can_frame));

        log_printf(DBG, "TX%d---%X:%d %02X %02X %02X %02X %02X %02X %02X %02X",
                    ti++,
                    frame.can_id,
                    nbytes,
                    frame.data[0],
                    frame.data[1],
                    frame.data[2],
                    frame.data[3],
                    frame.data[4],
                    frame.data[5],
                    frame.data[6],
                    frame.data[7]
                );
        usleep(90000);
    }
}

// 串口通信 服务线程
// 提供串口通信服务
static void *thread_uart_service(void *arg)
{
    int *done = (int *)arg;
    int mydone = 0;
    if ( done == NULL ) done = &mydone;
    log_printf(INF, "%s running...", __FUNCTION__);

    while ( ! *done ) {
        usleep(5000);
    }
}

// 套接字配置服务线程
// 提供套接字配置，查询服务
static void *thread_config_service(void *arg)
{
    return config_drive_service(arg);
}

int main()
{
    const char *user_cfg = NULL;
    pthread_t tid = 0;
    int thread_done[ 5 ] = {0};
    char buff[32];

    // 读取配置文件的顺序必须是
    // 1. 系统配置文件
    // 2. 用户参数配置文件
    // 需要注意的是，用户配置数据和用户配置数据可以有相同的配置项
    // 但优先级最高的是用户配置数据，如果某个配置项同时出现在系统配置
    // 和用户配置中，那么系统最终采用的值将是用户配置数据中的值
    // 因此这里需要注意的是：
    // * 有两个配置文件是一种冗余设计
    // * 非必要的情况下，分别将系统配置和用户配置分开存储到两个文件中
    config_initlize("ontom.cfg");
    user_cfg = config_read("user_config_file");
    if ( user_cfg == NULL ) {
        log_printf(WRN, "not gave user config file,"
                   "use 'user.cfg' by default.");
        user_cfg = "user.cfg";
    }
    config_initlize(user_cfg);

    // 检查是否需要开启SOCKET端的配置服务器以接受网络端的配置
    user_cfg = config_read("socket_config");
    if ( strcmp(user_cfg, "TURE") ||
         strcmp(user_cfg, "true") ) {
        pthread_create( & tid, NULL, thread_config_service, NULL);
        sprintf(buff, "%d", tid);
        config_write("thread_config_server_id", buff);
    }

    pthread_create( & tid, NULL, thread_xml_service, &thread_done[0]);
    sprintf(buff, "%d", tid);
    config_write("thread_xml_server_id", buff);
    pthread_create( & tid, NULL, thread_bms_service, &thread_done[1]);
    sprintf(buff, "%d", tid);
    config_write("thread_bms_server_id", buff);
    pthread_create( & tid, NULL, thread_bms1_service, &thread_done[2]);
    config_write("thread_bms1_server_id", buff);
    pthread_create( & tid, NULL, thread_uart_service, &thread_done[3]);
    sprintf(buff, "%d", tid);
    config_write("thread_uart_server_id", buff);

#if CONFIG_DEBUG_CONFIG >= 1
    config_print();
#endif
    for ( ;; ) {
        sleep(1);
    }

	return 0;
}
