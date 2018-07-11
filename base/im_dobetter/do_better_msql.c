
void StartConsumer()
{
  
   
   thread t([]{
   	   bool bRet=false;
       std::vector<std::string>vc;
       while(1){
	   	
   	    usleep(3000*1000);//sleep 3seconds
   	    //bRet=CSessionModel::getInstance()->popMessageAckCache2SqlStr(vc);
   	  
        INFO("StartConsumer runing...start");
        if(!map_message_ack.empty()){
			 INFO("StartConsumer lock... map_size=%u",map_message_ack.size());
			 map_mutex_lock.lock();
			 getMessageAckSqls(map_message_ack,vc);			 
		     map_message_ack.clear();
		     map_mutex_lock.unlock();
			 INFO("VC SIZE=%d empty() %d",vc.size(),vc.empty());
			 if(!vc.empty()){
			  	 bRet=CSessionModel::getInstance()->updateReadGroupSessionCache(vc);
			  	 vc.clear(); 
			  }
		     
		}
		INFO("StartConsumer runing... end");
		
	     
	   }

	});
	  t.detach();

}

void getMessageAckSqls(std::map<std::string/*string*/,uint32_t /*msg_id*/ >&maps,std::vector<std::string>&vc)
{
    if(maps.empty()){return;}
	//std::size_t nsz=maps.size();
	std::vector<std::string>sqls,userids,groupids,updates;
	std::map<uint32_t,std::vector<std::string>>msg_ack_valid_tables_;
	
	INFO("getMessageAckSqls START");
	for(auto iter=maps.begin();iter!=maps.end();iter++)
	{
	     string tablemark=findKthStr((*iter).first,"_",1);
	 	 string userId   =findKthStr((*iter).first,"_",2);
		 string groupId  =findKthStr((*iter).first,"_",3);
		 int ntable      =string2int(tablemark);
		 auto iter1=msg_ack_valid_tables_.find(ntable);
		 if(iter1!=msg_ack_valid_tables_.end()){
		 	 ((*iter1).second).push_back(userId+"_"+groupId+"_"+int2string((*iter).second));
		 }else{
		     std::vector<std::string>a_t;
			 a_t.push_back(userId+"_"+groupId+"_"+int2string((*iter).second));
		     msg_ack_valid_tables_.insert(std::map<uint32_t,std::vector<std::string>>::value_type(ntable,a_t));
		 }
	}

	std::size_t nsz=msg_ack_valid_tables_.size();
	std::size_t i=0;
	INFO("map msg_ack_valid_tables=%d",nsz);
	for(auto iter=msg_ack_valid_tables_.begin();iter!=msg_ack_valid_tables_.end(),i<nsz;iter++,i++)
	{    
	       std::vector<std::string> temp=(*iter).second;
		   if(temp.empty())continue;
		   string userId_prefix_tb,groupId_prefix_tb,sqls_prefix_tb,update_prefix_tb;
		   
		   for(auto iter1=temp.begin();iter1!=temp.end();iter1++)
		   {
		            string us=findKthStr((*iter1),"_",0);
					string gp=findKthStr((*iter1),"_",1);
					string msgId=findKthStr((*iter1),"_",2);

					sqls_prefix_tb+=" WHEN user_id="+(us)+" AND object_id="+(gp)+" THEN "+(msgId);
					update_prefix_tb+=" WHEN user_id="+(us)+" AND object_id="+(gp)+" THEN "+int2string(time(NULL));
					if(iter1==temp.begin()){
						groupId_prefix_tb+=" "+gp;
						userId_prefix_tb +=" "+us;
					}else if(iter1==--temp.end()){
						groupId_prefix_tb+=" , "+gp;
					    userId_prefix_tb +=" , "+us;
					}else{
						groupId_prefix_tb+=" "+gp+",";
					    userId_prefix_tb +=" "+us+",";
					}
					
		   }
		   INFO("sqls_prefix_tb=%s ,groupId_prefix_tb=%s,userId_prefix_tb=%s",sqls_prefix_tb.c_str(),groupId_prefix_tb.c_str(),userId_prefix_tb.c_str());
		   sqls.push_back(sqls_prefix_tb);
		   userids.push_back(userId_prefix_tb);
		   updates.push_back(update_prefix_tb);
		   groupids.push_back(groupId_prefix_tb);
	}
	INFO("temp_str");
	string temp_str;

	INFO("msg_ack_valid_tables_ size=%d sqls_size=%d update_size=%d group_id=%d user_id_size=%d",msg_ack_valid_tables_.size(),sqls.size(),updates.size(),groupids.size(),userids.size());
	i=0;
	for(auto iter=msg_ack_valid_tables_.begin();iter!=msg_ack_valid_tables_.end();iter++,i++)
	{   INFO("temp_str_start %d",i);
	    temp_str=" UPDATE t_user_recent_session_"+int2string((*iter).first)+"  SET last_read_msgid = (CASE ";
		temp_str+=sqls[i]+" ELSE  last_read_msgid END) ,";
		temp_str+=" update_ts = (CASE ";
		temp_str+=updates[i]+ " ELSE update_ts END) ";
		temp_str+="WHERE user_id  in ( "+userids[i]+" )"+" AND session_type=" + int2string(IM::BaseDefine::SESSION_TYPE_GROUP)+" AND object_id in ( "+groupids[i]+" )";
		vc.push_back(temp_str);
		INFO("combat %s",temp_str.c_str());
		
	}

	
	std::size_t size=vc.size();
	
	INFO("getMessageAckSqls END");
}


