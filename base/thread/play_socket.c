#include "play_socket.h"
#include "jw_pthread.h"
#include "msgdefine.h"

int m_listen_socketfd;
int m_client_socketfd;

char *recv_buffer_=NULL;
int recved_size=0;


void tcp_recv()
{
      

}



void para_command_msg(struct sockaddr_in  *clientsock){

         CC_MsgHeader *pheader=(CC_MsgHeader *)recv_buffer_;
		 printf("para_command_msg failed");
		 if(pheader->messageHeader[3]='C'){
		 	
		 }else if(pheader->messageHeader[3]='D'){
		 	
		 }
		 

      
}

//single-thread
void  child_active( struct sockaddr_in  *clientsock)
{
    
    fd_set read_sets,write_sets;
    FD_ZERO(&read_sets);
	FD_ZERO(&write_sets);
	
	FD_SET(m_client_socketfd,&read_sets);
	FD_SET(m_client_socketfd,&write_sets);
    struct timeval tmval={20,0};//20seconds if overtime close socket;
	int nRet=0;
	recved_size=0;
	int recvsize=0;
	int msgheaderSize=sizeof(CC_MsgHeader);

	int wait_count=0;
	int except_mark=0;
	while((wait_count < 10 )&&(!except_mark)
	{
         nRet=select(m_client_socketfd+1,&read_sets,&write_sets,NULL,&tmval);
		 switch (nRet)
		 {
              case 0: //overtime
              	{
              	    wait_count++;
              	}
			  case -1{
			  	     //exit(-1);
			  	     except_mark=1;
				     break;
			    }
			  default:{

			          if(FD_ISSET(m_client_socketfd,&read_sets)){
					  	   //tcp_recv();
					  	   recvsize=recv(m_client_socketfd,recv_buffer_+recved_size,msgheaderSize,0);
                           if(recvsize < 0){
						   	  except_mark=1;
							  break;
						   }else if(recvsize ==0&&errno==EWOULDBLOCK){
						   	  break;
						   }else{
						       if(recvsize >=sizeof(CC_MsgHeader)){
							   	 break;
							   }
							   
						       msgheaderSize-=recvsize;
						       recved_size+=recvsize;
							   break;
						   }
						   
					  }else {
					    wait_count++;
						break;
					  }
					  //break;
			  
			  }
			  

		 }

          if(recved_size > = sizeof(CC_MsgHeader)){
		  	  break;
		  }
		  
	             
       }
	    if(except_mark){
			 if(m_client_socketfd){
			 	 close(m_client_socketfd);
				 m_client_socketfd=0;
			 }
			 return ;
		}
		
        if(recved_size > = sizeof(CC_MsgHeader)){
		 	 //parm_command_line
		 	  para_command_msg(clientsock); 	 
		}else{
		
		     if(m_client_socketfd){
			 	 close(m_client_socketfd);
				 m_client_socketfd=0;
			 }
		}
       
}


void child_thread_progress(){
	    //pthread_t pid;
        //create_pthread_detach(&pid,child_active,NULL);
}

void accept_progress()
{     
      struct sockaddr_in clientsock;
	  socklen_t len=sizeof(struct sockaddr_in);
	  recv_buffer_=(char *)malloc(SENDBUF_SIZE);
	  
      while(1){
        m_client_socketfd=accept(m_listen_socketfd,(struct sockaddr*)&clientsock,&len);
		if(m_client_socketfd < 0){
			continue;
		}
		child_active(&clientsock);

		
	  }
 
}

void start_socket_server()
{

     printf("start_socket_server start......");
     struct sockaddr_in clientsock;
	 int                opt,nRet;
	 int len            =sizeof(opt);
	 m_listen_socketfd=socket(AF_INET,SOCK_STREAM,0);
	 if(m_listen_socketfd < 0){
	 	goto ERROR;
	 }

	 nRet=setsockopt(m_listen_socketfd,SOL_SOCKET,SO_REUSEADDR,&opt,len);
	 if(nRet < 0){
	 	 goto ERROR;
	 }
	 fcntl(m_listen_socketfd,F_SETFL,O_NONBLOCK);
	 memset(&clientsock,0,sizeof(clientsock));

	 clientsock.sin_family=AF_INET;
	 clientsock.sin_port=htons(LISTEN_PORT);
	 clientsock.sin_addr.s_addr=htonl(INADDR_ANY);
	 
	 nRet=bind(m_listen_socketfd,(struct sockaddr *)&clientsock,sizeof(clientsock));
	 if(nRet < 0 ){
	 	 goto ERROR;
	 }

	 nRet= listen(m_listen_socketfd,100);
	 if(nRet <0){
	 	 goto ERROR;
	 }

	 accept_progress();

	 
	 
         
ERROR:
   close(m_listen_socketfd);

}
