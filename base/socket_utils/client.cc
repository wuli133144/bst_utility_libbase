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
#include "IM.Live.pb.h"
#include "IM.RPC.pb.h"
#include "IM.Other.pb.h"

#include "ConfigFileReader.h"

using boost::asio::ip::tcp;
using namespace IM::BaseDefine;
using namespace IM::Live;


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
  void CreateLiveRoom(uint32_t live_id,uint32_t user_id,uint32_t capacitys){
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
      
  }

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
	
    boost::asio::ip::tcp::endpoint endpoints(boost::asio::ip::address::from_string(ipstr), std::atoi(portstr));
    chat_client c(io_service, endpoints);

	helper();
	
    std::thread t([&io_service](){ io_service.run(); });
    
    char line[2048]={0};
	std::cout<<"please input your selection:1 ,2,3"<<std::endl;
	
    while (std::cin.getline(line, 2048)){
		
     uint32_t nselect=std::atoi(line);
	 std::cout<<"your choice:"<<nselect<<std::endl;
		 switch(nselect){
		 	case 1:
		 	{	 	
			  //c.CreateLiveRoom(2,1111,333);
			  
		 	 break;
			 
		 	}
			case 2:
			{
			  break;
			}
			case 3:{
				
			std::cout<<"case 3"<<std::endl;				
			IM::Live::IMLiveGetRoomForbiddenStatusReq msg;
			msg.set_live_id(2);
		    msg.set_user_im_id(2222);
			CImPdu pPdu;
			pPdu.SetPBMsg(&msg);
			//std::cout<<"cimpdu end"<<std::endl;
			pPdu.SetCommandId(CID_LIVE_GET_ROOM_FORBIDDEN_STATUS_REQUEST);
			//std::cout<<"setcommand end"<<std::endl;
			pPdu.SetServiceId(SID_LIVE);
			//std::cout<<"setservice end"<<std::endl;
			//std::cout<<"c.write start"<<std::endl;
			c.write(pPdu);
			//std::cout<<"c.write end"<<std::endl;
				break;
			}
			default:{
			 std::cout<<"selection error"<<std::endl;
			 break;
			}
			
		 }
	   
     

   }
	 //sleep(3);
     
     c.close();
    //t.join();
     t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  
  return 0;
}