#ifndef _Redis_LOCK_H_
#define _Redis_LOCK_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

namespace SunRedis
{
class RedisLock {
public:
	RedisLock(pthread_mutex_t);
    RedisLock();
    virtual ~RedisLock();
    void Enter();
    void Leave();

private:
    pthread_mutex_t     mMutex;

};

class RedisAutoLock {
public:
    RedisAutoLock(RedisLock& lock);
    virtual ~RedisAutoLock();
private:
    RedisLock& mlock;
};

}

#define RedisLOCK(T) SunRedis::RedisAutoLock lock(T)

#endif /* _Redis_LOCK_H_ */

