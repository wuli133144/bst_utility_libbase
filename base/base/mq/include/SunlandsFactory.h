#ifndef __SUNLANDS_FACTORY_H_
#define __SUNLANDS_FACTORY_H_

#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "ConsumeType.h"
#include "SendResult.h"

namespace rocketmq {
class DefaultMQAdmin;
class DefaultMQProducer;
class RetrySendMessageData;
class DefaultMQPullConsumer;
class DefaultMQPushConsumer;
class MQMessage;
class MQMessageQueue;
class MQueueListener;
class MQProducer;
class MQConsumer;
class MQMessageListener;
class MessageListenerConcurrently;
class MessageListenerOrderly;
class MessageQueueSelector;
class SessionCredentials;
class SendResult;
class MQClientFactory;
class SendCallback;
class PullCallback;
class PullResult;
class TopicStat;

static const std::string testAppID = "SunLandsTestFactoryId";
static const std::string testSwitchKey = "SunLandsTestSwitch";
static const std::string testSwitchValue = "open";
static const std::string SunlandNameSpce = "SunLandsNameSpace";
static const std::string Extend = "Extend";
static const std::string testCheckAuthVersion = "SunLandsCheckAuthVersion";
static const std::string TopicInfo = "topicinfo";
static const std::string Securt = "securt";
static const std::string ConstType = "type";
static const std::string ConstProducerGroup = "producer_group";
static const std::string ConstProducerAppList = "producer_app_list";
static const std::string ConstConsumerAppList = "consumer_app_list";

enum RocketMQConsumerType {
  ROCKETMQ_COMMON_TYPE = 1,
  ROCKETMQ_ORDERLY_TYPE = 2,
  ROCKETMQ_TRANSACTION_TYPE = 3,
};

enum ToolOperateType {
  OPERATETYPEBEGIN = 0,
  QUERYCONFIG = 5,
  ADDTOPICPRODUCERPAIR = 6,  // 给某个APPID下的一个生产者集群的增加一个topic信息
  REMOVETOPICPRODUCERPAIR = 7,  // 给某个APPID下的一个生产者集群的删除一个topic信息
  ADDTOPICCONSUMERPAIR = 8,  // 给某个APPID下的一个消费者集群的增加一个topic信息
  REMOVETOPICCONSUMERPAIR = 9,  // 给某个APPID下的一个消费者集群的删除一个topic信息
  OPERATETYPEEND,
};

class RocketmqConfigToolArgsV2 {
 public:
  RocketmqConfigToolArgsV2():
	  topicType(ROCKETMQ_COMMON_TYPE),operate_type(OPERATETYPEBEGIN) {}

  const bool isRemoveOperate() const {
	  if(operate_type == REMOVETOPICPRODUCERPAIR ||
		  operate_type == REMOVETOPICCONSUMERPAIR){
		  return true;
	  }
	  return false;
  }

  const bool isAddOperate() const {
	  if(operate_type == ADDTOPICPRODUCERPAIR ||
		  operate_type == ADDTOPICCONSUMERPAIR){
		  return true;
	  }
	  return false;
  }

  const bool isOperaProducer() const {
	  if( operate_type == REMOVETOPICPRODUCERPAIR ||
		  operate_type == ADDTOPICPRODUCERPAIR){
		  return true;
	  }
	  return false;
  }

  const bool isOperaConsumer() const {
	  if( operate_type == ADDTOPICCONSUMERPAIR ||
		  operate_type == REMOVETOPICCONSUMERPAIR){
		  return true;
	  }
	  return false;
  }

  RocketMQConsumerType topicType;
  std::string namesrv;
  std::string groupname;

  std::string topic;
  std::string appid;
  std::string secret;
  std::string space;
  ToolOperateType operate_type;
//  std::string clusterName;
};

class SunlandsDefaultMQAdmin {
public:
	SunlandsDefaultMQAdmin();
	virtual ~SunlandsDefaultMQAdmin();
	friend class SunlandsFactoryAPI;

	void setSessionCredentials(const std::string& input_accessKey,
		const std::string& input_secretKey,
		const std::string& input_onsChannel);
	const SessionCredentials& getSessionCredentials() const;

	void getAllTopicStat(std::map<std::string/*topic*/, TopicStat> & topic_stat);

	void getOneTopicStat(const std::string & topic_name, TopicStat & topic_stat);
private:
	DefaultMQAdmin * admin;
};

class SunlandsDefaultMQProducer {
 public:
  SunlandsDefaultMQProducer(const std::string& groupname);
  virtual ~SunlandsDefaultMQProducer();
  friend class SunlandsFactoryAPI;
  virtual void start();
  virtual void shutdown();

