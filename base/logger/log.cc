#include "log.h"


Clogger::Clogger(){
	m_logger_file=Logger::getLogger("filetimeout");
	m_logger_console=Logger::getLogger("console");
	PropertyConfigurator::configureAndWatch("./log4cxx.properties");  
}


Clogger * Clogger::Instance_=NULL;

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
		do{ 								\
			 char buffer[1024]; 			\
			 memset(buffer,0,sizeof(buffer));\
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
	INFO("WUYUJIE %d",2);
	
	
   }
