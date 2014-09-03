#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>  /* system data type definitions */
#include <sys/socket.h> /* socket specific definitions */
#include <netinet/in.h> /* INET constants and stuff */
#include <arpa/inet.h>  /* IP address conversion stuff */
#include "config.h"

int noecho  = 0;

void printf_ack(struct socket_config_ack *head)
{
    int i, j, k;
    static char *value_type[] = {"STRING", "INT", "FLOAT", "BOOL", "N/A"};
    static char *value_system[] = {"NO", "YES", "N/A"};
    static char *value_status[] = {"INVALID", "VALID", "MODIFY", "N/A"};
    if ( noecho ) return;
    if ( head == NULL ) return;
    if ( head->config_result != 0 ) {
        printf("request faile, error code: %d\n", head->config_result);
        return;
    }

    i = (head->config_type >=0 && head->config_type <= 3)? head->config_type : 4;
    j = (head->config_user == 0 || head->config_user == 1) ? head->config_user : 2;
    k = (head->config_status >=0 && head->config_status <= 2) ? head->config_status : 3;
    printf("%-32s  %-6s  %-6s  %-8s  %s\n",
           head->config_name,
           value_type[ i ],
           value_system[ j ],
           value_status[ k ],
           head->config_value);
}

/*
 * onconfig  {-r name} {-w name -t [string|int|float|bool] -d newvalue} {-H IP} {-h}
*/
int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr;