  bool checkGroupAuth(const std::string& addr,
                      const std::string& projectNamespace,
                      SessionCredentials& sessionCredentials,
                      const std::string& appId, const std::string& secret,
                      const std::string& groupName, int groupType,
                      const std::set<std::string>& topicIdList, int topicType);
  void initFactory();
  const std::string& getNamesrvAddr() const;
  void setNamesrvAddr(const std::string& namesrvAddr);
  const std::string& getNamesrvDomain() const;
  void setNamesrvDomain(const std::string& namesrvDomain);
  const std::string& getInstanceName() const;
  void setInstanceName(const std::string& instanceName);
  const std::string& getGroupName() const;
  void setGroupName(const std::string& groupname);
  void setRetryTimes(int times);

  void setSessionCredentials(const std::string& input_accessKey,
                             const std::string& input_secretKey,
                             const std::string& input_onsChannel);
  const SessionCredentials& getSessionCredentials() const;

  SendResult send(MQMessage& msg, bool bSelectActiveBroker = true);
  SendResult send(MQMessage& msg, const MQMessageQueue& mq);
  SendResult send(MQMessage& msg, std::string shardingKey,
                  MessageQueueSelector* selector);
  SendResult send(MQMessage& msg, std::string shardingKey,
                  MessageQueueSelector* selector, int autoRetryTimes,
                  bool bActiveBroker = true);
  SendStatus send(MQMessage& msg, SendCallback* pSendCallback,
            bool bSelectActiveBroker = true);
  SendStatus send(MQMessage& msg, const MQMessageQueue& mq,
            SendCallback* pSendCallback);
  SendStatus send(MQMessage& msg, std::string shardingKey,
            MessageQueueSelector* selector, SendCallback* pSendCallback);
  SendStatus sendOneway(MQMessage& msg, bool bSelectActiveBroker = true);
  SendStatus sendOneway(MQMessage& msg, const MQMessageQueue& mq);
  SendStatus sendOneway(MQMessage& msg, std::string shardingKey,
                  MessageQueueSelector* selector);

  MQClientFactory* getFactory() const;

  void setSendMsgTimeout(int sendMsgTimeout);
  void setTcpTransportTryLockTimeout(uint64_t timeout);  // ms
  void setTcpTransportConnectTimeout(uint64_t timeout);  // ms

  void addRetrySendMessage(RetrySendMessageData& data);
  void removeRetrySendMessage(int index);

 private:
  DefaultMQProducer* producer;
};

class SunlandsDefaultMQPullConsumer {
 public:
  SunlandsDefaultMQPullConsumer(const std::string& groupname);
  virtual ~SunlandsDefaultMQPullConsumer();
  friend class SunlandsFactoryAPI;
  virtual void start();
  virtual void shutdown();

  bool checkGroupAuth(const std::string& addr,
                      const std::string& projectNamespace,
                      SessionCredentials& sessionCredentials,
                      const std::string& appId, const std::string& secret,
                      const std::string& GroupName, int groupType,
                      const std::set<std::string>& topicIdList, int topicType);

  void initFactory();
  const std::string& getNamesrvAddr() const;
  void setNamesrvAddr(const std::string& namesrvAddr);
  const std::string& getNamesrvDomain() const;
  void setNamesrvDomain(const std::string& namesrvDomain);
  const std::string& getInstanceName() const;
  void setInstanceName(const std::string& instanceName);
  const std::string& getGroupName() const;
  void setGroupName(const std::string& groupname);

  void setSessionCredentials(const std::string& input_accessKey,
                             const std::string& input_secretKey,
                             const std::string& input_onsChannel);
  const SessionCredentials& getSessionCredentials() const;

  void registerMessageQueueListener(const std::string& topic,
                                    MQueueListener* pListener);

  MQClientFactory* getFactory() const;
  void setMessageModel(MessageModel messageModel);

  virtual void fetchSubscribeMessageQueues(const std::string& topic,
                                           std::vector<MQMessageQueue>& mqs);

  virtual PullResult pull(const MQMessageQueue& mq,
                          const std::string& subExpression, uint64_t offset,
                          int maxNums);
  virtual void pull(const MQMessageQueue& mq, const std::string& subExpression,
                    uint64_t offset, int maxNums, PullCallback* pPullCallback);

