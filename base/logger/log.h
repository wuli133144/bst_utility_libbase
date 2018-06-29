



#ifndef LOG4CXX_H
#define LOG4CXX_H


#include <log4cxx/logger.h>  
#include <log4cxx/basicconfigurator.h>  
#include <log4cxx/propertyconfigurator.h>  
#include <log4cxx/helpers/exception.h>  
#include <iostream> 
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>


using namespace log4cxx;
using namespace log4cxx::helpers;



class Clogger{
public:

      static Clogger *getInstance(){
	  	 if(!Instance_){
		 	Instance_=new Clogger;
		 }
		 return Instance_;
	  }
      LoggerPtr get(){return m_logger_file;}
	  void Init(const char *prg="test",const char *config="log4cxx.properities");
	  

	  LoggerPtr m_logger_file;
public:
	 LoggerPtr m_logger_console;
	// string config_;
	// string program_name_;
	 enum{BUFFERSIZE=1024};
	 char config_[BUFFERSIZE];
	 char program_name_[BUFFERSIZE];

     Clogger();
	 
     static Clogger * Instance_;

};





#endif




