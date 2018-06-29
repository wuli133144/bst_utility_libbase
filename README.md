# bst_utility_libbase
boost‘s simple tools including logger,config ,thread,socket

##### intruduction

######## what's about it?

>first it's a simple 3rd library,including logger,config,socket,thread depending on boost ,
>now it's uncompleted ,so i will continue finish it,if you are intrested in it,please call me,
>i appreciate for what you do for it,now config function is finished,next step,i want to do logger,

######## log4cxx how to use 

```
<code>

     int main(){
   
    //Clogger *lloger=Clogger::getInstance();

	//INFO("WUYUJIE");
	INFO("WUYUJIE %d",2);
	
	
   }


</code>
```
>if you want to use it,first you have to  write a config file,like this:
```
#缺省不输出日志到控制台stdout
log4j.rootLogger=INFO,filetimeout
log4j.additivity.org.apache=false

#log4j.logger用于控制采集级别及采集内容
#Threshold用于控制输出级别

#应用于控制台
log4j.appender.stdout=org.apache.log4j.ConsoleAppender 
log4j.appender.stdout.layout=org.apache.log4j.PatternLayout 
log4j.appender.stdout.layout.ConversionPattern=%d{HH:mm:ss,SSS} [%-5p] - %m%n

#应用于基于文件时间的回滚
log4j.appender.filetimeout=org.apache.log4j.DailyRollingFileAppender
log4j.appender.filetimeout.File=./log/logtime.log
log4j.appender.filetimeout.Threshold=DEBUG
log4j.appender.filetimeout.ImmediateFlush=true
log4j.appender.filetimeout.Append=true
#每天滚动日志文件
log4j.appender.filetimeout.DatePattern='.'yyyy-MM-dd
log4j.appender.filetimeout.layout=org.apache.log4j.PatternLayout
log4j.appender.filetimeout.layout.ConversionPattern=%d [%-5p %.16c] - %m%n
```

filename must be called log4cxx.properities,otherwise trouble will happen,

