#ifndef _IDEM_CONFIG_H_
#define _IDEM_CONFIG_H_

#include <SunRedis/RedisNode.h>
#include <string>
#include "RocketMQClient.h"

namespace rocketmq {
class ROCKETMQCLIENT_API IdemConfig {
 public:
  IdemConfig();
  bool init(const std::string& jsonStr, Mode & mode);
  SunRedis::RedisNodeVec& redisNodes();

 private:
  SunRedis::RedisNodeVec mNodes;
};

#define JSON_GET_STRING(JSON_OBJ, FIELD_NAME, VAR_NAME) \
  do {                                                  \
    if (JSON_OBJ[FIELD_NAME].isString()) {              \
      VAR_NAME = JSON_OBJ[FIELD_NAME].asString();       \
    } else {                                            \
      VAR_NAME = "";                                    \
    }                                                   \
  } while (0);

#define JSON_GET_INT(JSON_OBJ, FIELD_NAME, VAR_NAME)                       \
  do {                                                                     \
    VAR_NAME =                                                             \
        (JSON_OBJ[FIELD_NAME].isInt() ? JSON_OBJ[FIELD_NAME].asInt() : 0); \
  } while (0);
}
#endif
