#!/bash/bin

#wuyujie

if [ $UID -ne  0 ];then
   echo "runer has no permission"
   sleep 1
   exit 1
if 

#before install log4cxx,you have to prepare enough resource about it,
#example apr,apr-util,apache-log4cxx,
#in this program ,i have got it,so you just unpack it,
#then follow tips step by step install


#tar -xvf apr-1.6.3.tar.gz 
#install apr

APR_BASE="/usr/local/apr"


function install_apr(){
   tar -xvf apr-1.6.3.tar.gz
   cd apr-1.6.3
   if [ ! -d ${APR_BASE} ];then 
       mkdir ${APR_BASE}
   fi 
   ./configure --prefix=${APR_BASE}
   make 
   make install 
}



function install_apr_util(){
   tar -xvf apr-util-1.6.1.tar.gz
   cd apr-util-1.6.1
   if [ ! -d ${APR_BASE} ];then 
       mkdir ${APR_BASE}
   fi 
   ./configure --prefix=${APR_BASE}
   make 
   make install 
}


APR_LOG4CXX="/usr/local/log4cxx"
function install_log4cxx(){
   tar -xvf apache-log4cxx-0.10.0.tar.gz
   cd apache-log4cxx-0.10.0
   if [ ! -d ${APR_LOG4CXX} ];then 
       mkdir ${APR_LOG4CXX}
   fi 
   ./configure --prefix=${APR_LOG4CXX} --with-apr=${APR_BASE} --with-apr-util=${APR_BASE} --with-charset=utf-8
   
   make 
   #if make then error happened,do't worry ,
   #just follow error tips,fix it,then make continue,
   #success 
   make install 
}


case $1 in 
   "all")
    install_apr
	install_apr_util
	install_log4cxx
   ;;
   "apr")
   install_apr
   ;;
   "apr-util")
   install_apr_util
   ;;
   "log4cxx")
   install_log4cxx
   ;;
   *)
   ;;
esac 
    




