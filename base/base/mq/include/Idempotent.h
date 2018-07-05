#ifndef _IDEMPOTENT_
#define _IDEMPOTENT_
#include <string>
#include "IdemConfig.h"
#include "MQMessageExt.h"
#include "RocketMQClient.h"
#include "SunRedis/RedisClient.h"
#include "SunRedis/RedisComm.h"

namespace rocketmq {

#define EXPIRE_TIME 60 * 60 * 24
class ROCKETMQCLIENT_API Idempotent {
 public:
  Idempotent();
  bool init(const std::string& jsonStr);
  /*
   * return EC_REDIS_SUCCEED means msg has not been consumed yet, now can be consumed
   * EC_REDIS_CONNECT_FAIL means can not get redis connection
   * EC_REDIS_ERROR means msg already has been consumed ,now can be not consumed again
   *
   */
  int hasConsume(const std::string &consumerGroup, const MQMessageExt& msgExt);
  bool setReconsume(const std::string & consumerGroup, const MQMessageExt& msgExt);

 private:
  SunRedis::RedisClient mRedisClient;
  IdemConfig mConfig;
};
}
#endif  //_IDEMPOTENT_
