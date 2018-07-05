/*================================================================
*     Copyright (c) 2017年 All rights reserved.
*   
*   文件名称：FileMonitor.h
*   创 建 者：Zhang Qiongying
*   创建日期：2017年12月29日
*   描    述：采用inotify监控文件或目录
*
#pragma once
================================================================*/
#ifndef __FILE_MONITOR_H__
#define __FILE_MONITOR_H__

#include "ConfigFileReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "ostype.h"
#include "../base/segment/seg_word_api.h"

class CFileMonitor
{
	
public:
    CFileMonitor(char* file_path,callback_t callback);
    CFileMonitor();
    ~CFileMonitor();

	char *GetFilePath(){return m_file_path;}
	callback_t GetCallBack(){return m_callback;}
	
	void SetFilePath(char *filePath){m_file_path = filePath;}
	void SetCallBack(callback_t callback){m_callback = callback;}
	
private:
	char *m_file_path;//文件路径
	callback_t m_callback;//执行的操作
};

void logLevelUpdate(void* callback_data, uint8_t msg, uint32_t handle, void* pParam);
void dataUpdate(void* callback_data, uint8_t msg, uint32_t handle, void* pParam);
void * fileMonitor(void *ptr);

#endif /*defined(__FILE_MONITOR_H__) */
