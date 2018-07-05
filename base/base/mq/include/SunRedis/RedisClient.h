#ifndef _Redis_CLIENT_H_
#define _Redis_CLIENT_H_

#include "RedisCmd.h"

using namespace std;

namespace SunRedis
{

class RedisClient : public RedisCmd
{
public:
	RedisClient();
    virtual ~RedisClient();

public:

    //              Connection
    /* AUTH        */  /* nonsupport */
    /* ECHO        */  int echo(const std::string& str, Value &value);
    /* PING        */  /* nonsupport */
    /* SELECT      */  /* nonsupport */

    //                 Commands operating on std::string values
    /* APPEND      */  int append( const Key& key,  const Value& value);
    /* BITCOUNT    */  int bitcount(const Key& key, int& count, int start=0, int end=0);
    /* BITOP       */  //int bitop(  const std::string& destkey, const BITOP operation, const KEYS& keys, int& lenght);
    /* BITPOS      */  int bitpos( const Key& key, int bit, int64_t& pos, int start=0, int end=0);
    /* DECR        */  int decr(   const Key& key, int64_t& result);
    /* DECRBY      */  int decrby( const Key& key, int by, int64_t& result);
    /* GET         */  int get(    const Key& key, std::string& value);
    /* GETBIT      */  int getbit( const Key& key, int& offset, int& bit);
    /* GETRANGE    */  int getrange(const Key& key, int start, int end, std::string& out);
    /* GETSET      */  int getset( const Key& key, const Value& newValue, Value& oldValue);
    /* INCR        */  int incr(   const Key& key, int64_t& result);
    /* INCRBY      */  int incrby( const Key& key, int by, int64_t& result);
    /* INCRBYFLOAT */  
    /* MGET        */  //int mget(const DBIArray& dbi,    const KEYS &  keys, ReplyData& vDdata);
    /* MSET        */  //int mset(const DBIArray& dbi,    const VDATA& data);
    /* MSETNX      */ 
	enum SET_MODE {
		SET_MODE_NORMAL,
		SET_MODE_NX,
		SET_MODE_XX,
	};
	/* SET COMMON  */  int set(const std::string& key, const std::string& value,
 		int timeout, SET_MODE set_mode);
    /* PSETEX      */  int psetex( const Key& key,  int milliseconds, const Value& value);
    /* SET         */  int set( const Key& key,  const Value& value);
    /* SET         */  int setByLen( const Key& key, const char *value, int len, int second);
    /* SETBIT      */  int setbit( const Key& key,  int offset, int64_t newbitValue, int64_t oldbitValue);
    /* SETEX       */  int setex(  const Key& key,  int seconds, const Value& value);
    /* SETNX       */  int setnx(  const Key& key,  const std::string& value);
    /* SETRANGE    */  int setrange(const Key& key,  int offset, const Value& value, int& length);
    /* STRLEN      */  int strlen( const Key& key, int& length);


    /* DEL          */  int del(   const Key& key);
                        //int del(const DBIArray& dbi,      const KEYS &  vkey, int64_t& count);
    /* DUMP         */
    /* EXISTS       */  int exists(const Key& key);
    /* EXPIRE       */  int expire(const Key& key, unsigned int second);
    /* EXPIREAT     */  int expireat(const Key& key, unsigned int timestamp);
    /* KEYS         */  
    /* MIGRATE      */  
    /* MOVE         */  
    /* OBJECT       */  
    /* PERSIST      */  int persist(const Key& key);
    /* PEXPIRE      */  int pexpire(const Key& key, unsigned int milliseconds);
    /* PEXPIREAT    */  int pexpireat(const Key& key, unsigned int millisecondstimestamp);
    /* PTTL         */  int pttl(const Key& key,  int64_t &milliseconds);
    /* RANDOMKEY    */  int randomkey( Key& key);
    /* RENAME       */  
    /* RENAMENX     */  
    /* RESTORE      */  
    /* SCAN         */  

    
    /* SORT         */  int sort(const Key& key, ArrayReply& array, const char* by = NULL,
    								LIMIT *limit = NULL, bool alpha = false, const FILEDS* get = NULL,
									const SORTODER order = ASC, const char* destination = NULL);

    /* TTL          */  int ttl(const Key& key, int64_t& seconds);
    /* TYPE         */  


    /* HDEL         */  int hdel(   const Key& key, const std::string& field, int64_t& num);
                        int hdel(   const Key& key, const Keys& vfiled, int64_t& num);
    /* HEXISTS      */  int hexist( const Key& key, const std::string& field);
    /* HGET         */  int hget(   const Key& key, const std::string& field, Value& value);
    /* HGETALL      */  int hgetall(const Key& key, ArrayReply& array);
    /* HINCRBY      */  int hincrby(const Key& key, const std::string& field, int64_t increment ,int64_t& value);
    /* HINCRBYFLOAT */  int hincrbyfloat( const Key& key, const std::string& field, const float increment, float& value);
    /* HKEYS        */  int hkeys(  const Key& key, Keys& keys);
    /* HLEN         */  int hlen(   const Key& key, int64_t& count);
    /* HMGET        */  int hmget(  const Key& key, const Keys& field, ArrayReply& array);
    /* HMSET        */  int hmset(  const Key& key, const VDATA& vData);
    /* HSCAN        */                                      
    /* HSET         */  int hset(   const Key& key, const std::string& field, const Value& value, int64_t& retval);
    /* HSETNX       */  int hsetnx( const Key& key, const std::string& field, const Value& value);
    /* HVALS        */  int hvals(  const Key& key, Values& values);

