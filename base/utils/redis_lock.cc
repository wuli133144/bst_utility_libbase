	
	   string keys=GROUP_MESSAGE_ACK_LOCK;
	   uint64_t currentTms=get_tick_count();//ms 
	   
	   bRet1=pCacheConn->setNx(keys,int2string(timeout+300+1));//cache lock 300ms timeout
	   if(bRet1 ||( (currentTms > string2int( pCacheConn->get(keys) ) ) && (currentTms > pCacheConn->getset(keys,int2string(currentTms+300+1))) )){
            string Keystr="t_user_recent_session";
            string fields=+"session_"+int2string(nFromId%16)+"_"+int2string(nFromId)+"_"+int2string(nToId);
            string values=int2string(nLastReadMsgId);
            pCacheConn->hset(Keystr,fields,values);
			pCacheConn->del(keys);
			bRet=true;
	   }
	   	