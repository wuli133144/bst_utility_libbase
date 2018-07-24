#include "client_socket.h"
#include "config.h"


extern socket_addr_t g_sockaddr_info;


int main()
{

 signal(SIGPIPE,SIG_IGN);
 load_config("config.lua");
 char ip[100];
 int port;
 memset(ip,0,sizeof(ip)/sizeof(char));
 get_Value_string("ip1",ip);
 get_Value_int("port",&port);
 close_config();


 //signal(SIGINT,SIG_IGN);
 start_client_server(ip,port);
 
 return 0;
}
