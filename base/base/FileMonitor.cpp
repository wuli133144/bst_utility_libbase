#include "FileMonitor.h"

#define EVENT_NUM 12

const char *event_str[EVENT_NUM] = 
{
    "IN_ACCESS",
    "IN_MODIFY",
    "IN_ATTRIB",
    "IN_CLOSE_WRITE",
    "IN_CLOSE_NOWRITE",
    "IN_OPEN",
    "IN_MOVED_FROM",
    "IN_MOVED_TO",
    "IN_CREATE",
    "IN_DELETE",
    "IN_DELETE_SELF",
    "IN_MOVE_SELF"
};

CFileMonitor::CFileMonitor(){}
CFileMonitor::~CFileMonitor(){}
 
CFileMonitor::CFileMonitor(char* file_path,callback_t callback)
{
	m_file_path = file_path;
	m_callback = callback; 
}

void logLevelUpdate(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if(callback_data == NULL){
		ERROR("logLevelUpdate failed: empty params");
		return;
	}

	char *filePath = (char *)callback_data;
	
	CConfigFileReader config_file(filePath);
	char* confLevel = config_file.GetConfigName("LogLevel");
	if (!confLevel) {
		ERROR("missing confLevel, exit...");
		return ;
	}
	uint32_t new_level = atoi(confLevel);
	LogLevel::getInstance()->update(new_level);
	INFO("%s has been changed!!!, new_log_level=%d", filePath,new_level);	
}

void dataUpdate(void* callback_data,uint8_t msg, uint32_t handle, void* pParam)
{
	if(callback_data == NULL){
		ERROR("dataUpdate failed: empty params");
		return;
	}

	string strTemp((char *)callback_data);
	int pos = strTemp.find('~');
	string strFileName = strTemp.substr(0,pos);
	
	//DEBUG("%s %s",strTemp.c_str(),strFileName.c_str());
	
	//更新群聊敏感词词库
	if(strcmp(strFileName.c_str(),"sensitive_word.utf8") == 0)
	{
		ba::WordCutter::getInstance()->update(GROUP_WORD_SENSITIVE);
		INFO("The group_sensitive_word_dict_file has been changed!!!");		
	}
	
	//更新群昵称敏感词词库
	else if(strcmp(strFileName.c_str(),"sensitive_nickname.utf8") == 0)
	{
		ba::WordCutter::getInstance()->update(GROUP_NAME_SENSITIVE);
		INFO("The sensitive_name_dict_file has been changed!!!");
		
	}
	
	//更新单聊屏蔽词库
	else if(strcmp(strFileName.c_str(),"single_sensitive_word.utf8") == 0)
	{
		ba::WordCutter::getInstance()->update(SINGLE_WORD_SENSITIVE);
		INFO("The single_sensitive_word_dict_file has been changed!!!");		
	}	
}

/*
 * ******************************
 * MonitorFile中包含要监控的文件
 * 或目录的地址、执行的操作
 * ******************************
*/
void * fileMonitor(void *ptr)
{

	int fd,wd,i;
    int len, nread;
    char buf[BUFSIZ];
    struct inotify_event *event;

    CFileMonitor *pMonitorEvent = (CFileMonitor *)ptr;
	char *filePath = pMonitorEvent->GetFilePath();
	callback_t callback = pMonitorEvent->GetCallBack();
	
    fd = inotify_init();
    if( fd < 0 ){
        ERROR("inotify_init failed");
        return NULL;
    }

    DEBUG("monitoringFilePath=%s",filePath);

    wd = inotify_add_watch(fd, filePath, IN_ALL_EVENTS);
    if(wd < 0){
        ERROR("inotify_add_watch %s failed", filePath);
        return NULL;
    }

    buf[sizeof(buf) - 1] = 0;
    while( (len = read(fd, buf, sizeof(buf) - 1)) > 0 )
    {
        nread = 0;
        while( len > 0 )
        {
            event = (struct inotify_event *)&buf[nread];
            for(i=0; i<EVENT_NUM; i++)
            {
                if((event->mask >> i) & 1)
                {
					//监控文件,若该文件有修改
                    if(strcmp(event_str[i],"IN_DELETE_SELF")==0)
					{
                        callback(filePath,0,0,NULL);
                        DEBUG("%s has been changed.",filePath);
                    }
					
					//监控文件夹，若文件夹中的文件有修改
					//else if((IN_ISDIR & event->mask) && (strcmp(event_str[i],"IN_MOVED_TO")==0))
                    else if( strcmp(event_str[i],"IN_MOVED_TO")==0)
					{
                        callback(event->name,0,0,NULL);
						DEBUG(" %s has been changed in %s",event->name,filePath);
					}

                }
            }
            nread = nread + sizeof(struct inotify_event) + event->len;
            len = len - sizeof(struct inotify_event) - event->len;
        }
	    usleep(1000);	//等待1ms，有部分文件在更新后会立刻通过异步信号重新将本地文件加载到内存，在这期间，该文件没有读权限，从新添加监控事件会失败
        wd = inotify_add_watch(fd, filePath, IN_ALL_EVENTS);
	    if(wd < 0){
	        ERROR("inotify_add_watch %s failed\n", filePath);
	        return NULL;
	    }
    }
	inotify_rm_watch(fd, wd);
	return NULL;	
}




