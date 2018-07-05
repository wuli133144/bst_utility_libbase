#ifndef _Redis_DBSLICE_H_
#define _Redis_DBSLICE_H_

#include "RedisComm.h"
#include "RedisConn.h"
#include "RedisLock.h"
#include "Timer.h"
#include <mutex>
#include <thread>
#include <condition_variable>

namespace SunRedis
{

enum RedisStatus {
    REDISDB_UNCONN,
    REDISDB_WORKING,
    REDISDB_DEAD
};

class RedisPool {
public:
	RedisPool();
	virtual ~RedisPool();
	
	bool init(const RedisNode& redisNode);	
    RedisConn* conn();
    Node getNode() const;	
	void backPool(RedisConn* conn);

private:
	void keepAlive();
	//获取连接数量
	int connCount() const ;
	//定时执行函数
	void timerRun();
	//动态扩容连接池中连接数量
	bool adjustConns();
	
	//获取待保活队列
	void getKeepAliveConns(std::list<RedisConn*>& need_keep_alive_pool);
	//添加链接到保活池中
	void addKeepAliveConns(std::list<RedisConn*>& need_keep_alive_pool);
	void addKeepAliveConn(RedisConn*);
	//添加链接到工作池中
	void addConns(const std::list<RedisConn*>& add_conn_pool);
	void addConn(RedisConn* redisConn);

private:
	//工作连接池
	std::mutex mConnsMutex;
	//保活连接池
	std::mutex mKeepAliveConnsMutex;
	std::list<RedisConn*> mKeepAliveConns;

	Timer mTimer; // 定时器
	
	static int mMinConnCount; //最小连接数，每次调整时增加量
	static int mMaxConnCount; //最大连接数
	bool  mNeedAdjustConns; //是否需要扩容连接池
	int   mAdjustConnsCount; //调整连接池大小的次数，每次增加最小值一倍
	
    std::list<RedisConn*> mConns;
    RedisNode mRedisNode;

};

}

#endif /* _Redis_DBSLICE_H_ */
