#include "stdafx.h"

extern void * pandora_listen_proc(void *);

struct pandoras_box pb;
int main(int argc, const char *argv[])
{
    pthread_t sniffer_thread;

    pb.zeus_port = 8082;

    pthread_create( &sniffer_thread , NULL ,
                                pandora_listen_proc , &pb);

    while (1) {
        sleep(1);
    }
	return 0;
}
