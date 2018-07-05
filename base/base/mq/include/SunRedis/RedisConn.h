
#ifndef _Redis_CONN_H_
#define _Redis_CONN_H_

#include <string>
#include <list>
#include <vector>
#include <hiredis/hiredis.h>
#include "RedisNode.h"


namespace SunRedis
{

class RedisPool;
class RedisConn
{
public:
    RedisConn(RedisPool* pool);
    virtual ~RedisConn();

	bool init(const RedisNode& redisNode);
	bool keepAlive();
	void backPool();
    RedisNode getNode() const  { return mRedisNode; }
	bool canWork();

public:
    redisReply* command(const char *format, ...);
    redisReply* commandv(const char *format, va_list ap);
    redisReply* commandArgv(int argc, const char **argv, const size_t *argvlen);

private:
    
	bool connect();
    bool connectWithTimeout();
	bool auth();
	bool ping();
	void disconnect();    

private:
    // redis connector context
    RedisPool* mPool;
    redisContext* mCtx;
    RedisNode     mRedisNode; // redis node info
    bool          mStatus; // redis connection status
    int 		  mLastPingStamp;
    int           mLastWorkingStamp;
	int 		  mTimeout;
};

}

#endif /* _Redis_CONN_H_ */