#define CONFIG_PRINT   0xFF
#define CONFIG_HELP    0xFE

   unsigned int cmd = CONFIG_HELP;
   char name[32] = {0}, value[CONFIG_MAX_VALUE] = {0};
   char username[8] = {0}, passwd[16] = {0};
   char ip[16] = {"127.0.0.1"};
   int port = 9990;
   char type = -1;
   struct socket_config_request requst;
   struct socket_config_ack ack;

   int i, index = 0;

   for ( i = 1; i < argc; i ++ ) {
       if ( 0 == strcmp ("-r", argv[i]) ||
            0 == strcmp ("--read", argv[i]) ) {
           cmd = CONFIG_RD;
           if ( i + 1 >= argc ) {
               fprintf(stderr, "name need a paramater.\n");
               exit(0);
           } else {
               strncpy(name, argv[ i + 1], 31);
               name[ 31 ] = '\0';
               i ++;
               continue;
           }
           continue;
       }
       if ( 0 == strcmp ("-w", argv[i]) ||
            0 == strcmp ("--write", argv[i]) ) {
           cmd = CONFIG_WR;
           if ( i + 1 >= argc ) {
               fprintf(stderr, "name need a paramater.\n");
               exit(0);
           } else {
               strncpy(name, argv[ i + 1], 31);
               name[ 31 ] = '\0';
               i ++;
               continue;
           }
           continue;
       }
       if ( 0 == strcmp ("-v", argv[i]) ||
            0 == strcmp ("--value", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "value need a paramater.\n");
               exit(0);
           } else {
               strncpy(value, argv[ i + 1], 127);
               value[ 127 ] = '\0';
               i ++;
               continue;
           }
       }
       if ( 0 == strcmp ("-t", argv[i]) ||
            0 == strcmp ("--type", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "type need a paramater.\n");
               exit(0);
           } else {
               if ( 0 == strcmp("string", argv[i + 1]) ) {
                   type = C_STRING;
               }
               else if ( 0 == strcmp("int", argv[i + 1]) ) {
                   type = C_INT;
               }
               else if ( 0 == strcmp("float", argv[i + 1]) ) {
                   type = C_FLOAT;
               }
               else if ( 0 == strcmp("bool", argv[i + 1]) ) {
                   type = C_BOOL;
               } else {
                   fprintf(stderr, "invalid type %s\n", argv[i + 1]);
                   exit(0);
               }
               i ++;
           }
       }
       if ( 0 == strcmp ("-u", argv[i]) ||
            0 == strcmp ("--user", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "user need a paramater.\n");
               exit(0);
           } else {
               strncpy(username, argv[ i + 1], 8);
               username[ 7 ] = '\0';
               i ++;
               continue;
           }
       }
       if ( 0 == strcmp ("-p", argv[i]) ||
            0 == strcmp ("--passwd", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "passwd need a paramater.\n");
               exit(0);
           } else {
               strncpy(passwd, argv[ i + 1], 15);
               passwd[ 15 ] = '\0';
               i ++;
               continue;
           }
       }
       if ( 0 == strcmp ("-H", argv[i]) ||
            0 == strcmp ("--host", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "host need a paramater.\n");
               exit(0);
           } else {
               strncpy(ip, argv[ i + 1], 15);
               ip[ 15 ] = '\0';
               i ++;
               continue;
           }
       }
       if ( 0 == strcmp ("-P", argv[i]) ||
            0 == strcmp ("--port", argv[i]) ) {
           if ( i + 1 >= argc ) {
               fprintf(stderr, "port need a paramater.\n");
               exit(0);
           } else {
               port = atoi(argv[ i + 1 ]);
               if ( port <= 2048 ) {
                   fprintf(stderr, "port %d is invalid.", port);
                   exit(0);
               }
               i ++;
               continue;
           }
       }
       if ( 0 == strcmp ("--noecho", argv[i]) ) {
           noecho = 1;
           continue;
       }
       if ( 0 == strcmp ("--print", argv[i]) ) {
            cmd = CONFIG_PRINT;
            continue;
       }
       if ( 0 == strcmp ("--help", argv[i]) ) {
           cmd = CONFIG_HELP;
           continue;
       }
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=inet_addr(ip);
   servaddr.sin_port=htons(port);

   switch ( cmd ) {
   case CONFIG_RD:
       // need name, username, passwd
       requst.config_cmd = cmd;
       requst.config_item_index = 0;
       requst.config_item_type = 0;
       strncpy(requst.config_item_name, name, 31);
       requst.config_item_name[31] = '\0';
       strncpy(requst.config_username, username, 7);
       requst.config_username[7] = '\0';
       strncpy(requst.config_passwd, passwd, 15);
       requst.config_passwd[15] = '\0';
       sendto(sockfd, &requst, sizeof(requst),0,
              (struct sockaddr *)&servaddr,sizeof(servaddr));
       n=recvfrom(sockfd, &ack, sizeof(ack), 0, NULL, NULL);
       if ( n == sizeof(ack) )
            printf_ack( &ack );
       else fprintf(stderr, "wrong socket, check server config.\n");
       break;
   case CONFIG_WR:
       // need name, username, passwd
       requst.config_cmd = cmd;
       requst.config_item_index = 0;
       requst.config_item_type = type;

       strncpy(requst.config_item_name, name, 31);
       requst.config_item_name[31] = '\0';

       strncpy(requst.config_item_value, value, 127);
       requst.config_item_value[127] = '\0';

       strncpy(requst.config_username, username, 7);
       requst.config_username[7] = '\0';
       strncpy(requst.config_passwd, passwd, 15);
       requst.config_passwd[15] = '\0';
       //printf("config_item_name: %s:%s\n",requst.config_item_name, name);
       sendto(sockfd, &requst, sizeof(requst),0,
              (struct sockaddr *)&servaddr,sizeof(servaddr));
       n=recvfrom(sockfd, &ack, sizeof(ack), 0, NULL, NULL);
       if ( n == sizeof(ack) )
            printf_ack( &ack );
       else fprintf(stderr, "wrong socket, check server config.\n");
       break;
   case CONFIG_PRINT:
       if ( noecho ) noecho  = 0;
       while ( 1 )
       {
            memset( & requst, 0 , sizeof(requst));
            requst.config_cmd = CONFIG_RD;
            requst.config_item_index = index ++;
            strncpy(requst.config_username, username, 7);
            requst.config_username[7] = '\0';
            strncpy(requst.config_passwd, passwd, 15);
            requst.config_passwd[15] = '\0';
            //printf("request: %d\n", requst.config_item_index);
            sendto(sockfd, &requst, sizeof(requst),0,
                  (struct sockaddr *)&servaddr, sizeof(servaddr));
            n=recvfrom(sockfd, &ack, sizeof(ack), 0, NULL, NULL);

            //printf("ack: %d\n", ack.config_result);

            if ( n == sizeof(ack) )
                if ( ack.config_result != 0 ) break;
                else printf_ack( &ack );
            else {
                fprintf(stderr, "wrong socket, check server config.\n");
                break;
            }
       }
       break;
   case CONFIG_HELP:
       fprintf(stdout, "USAGE:\n\tonconfig ");
       fprintf(stdout, "[COMMAND] {OPTIONS}\n");
       fprintf(stdout, "COMMAND:\n");
       //[-r|--read|-w|--write|--print|--help]
       fprintf(stdout, "\t-r, --read: read a value.\n");
       fprintf(stdout, "\t-w, --write: read a value.\n");
       fprintf(stdout, "\t--print: print all value.\n");
       fprintf(stdout, "\t--help: show this message.\n");
       fprintf(stdout, "OPTIONS:\n");
       fprintf(stdout, "\t-v, --value: new value.\n");
       fprintf(stdout, "\t-t, --type: value type could be {string|int|float|bool}.\n");
       fprintf(stdout, "\t-u, --user:  username\n");
       fprintf(stdout, "\t-p, --passwd: passwd.\n");
       fprintf(stdout, "\t-H, --HOST: host ip address.\n");
       fprintf(stdout, "\t-P, --port: host port.\n");
       fprintf(stdout, "\t--noecho: no result echo.\n");
       fprintf(stdout, "AUTHOR:\n\tLiJie <bedreamer@163.com> 2014-08-28\n");
       break;
   }

   close(sockfd);
   return 0;
}