    /* BLPOP        */  
    /* BRPOP        */  
    /* BRPOPLPUSH   */  
    /* LINDEX       */  int lindex(   const Key& key, int64_t index, Value& value);
    /* LINSERT      */  int linsert( const Key& key, LMODEL mod, const std::string& pivot, const Value& value, int64_t& retval);
    /* LLEN         */  int llen(    const Key& key, int64_t& len);
    /* LPOP         */  int lpop(    const Key& key, Value& value);
    /* LPUSH        */  int lpush(   const Key& key, const Values& vValue, int64_t& length);
    /* LPUSHX       */  int lpushx(  const Key& key, const Value& value, int64_t& length);
    /* LRANGE       */  int lrange(  const Key& key, int64_t start, int64_t end, ArrayReply& array);
    /* LREM         */  int lrem(    const Key& key,  int count, const Value& value, int64_t num);
    /* LSET         */  int lset(    const Key& key,  int index, const Value& value);
    /* LTRIM        */  int ltrim(   const Key& key,  int start, int end);
    /* RPOP         */  int rpop(    const Key& key, Value& value);
    /* RPOPLPUSH    */  int rpoplpush(const std::string& key_src, const std::string& key_dest, Value& value);
    /* RPUSH        */  int rpush(   const Key& key, const Values& vValue, int64_t& length);
    /* RPUSHX       */  int rpushx(  const Key& key, const Value& value, int64_t& length);



    /* SADD         */  int sadd(       const Key& key, const Values& vValue, int64_t& count);
    /* SCARD        */  int scard(const Key& key, int64_t& count);
    /* SDIFF        */  //int sdiff(const DBIArray& dbi,       const KEYS& vKkey, VALUES& vValue);
    /* SDIFFSTORE   */  //int sdiffstore( const KEY& destinationkey, const DBIArray& vdbi, const KEYS& vkey, int64_t& count);
    /* SINTER       */  //int sinter(const DBIArray& dbi,      const KEYS& vkey, VALUES& vValue);
    /* SINTERSTORE  */  //int sinterstore(const KEY& destinationkey, const DBIArray& vdbi, const KEYS& vkey, int64_t& count);
    /* SISMEMBER    */  int sismember(  const Key& key,   const Value& member);
    /* SMEMBERS     */  int smembers(    const Key& key,  Values& vValue);
    /* SMOVE        */  int smove(      const std::string& srckey, const Key& deskey,  const Value& member);
    /* SPOP         */  int spop(       const Key& key, Value& member);
    /* SRANDMEMBER  */  int srandmember(const Key& key, Values& vmember, int num=0);
    /* SREM         */  int srem(       const Key& key, const Values& vmembers, int64_t& count);
    /* SSCAN        */  
    /* SUNION       */  //int sunion(const DBIArray& dbi,      const KEYS& vkey, VALUES& vValue);
    /* SUNIONSTORE  */  //int sunionstore(const KEY& deskey, const DBIArray& vdbi, const KEYS& vkey, int64_t& count);

    /* ZADD             */  int zadd(   const std::string& deskey,   const Values& vValues, int64_t& count);
    /* ZCARD            */  int zscrad( const Key& key, int64_t& num);
    /* ZCOUNT           */
    /* ZINCRBY          */  int zincrby(const Key& key, const double &increment, const std::string& member, Value& value );
    /* ZINTERSTORE      */  
    /* ZRANGE           */  int zrange( const Key& key, int start, int end, Values& vValues, bool withscore=false);
    /* ZRANGEBYSCORE    */  
    /* ZRANK            */  int zrank(  const Key& key, const std::string& member, int64_t &rank);
    /* ZREM             */  int zrem(   const Key& key, const Values& vmembers, int64_t &num);
    /* ZREMRANGEBYRANK  */  int zremrangebyrank( const Key& key, int start, int stop, int64_t& num);
    /* ZREMRANGEBYSCORE */  
    /* ZREVRANGE        */  int zrevrange( const Key& key, int start, int end, Values& vValues, bool withscore=false);
    /* ZREVRANGEBYSCORE */  
    /* ZREVRANK         */  int zrevrank( const Key& key, const std::string &member, int64_t& rank);
    /* ZSCAN            */  
    /* ZSCORE           */  int zscore( const Key& key, const std::string &member, std::string& score);
    /* ZUNIONSTORE      */  

    /* PSUBSCRIBE   */
    /* PUBLISH      */
    /* PUBSUB       */
    /* PUNSUBSCRIBE */
    /* SUBSCRIBE    */
    /* UNSUBSCRIBE  */


    /* DISCARD  */
    /* EXEC     */
    /* MULTI    */
    /* UNWATCH  */
    /* WATCH    */
};

}

#endif
