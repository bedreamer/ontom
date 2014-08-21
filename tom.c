#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mongoose.h"

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  int result = MG_FALSE;

  if (ev == MG_REQUEST) {
	if ( strstr(conn->uri, "version.xml") != 0 ) {
		//printf("will send : version.xml\n");
		//mg_send_file(conn, "version.xml");
		mg_printf(conn, "HTTP/1.1 200 HTTP\r\n"
		"Server: thttpd/2.21b PHP/20030920\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Type: text/xml\r\n"
		"Date: Wed, 20 Aug 2014 03:29:12 GMT\r\n"
		"Last-Modified: Tue, 19 Aug 2014 09:23:50 GMT\r\n"
		"Accept-Ranges: bytes\r\n"
		"Content-Length: 192\r\n"
		"Connection: keep-alive\r\n"
		"\r\n"
		"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n"
		"<VERSION>"
		"<THTTPD>V1.01 build-20140820.r124</THTTPD>"
		"<BROWSER>V1.20 build-20140809.r3487</BROWSER>"
		"<TOM>V2.03.4</TOM>"
		"<GOD>V1.0.0</GOD>\r\n"
		"</VERSION>\r\n"
		"\r\n"
		);
		printf("send done %d\n", strlen("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n"
		"<VERSION>"
		"<THTTPD>V1.01 build-20140820.r124</THTTPD>"
		"<BROWSER>V1.20 build-20140809.r3487</BROWSER>"
		"<TOM>V2.03.4</TOM>"
		"<GOD>V1.0.0</GOD>\r\n"
		"</VERSION>\r\n"
		"\r\n"));
	} else {
		printf("request uri: %s", conn->uri);
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

	server = mg_create_server(NULL, ev_handler);
	mg_set_option(server, "listening_port", "8081");

	printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
	for (;;) {
		mg_poll_server(server, 1000);
	}
	mg_destroy_server(&server);

	return 0;
}
