#include "client_socket.h"


void setNonblock(int fd){
         int flags=fcntl(fd,F_GETFL);
		 int mode=(flags)|=(O_NONBLOCK);
		 if(fcntl(fd,F_SETFL,mode) < 0){
		 	close(fd);
		 }
}


void start_client_server(const char *ip,int port)
{

         
   
}