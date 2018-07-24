
#ifndef        ___CLIENT_SOCKET_TOOLS_
#define        ___CLIENT_SOCKET_TOOLS_


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <err.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>
#include <alsa/asoundlib.h>
#include <signal.h>


typedef struct  __IP_USER_LOCAL{
  unsigned char ipaddr[100];
  unsigned int  port;
  
}socket_addr_t;


socket_addr_t g_sockaddr_info;



void start_client_server(const char *ip,int port);


#endif 

