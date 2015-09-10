#include "stdafx.h"

int pandora_data_dorx(struct session_struct *ss);
int pandora_data_dotx(struct session_struct *ss);

// 潘多拉协议服务程序
// 协议转发，数据 <---> zeus.
void *pandora_main_proc(void *socket_desc)
{
    //Get the socket descriptor
    struct session_struct *ss = (struct session_struct *)socket_desc;
    int ret_value, rd, wr;
    fd_set rfds, wrds;
    struct timeval  tv ;
    struct pack_tp_struct rx, tx;

    while ( 1  ) {
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        FD_ZERO(&rfds);
        FD_ZERO(&wrds);

        ret_value = select(ss->socket_handle+1, &rfds, &wrds, NULL, &tv);
        if ( -1 == ret_value ) {
        } else if ( 0 != ret_value ) {
            // 有数据可读
            if ( FD_ISSET(ss->socket_handle, &rfds) ) {
                rd = recv(ss->socket_handle, &rx, TP_BUFF_SIZE, 0);
                if ( rd == TP_BUFF_SIZE ) {
                    session_push_bytes(&ss->session_rx_buff, &rx, rd);
                } else {
                    log_printf(WRN, "PANDORA: recv error.");
                }
            }
            // 可以写数据
            if ( FD_ISSET(ss->socket_handle, &wrds) ) {
                pandora_data_dotx(ss);
            }
        } else { // 超时
            continue;
        }
    }

    return NULL;
}

void * pandora_listen_proc(void *p)
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
    struct pandoras_box *sp = p;
    size_t session_count = 1;

reinit:
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( sp->zeus_port );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return NULL;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc,
                                 (struct sockaddr *)&client, (socklen_t*)&c)) ) {
        pthread_t sniffer_thread;
        struct session_struct *ss =
                (struct session_struct*)malloc(sizeof(struct session_struct));
        if ( ss == NULL ) {
        } else {
            memset(ss, 0, sizeof(struct session_struct));
            ss->connected_tstp = time(NULL);
            ss->socket_handle = client_sock;
            ss->stype = SESSION_UNDEFINED;
            ss->session_id = session_count ++;
            list_ini(ss->nd);
            pthread_mutex_init(&ss->session_rx_buff.buff_rw_lck, NULL);
            pthread_mutex_init(&ss->session_rx_buff.buff_rw_lck, NULL);

            if( pthread_create( &sniffer_thread , NULL ,
                                pandora_main_proc , ss) < 0) {
                perror("could not create thread");
                continue;
            }
        }
    }

    if (client_sock < 0) {
        perror("accept failed");
        close(socket_desc);
        goto reinit;
    }

    return NULL;
}

int pandora_data_dorx(struct session_struct *ss)
{
    return 0;
}

int pandora_data_dotx(struct session_struct *ss)
{
    return 0;
}

// 向缓冲区中填入数据
int session_push_bytes(struct session_buff *sb, const char *ibuf, int bytes)
{
    int pushed_bytes = 0;

    pthread_mutex_lock(&sb->buff_rw_lck);
    if ( sb->buff_size + bytes > BUFF_SIZE || bytes <= 0 ) {
        // 避免缓冲区溢出
        goto out;
    }

    for ( pushed_bytes = 0; pushed_bytes < bytes; pushed_bytes ++ ) {
        sb->buff[position_index(sb->io_cursor)] = ibuf[ pushed_bytes ];
        sb->io_cursor ++;
        sb->buff_size ++;
    }
out:
    pthread_mutex_unlock(&sb->buff_rw_lck);
    return pushed_bytes;
}

// 从缓冲区中取出前bytes数据
int session_pull_bytes(struct session_buff *sb, char *obuf, int bytes)
{
    int pulled_bytes = 0;
    unsigned short cursor;

    pthread_mutex_lock(&sb->buff_rw_lck);
    if ( sb->buff_size < bytes ) {
        // 避免缓冲区溢出
        goto out;
    }

    cursor = position_index(sb->io_cursor);
    for ( pulled_bytes = 0; pulled_bytes < bytes; pulled_bytes ++ ) {
        obuf[ pulled_bytes ] = sb->buff[position_index(cursor)];
        cursor ++;
    }
out:
    pthread_mutex_unlock(&sb->buff_rw_lck);
    return pulled_bytes;
}

// 从缓冲区中弹出bytes数据
int session_pop_bytes(struct session_buff *sb, char *obuf, int bytes)
{
    int poped_bytes = 0;

    pthread_mutex_lock(&sb->buff_rw_lck);
    if ( sb->buff_size < bytes ) {
        // 避免缓冲区溢出
        goto out;
    }

    for ( poped_bytes = 0; poped_bytes < bytes; poped_bytes ++ ) {
        obuf[ poped_bytes ] = sb->buff[position_index(sb->io_cursor)];
        sb->io_cursor ++;
        sb->buff_size --;
    }
out:
    pthread_mutex_unlock(&sb->buff_rw_lck);
    return poped_bytes;
}