void readMessageAck(CImPdu* pPdu, uint32_t conn_uuid)
{
    IM::Message::IMMsgDataAck msg;
    if(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()))
    {
        uint32_t nFromId = msg.user_id();
        uint32_t nToId = msg.session_id();
        uint32_t nMsgId = msg.msg_id();
        uint32_t nType = msg.session_type();
        uint32_t nNow = (uint32_t)time(NULL);

        //INFO("readMessageAck. fromUserId=%u, groupId                                                                            =%u, msgid=%u, type=%u", nFromId, nToId, nMsgId, nType);

        if (IM::BaseDefine::SessionType_IsValid(nType))
        {
            if(nType== IM::BaseDefine::SESSION_TYPE_SINGLE)
            {
                INFO("invalid msgType. fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nType);
            }
            //群聊文本消息
            else if(nType == IM::BaseDefine::SESSION_TYPE_GROUP)
            {
                CGroupModel* pGroupModel = CGroupModel::getInstance();
                //群是否存在 && 用户是否群成员
                //if (pGroupModel->isGroupExist(nToId) && pGroupModel->isUserInGroup(nFromId, nToId))
                if (pGroupModel->isUserInGroup(nFromId, nToId))
                {  
                   
                    // bool bRet = CSessionModel::getInstance()->updateReadGroupSession(nFromId, nToId, nMsgId);
                     INFO("pushMessageAckCache start fromId =%u toId=%u ,msgId=%u",nFromId,nToId,nMsgId);
                     if(map_message_ack.size()>10000){//too many messageack cache,memory maybe floatout
					 	  CSessionModel::getInstance()->updateReadGroupSession(nFromId, nToId, nMsgId);
					 }
					 string Keys="session_"+int2string(nFromId%16)+"_"+int2string(nFromId)+"_"+int2string(nToId);
					
					 map_mutex_lock.lock();
					 auto iter=map_message_ack.find(Keys);
					 if(iter!=map_message_ack.end()){
					 	  (*iter).second=std::max((*iter).second,nMsgId);
					 }else{
					      map_message_ack.insert(std::map<std::string,uint32_t>::value_type(Keys,nMsgId));
					 }
					 map_mutex_lock.unlock();
					 
                     //bool bRet=CSessionModel::getInstance()->pushMessageAckCache(nFromId,nToId,nMsgId);
					 //if(!bRet){
					 //	 INFO("pushMessageAckCache failed update into mysqldb directly fromId =%u toId=%u ,msgId=%u",nFromId,nToId,nMsgId);
					 //	 CSessionModel::getInstance()->updateReadGroupSession(nFromId, nToId, nMsgId);
					 //}
					
                }
                else
                {
                    INFO("invalid groupId or user not in group. fromUserId=%u, groupId=%u", nFromId, nToId);
                    return;
                }
            }
        }
        else
        {
            INFO("invalid msgType.fromId=%u, toId=%u, msgType=%u", nFromId, nToId, nType);
        }
    }
    else
    {
        INFO("parse pb failed");
    }
}