 private:
  DefaultMQPullConsumer* consumer;
};

class SunlandsDefaultMQPushConsumer {
 public:
  SunlandsDefaultMQPushConsumer(const std::string& groupname);
  virtual ~SunlandsDefaultMQPushConsumer();
  friend class SunlandsFactoryAPI;
  virtual void start();
  virtual void shutdown();

  bool checkGroupAuth(const std::string& addr,
                      const std::string& projectNamespace,
                      SessionCredentials& sessionCredentials,
                      const std::string& appId, const std::string& secret,
                      const std::string& GroupName, int groupType,
                      const std::set<std::string>& topicIdList, int topicType);

  void initFactory();
  const std::string& getNamesrvAddr() const;
  void setNamesrvAddr(const std::string& namesrvAddr);
  const std::string& getNamesrvDomain() const;
  void setNamesrvDomain(const std::string& namesrvDomain);
  const std::string& getInstanceName() const;
  void setInstanceName(const std::string& instanceName);
  const std::string& getGroupName() const;
  void setGroupName(const std::string& groupname);

  void setSessionCredentials(const std::string& input_accessKey,
                             const std::string& input_secretKey,
                             const std::string& input_onsChannel);
  const SessionCredentials& getSessionCredentials() const;
  void setConsumeFromWhere(ConsumeFromWhere consumeFromWhere);

  /*
    set consuming thread count, default value is cpu cores
  */
  void setConsumeThreadCount(int threadCount);
  int getConsumeThreadCount() const;

  /*
    for orderly consume, set the pull num of message size by each pullMsg,
    default value is 1;
  */
  void setConsumeMessageBatchMaxSize(int consumeMessageBatchMaxSize);
  int getConsumeMessageBatchMaxSize() const;

  void setMaxReconsumeTimes(int maxConsumeTimes);
  int getMaxReconsumeTimes() const;

  void setConsumeTimestamp(uint64_t timestamp);
  uint64_t getConsumeTimestamp() const;

  MQClientFactory* getFactory() const;

  void setMessageModel(MessageModel messageModel);

  void subscribe(const std::string& topic, const std::string& subExpression);

  void setTcpTransportTryLockTimeout(uint64_t timeout);  // ms

  void setTcpTransportConnectTimeout(uint64_t timeout);  // ms

  void registerMessageListener(MQMessageListener* pMessageListener);

  void setAsyncPull(bool asyncFlag);

 private:
  DefaultMQPushConsumer* consumer;
};

class SunlandsFactoryProperty {
 public:
  SunlandsFactoryProperty();
  virtual ~SunlandsFactoryProperty();
  bool checkValidityOfFactoryProperties(const std::string& key,
                                        const std::string& value);
  const char* getLogPath();
  void setSendMsgTimeout(const int value);
  void setSendMsgRetryTimes(const int value);
  void setMaxMsgCacheSize(const int size);
  void setConsumeThreadNums(const int size);
  void setNewConsumeFromWhere(ConsumeFromWhere value);
  void setConsumeMessageBatchMaxSize(const int size);
  void setMaxReconsumeTimes(const int size);
  void setFactoryProperty(const char* key, const char* value);
  void setSubscribeTopicAndTagInfo(const std::string& topic,
                                   const std::string& tag);
  void setPublishTopic(const std::string& topic);

  const std::map<std::string, std::string> getPublishTopicAndTagInfo();

  void setFactoryProperties(
      std::map<std::string, std::string> factoryProperties);
  std::map<std::string, std::string> getFactoryProperties();
  const char* getProducerId();
  const char* getConsumerId();
  std::set<std::string> getPublishTopics();
  const char* getMessageModel();
  const int getSendMsgTimeout();
  const int getSendMsgRetryTimes();
  const int getConsumeThreadNums();
  ConsumeFromWhere getNewConsumeFromWhere();
  const int getMaxMsgCacheSize();
  const int getConsumeMessageBatchMaxSize();
  const int getMaxReconsumeTimes();

  const char* getMessageContent();
  const char* getNameSrvAddr();
  const char* getNameSrvDomain();
  const char* getAccessKey();
  const char* getSecretKey();
  const char* getAppID();
  const char* getNameSpace();
  const char* getNewConsumeTimestampString();

