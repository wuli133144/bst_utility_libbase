#ifndef  JW___TOOLS_SOCKET_H
#define  JW___TOOLS_SOCKET_H



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


#define  LISTEN_PORT          30000

#define  TCP_SNDRCV_BUFSIZE   1024*100

#define   SENDBUF_SIZE         1024

void start_socket_server();




#endif 

