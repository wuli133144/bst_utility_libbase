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
#include "IM.Message.pb.h"

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
     std::thread work([&]{
	       std::cout<<"test msg ack Cache function"<<std::endl;
		    
		    IM::Message::IMMsgDataAck msg;
			msg.set_user_id(3662414);
			msg.set_session_id(7899);
			msg.set_msg_id(12);
			msg.set_create_time((uint64_t)time(NULL));
			msg.set_session_type(2);
			msg.set_result_code(0);
			
			
			CImPdu pdu;
		    pdu.SetPBMsg(&msg);
		    pdu.SetServiceId(SID_MSG);                            
		    pdu.SetCommandId(CID_MSG_DATA_ACK);
			socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		   
           
	 });

	      std::thread work1([&]{
	       std::cout<<"test msg ack Cache function"<<std::endl;
		    
		    IM::Message::IMMsgDataAck msg;
			msg.set_user_id(3662415);
			msg.set_session_id(7899);
			msg.set_msg_id(12);

			msg.set_create_time((uint64_t)time(NULL));
			msg.set_session_type(2);
			msg.set_result_code(0);
			
			
			CImPdu pdu;
		    pdu.SetPBMsg(&msg);
		    pdu.SetServiceId(SID_MSG);
		    pdu.SetCommandId(CID_MSG_DATA_ACK);
			socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		   
           
	 });

	    std::thread work2([&]{
	       std::cout<<"test msg ack Cache function"<<std::endl;
		    
		    IM::Message::IMMsgDataAck msg;
			msg.set_user_id(3662413);
			msg.set_session_id(7899);
			msg.set_msg_id(12);

			msg.set_create_time((uint64_t)time(NULL));
			msg.set_session_type(2);
			msg.set_result_code(0);
			
			
			CImPdu pdu;
		    pdu.SetPBMsg(&msg);
		    pdu.SetServiceId(SID_MSG);
		    pdu.SetCommandId(CID_MSG_DATA_ACK);
			socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		   
           
	 });
	 //1816394 1858937 1859909 1860721

	    std::thread work3([&]{
	       std::cout<<"test msg ack Cache function"<<std::endl;
		    
		    IM::Message::IMMsgDataAck msg;
			msg.set_user_id(3662412);
			msg.set_session_id(7899);
			msg.set_msg_id(12);

			msg.set_create_time((uint64_t)time(NULL));
			msg.set_session_type(2);
			msg.set_result_code(0);
			
			
			CImPdu pdu;
		    pdu.SetPBMsg(&msg);
		    pdu.SetServiceId(SID_MSG);
		    pdu.SetCommandId(CID_MSG_DATA_ACK);
			socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		   
           
	 });
	  std::thread work4([&]{
	       std::cout<<"test msg ack Cache function"<<std::endl;
		    
		    IM::Message::IMMsgDataAck msg;
			msg.set_user_id(5001);
			msg.set_session_id(7899);
			msg.set_msg_id(12);

			msg.set_create_time((uint64_t)time(NULL));
			msg.set_session_type(2);
			msg.set_result_code(0);
			
			
			CImPdu pdu;
		    pdu.SetPBMsg(&msg);
		    pdu.SetServiceId(SID_MSG);
		    pdu.SetCommandId(CID_MSG_DATA_ACK);
			socket.send(boost::asio::buffer(pdu.GetBuffer(),pdu.GetLength()));
		   
           
	 });
	
	 	
	 	
	
	//socket.send(boost::asio::buffer())

	
	 
     t.join();
	 f.join();
	 work.join();
	 work1.join();
	 work2.join();
	 work3.join();
	 work4.join();

	 
  }

  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  
  return 0;
}