 public:
  // static const char* LogPath;
  static const char* AppId;
  static const char* ProducerId;
  static const char* ConsumerId;
  static const char* MsgContent;
  static const char* AccessKey;
  static const char* SendMsgTimeoutMillis;
  static const char* NAMESRV_ADDR;
  static const char* ConsumeThreadNums;
  static const char* NewConsumeFromWhere;
  static const char* NewConsumeTimeStamp;
  static const char* MaxMsgCacheSize;
  static const char* SendMsgRetryTimes;
  static const char* ConsumeMessageBatchMaxSize;
  static const char* MaxReconsumeTimes;
  static const char* NameSpace;

 private:
  // topic,tag
  std::map<std::string, std::string> m_publishTopicInfos;
  std::map<std::string, std::string> m_sunlandsFactoryProperties;
};

class SunlandsFactoryAPI {
 public:
  SunlandsFactoryAPI();
  virtual ~SunlandsFactoryAPI();

  /*
   * 参数中指定一个producerid，一组topicid，轮询检查每一组 topicid
   * 是否在本服务已经创建过producer对象，
   * 如果创建过则返回null，如果没有创建则尝试创建，并进行配置检查，配置检查成功则返回创建的producer对象
   */
  virtual SunlandsDefaultMQProducer* createProducer(
      SunlandsFactoryProperty factoryProperty, std::string& error);
  virtual SunlandsDefaultMQProducer* createOrderProducer(
      SunlandsFactoryProperty factoryProperty, std::string& error);
  // 检查此consumerid-topic组是否已经在本机上创建过consumer对象
  bool isConsumerTopicExist(
      const std::string& consumerId,
      const std::map<std::string, std::string>& topicAndTags,
      std::string& error);
  // 检查完配置之后，进行consumer对象的初始化，订阅topic处理
  void initAfterCheckConfig(
      MQConsumer* consumer, const std::string& consumerId,
      const std::map<std::string, std::string>& topicAndTags,
      MQMessageListener* listener);
  /*
   * 参数中指定一个consumerid，一组topicid，轮询检查每一组 consumerid-topicid
   * 是否在本服务已经创建过consumer对象，
   * 如果创建过则返回null，如果没有创建则尝试创建，并进行配置检查，配置检查成功则返回创建的consumer对象
   */
  virtual SunlandsDefaultMQPullConsumer* createPullConsumer(
      SunlandsFactoryProperty factoryProperty, std::string& error);
  virtual SunlandsDefaultMQPushConsumer* createPushConsumer(
      SunlandsFactoryProperty factoryProperty,
      MessageListenerConcurrently* listener, std::string& error);
  virtual SunlandsDefaultMQPushConsumer* createOrderPushConsumer(
      SunlandsFactoryProperty factoryProperty, MessageListenerOrderly* listener,
      std::string& error);

  virtual bool canSendMessage(const std::string& groupname,
                              const std::string& topicId);
  virtual bool canSubscribeMessage(const std::string& groupname,
                                   const std::string& topicId);
  SunlandsDefaultMQProducer* getMQProducerByTopicID(const std::string& topicId);

  SunlandsDefaultMQPullConsumer* getMQPullConsumer(
      const std::string& groupname);

  SunlandsDefaultMQPushConsumer* getMQPushConsumer(
      const std::string& groupname);

  SunlandsDefaultMQProducer* getMQProducer(const std::string& groupname);

 private:
  virtual void initBrokerServerByProducer(MQProducer* producer,
                                          std::string topicId);
  virtual void initBrokerServerByConsumer(MQConsumer* consumer,
                                          std::string topicId);

 private:
  //发布的topic,一个topic只能由一个MQProducer发布,topicId->MQProducer
  std::map<std::string, SunlandsDefaultMQProducer*> topicToProducerMap;

  /*
   * groupname - consumer *
   */
  std::map<std::string, SunlandsDefaultMQPullConsumer*> PullConsumerMap;
  /*
   * groupname - consumer *
   */
  std::map<std::string, SunlandsDefaultMQPushConsumer*> PushConsumerMap;
  // producerId->发布的topic集合
  std::map<std::string, std::vector<std::string> > producerIdToTopicsMap;

  // topicId->consumerIds
  std::map<std::string, std::vector<std::string> > topicIdToConsumerIdsMap;
  //发布的producerIds
  std::set<std::string> producerIdsSet;
  //订阅的consumerIds
  std::set<std::string> consumerIdsSet;
};

class SunlandsFactory {
 public:
  virtual ~SunlandsFactory();
  static SunlandsFactoryAPI* getInstance();

 private:
  SunlandsFactory();
  static SunlandsFactoryAPI* sunlandsFactoryInstance;
};
}
#endif
