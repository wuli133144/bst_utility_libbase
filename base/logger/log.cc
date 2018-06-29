#include "log.h"


Clogger::Clogger(){
	
	
	//m_logger_console=Logger::getLogger("console");
	Init();
}


Clogger * Clogger::Instance_=NULL;

void Clogger::Init(const char *prg,const char *config)
{
         //config_=_cnf;
		 //program_name_=prg;
		 bzero(config_,BUFFERSIZE);
		 bzero(program_name_,BUFFERSIZE);
		 memcpy(config_,config,strlen(config));
		 memcpy(program_name_,prg,strlen(prg));
		 
		 PropertyConfigurator::configureAndWatch(config_);  
	     m_logger_file=Logger::getLogger(program_name_);
}


#define INFO(fmt,args...)                \
     do{                                 \
	 	  char buffer[1024];             \
		  memset(buffer,0,sizeof(buffer));\
	      sprintf(buffer,(fmt),##args);    \
	      LOG4CXX_INFO(Clogger::getInstance()->get(),buffer); \
	 }while(0)


	 
#define DEBUG(fmt,args...)                    \
		  do{								  \
			   char buffer[1024];			  \
			   memset(buffer,0,sizeof(buffer));\
			   sprintf(buffer,(fmt),##args);	\
			   LOG4CXX_DEBUG(Clogger::getInstance()->get(),buffer); \
		  }while(0)


#define ERROR(fmt,args...)                    \
		do{ 								  \
			 char buffer[1024]; 			  \
			 memset(buffer,0,sizeof(buffer)); \
			 sprintf(buffer,(fmt),##args);	  \
			 LOG4CXX_ERROR(Clogger::getInstance()->get(),buffer); \
		}while(0)

					
#define TRACE(fmt,args...)                    \
		do{ 								\
			 char buffer[1024]; 			\
			 memset(buffer,0,sizeof(buffer));\
			 sprintf(buffer,(fmt),##args);	  \
			 LOG4CXX_TRACE(Clogger::getInstance()->get(),buffer); \
		}while(0)



int main(){
   
    //Clogger *lloger=Clogger::getInstance();
	//INFO("WUYUJIE");

	Clogger *loger=Clogger::getInstance();
    loger.Init("test1","log4cxx.properities");
	INFO("WUYUJIE %d",2);
	
	
   }
