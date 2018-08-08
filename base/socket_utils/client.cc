#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include <memory>
#include <boost/asio.hpp>


#include "public_define.h"
#include "ImPduBase.h"
#include "BaseSocket.h"
#include "StringUtils.h"
#include "IM.BaseDefine.pb.h"
#include "IM.RPC.pb.h"
#include "IM.Other.pb.h"
#include "IM.Message.pb.h"
#include "IM.Group.pb.h"
#include "ConfigFileReader.h"

using boost::asio::ip::tcp;
using namespace IM::BaseDefine;
using namespace IM::Group;
//using namespace IM::Live;


typedef std::deque<CImPdu> chat_message_queue;


class chat_client:public std::enable_shared_from_this<chat_client>
{
public:
	
  chat_client(boost::asio::io_service& io_service,
      const boost::asio::ip::tcp::endpoint& endpoints)
     : io_context_(io_service),
      socket_(io_service)
     
  {
     m_thread_heartbeat=NULL;
     do_connect(endpoints);
	
  }
	  
  ~chat_client(){
  
    if(NULL!=m_thread_heartbeat){
		 delete m_thread_heartbeat;
		 m_thread_heartbeat=NULL;
	}
	
  }
  void write(const CImPdu & msg)
  {    
        io_context_.post(
        [this, &msg]()
        { 
          //std::cout<<"xxxxx"<<std::endl;
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }
  
  //!  create liveroom test async_create_liveroom
  //!  recent createliveroom impl has't resolve
  //！ issues between tars and teamtalk,you know tengcent's tars 
  //!  structure need a current transport object,it contains 
  //!  ip port,functionname and so on,but this client test
  //!  can't create tarsinfo.
  /*void CreateLiveRoom(uint32_t live_id,uint32_t user_id,uint32_t capacitys){
     CImPdu pPdu;
	 IM::RPC::IMRPCCreateLiveChatRoomReq msg;
     msg.set_live_id(live_id);
	 msg.set_user_im_id(user_id);
	 msg.set_live_capacity(capacitys);
	 IM::RPC::TarsInfo* pTarsInfo = msg.mutable_tars_info();
     pTarsInfo->set_fd(1);
     pTarsInfo->set_uid(2);
     pTarsInfo->set_request_id(2);
     pTarsInfo->set_port(2000);
     pTarsInfo->set_ip("www.baidu.com");
     pPdu.SetPBMsg(&msg);
     pPdu.SetServiceId(SID_RPC);
	 pPdu.SetCommandId(CID_RPC_CREATE_LIVEROOM_MSG_REQUEST);
	 
	 write(pPdu);
      
  }*/

  void close()
  {
    io_context_.post([this]() { socket_.close(); });
  }

private:
  void do_connect(const boost::asio::ip::tcp::endpoint& endpoints)
  {
        socket_.async_connect(endpoints,
        [this](const boost::system::error_code ec)
        {
          if (!ec)
          {
            do_read_header();
			std::cout<<"link success start heartbeat"<<std::endl;
			if(!m_thread_heartbeat){
				 m_thread_heartbeat=new thread([this](){
			     while(1){
			      hearbeat();
			      usleep(5000*1000);
			     }
		   
		   });
				}
          	}
	    });
        
       
  }

  void do_read_header()
  {
        boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.GetBuffer(),IM_PDU_HEADER_LEN),
        [this](const boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {     
        boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.GetBodyData(), read_msg_.GetBodyLength()),
        [this](const boost::system::error_code  ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            //std::cout.write(read_msg_.body(), read_msg_.body_length());
            //std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {      
        //std::cout<<"do_write_"<<std::endl;
        //auto self(shared_from_this());
		if(!write_msgs_.empty()){
		 // std::cout<<"[write_msg_]"<<write_msgs_.size()<<std::endl;
		}
		
        boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().GetBuffer(),
          write_msgs_.front().GetLength()),
        [this](const boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          { 
            std::cout<<"do_write_pop_front over callback start"<<std::endl; 
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

  void hearbeat(){

		IM::Other::IMHeartBeat msg;
		
        CImPdu pdu;
        pdu.SetPBMsg(&msg);
        pdu.SetServiceId(SID_OTHER);
        pdu.SetCommandId(CID_OTHER_HEARTBEAT);
		//SendPdu(&pdu);
		write(pdu);
  }
  
private:
  boost::asio::io_service& io_context_;
  boost::asio::ip::tcp::socket socket_;
  CImPdu read_msg_;
  chat_message_queue write_msgs_;
  thread *m_thread_heartbeat;
};



void helper()
{
        std::cout<<"TARS and TeamTalk test function"<<std::endl;
		std::cout<<"select 1 :test create live room"<<std::endl;
		std::cout<<"select 2 :test close live room"<<std::endl;
}




int main(int argc, char* argv[])
{
  try
  {
   

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
	
	CConfigFileReader config("client_netinfo.conf");
	char* ipstr=config.GetConfigName("ServerIP1");
	char* portstr=config.GetConfigName("ServerPort1");
	boost::asio::ip::tcp::socket socket(io_service);
	//socket(io_service);
    boost::asio::ip::tcp::endpoint endpoints(boost::asio::ip::address::from_string(ipstr), std::atoi(portstr));
    socket.connect(endpoints);
	  
	std::thread t([&io_service]{ io_service.run();});
	std::thread f([&]{
	while(1){
		
		IM::Other::IMHeartBeat msg;
		CImPdu pdu;
		pdu.SetPBMsg(&msg);
		pdu.SetServiceId(SID_OTHER);
		pdu.SetCommandId(CID_OTHER_HEARTBEAT);

		socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		usleep(5000*1000);}

	});

	#if 0
	std::thread work([&]{
		
		std::cout<<"test  IMGroupMemberDismissReq msg ack Cache function"<<std::endl;
		IM::Group::IMGroupMemberDismissReq msg;    
		//creator 
		//msg.set_creator_id(1856727);
		
		//manager
		msg.set_creator_id(1856728);
		//1856728
		msg.set_group_id(6144);
		msg.set_reason("test creator_id 1856727");
		//msg.set_result_code(0);


		CImPdu pdu;
		pdu.SetPBMsg(&msg);
		pdu.SetServiceId(SID_GROUP);                            
		pdu.SetCommandId(CID_GROUP_MEMBER_DISMISS_REQUEST);
		socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));

		//usleep(1000*100);
        //vector<char>recvbuffer(1024,0);
		unsigned char *recvbuffer=new unsigned char[1024];
		memset(recvbuffer,0,sizeof(unsigned char)*1024);
		
		socket.receive(boost::asio::buffer(recvbuffer,1024));

		IM::Group::IMGroupMemberDismissResp msgresp;
        
	    msgresp.ParseFromArray(recvbuffer+16, 1024);

		//std::cout<<recvbuffer<<std::endl;

		std::cout<<std::endl<<"#############################response start#######################"<<std::endl;
		std::cout<<"group  name :  "<<msgresp.group_name()<<std::endl;
		std::cout<<"result code : "<<msgresp.result_code()<<std::endl;
		std::cout<<"group    id : "<<msgresp.group_id()<<std::endl;
		std::cout<<"creator  id : "<<msgresp.creator_id()<<std::endl;

        for(auto i=0;i < msgresp.member_list_size();i++)
        {
           std::cout<<"member index "<< i<<" imid "<<msgresp.member_list(i)<<" in group [ "<< msgresp.group_id()<<" ]"<<std::endl;
        }
		

		std::cout<<std::endl<<"#############################response end #######################"<<std::endl;

		
		
		delete [] recvbuffer;
		
		

	      
	});

    work.join();
 #endif 
	
std::thread work1([&]{

		std::cout<<"test  managementGroupModify msg ack Cache function"<<std::endl;
		IM::Group::IMGroupManagementModifyReq msg;
		IM::Group::IMGroupManagementModifyResp msgResp; 
		//creator 
		//msg.set_creator_id(1856727);

		//manager
		msg.set_creator_id(1856730);
		msg.set_creator_name("xxxxxxxxxxxxcreator");
		msg.set_group_id(6144);
		//1856728
		msg.set_group_id(6144);
		msg.set_group_name("xxxxxxxgroup name");
		msg.set_peer_id(3305421);//creator
	
		//msg.set_result_code(0);


		CImPdu pdu;
		pdu.SetPBMsg(&msg);
		pdu.SetServiceId(SID_GROUP);                            
		pdu.SetCommandId(CID_GROUP_MANAGEMENT_MODIFY_REQUEST);
		socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));

		//usleep(1000*100);
		//vector<char>recvbuffer(1024,0);
		try{
			unsigned char *recvbuffer=new unsigned char[1024];
			memset(recvbuffer,0,sizeof(unsigned char)*1024);

			socket.receive(boost::asio::buffer(recvbuffer,1024));


			msgResp.ParseFromArray(recvbuffer+16, 1024);

			//std::cout<<recvbuffer<<std::endl;

			std::cout<<std::endl<<"#############################response start#######################"<<std::endl;
			std::cout<<"group  name :  "<<msgResp.group_name()<<std::endl;
			std::cout<<"result code : "<<msgResp.result_code()<<std::endl;
			std::cout<<"group    id : "<<msgResp.group_id()<<std::endl;
			std::cout<<"creator  id : "<<msgResp.creator_id()<<std::endl;
	        std::cout<<"peer   name : "<<msgResp.peer_name()<<std::endl;
			for(auto i=0;i < msgResp.member_list_size();i++)
			{
			   std::cout<<"member index "<< i<<" imid "<<msgResp.member_list(i)<<" in group [ "<< msgResp.group_id()<<" ]"<<std::endl;
			}


			std::cout<<std::endl<<"#############################response end #######################"<<std::endl;



			delete [] recvbuffer;
		
		}
		catch(...){
			 std::cout<<"some system error ocurred!!! please pay attention to it"<<std::endl;
		}



  
});
	 
     t.join();
	 f.join();

	 work1.join();
	

	 
  }

  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  
  return 0;
}
