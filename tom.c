#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
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
            mg_printf(conn, "HTTP/1.1 404 HTTP\r\n"
                "Server: thttpd/2.21b PHP/20030920\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "\r\n"
            );
        }
        result = MG_TRUE;
    } else if (ev == MG_AUTH) {
        result = MG_TRUE;
    }

  return result;
}

int main()
{
	struct mg_server *server;

    config_initlize("cfg.fg");
    server = mg_create_server(NULL, ev_handler);
	mg_set_option(server, "listening_port", "8081");

	printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
	for (;;) {
		mg_poll_server(server, 1000);
	}
	mg_destroy_server(&server);

	return 0;
}
