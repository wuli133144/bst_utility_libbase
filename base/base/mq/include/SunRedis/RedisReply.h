#ifndef _Redis_REPLY_H_
#define _Redis_REPLY_H_

#include <string>
#include <hiredis/hiredis.h>

#include "RedisNode.h"

namespace SunRedis
{

class SunRedisReply {
public:
	SunRedisReply(redisReply* const reply, bool isConnect = true, const bool autoFree=true);
	virtual ~SunRedisReply();
	//拷贝构造函数
	SunRedisReply(const SunRedisReply& reply);

	bool empty() const;

	SunRedisReply& operator=(redisReply* const reply);

//	operator redisReply*&();

	void Print();

    void FreeReply();
    static void FreeReply(redisReply*& reply);

    bool CheckReply();
    static bool CheckReply(const redisReply* const reply);

    /**
     * 判断是否需要切换到集群其它节点上去进行操作
     */
    static bool ExistCluster(const redisReply* reply, Node& node, unsigned int& slot);

	std::string str();

	redisReply * getRedisReply()
	{
	    return mRedisReply;
	}

	bool isConnect()
	{
	    return misConnect;
	}
private:
	bool misConnect;
    bool mAutoFree;
    redisReply* mRedisReply;
};

}

#endif /* _Redis_REPLY_H_ */
