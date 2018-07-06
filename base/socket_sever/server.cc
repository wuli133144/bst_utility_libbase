#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>

//#include "chat_message.hpp"

#include "public_define.h"
#include "ImPduBase.h"
#include "BaseSocket.h"
#include "StringUtils.h"

#include "IM.BaseDefine.pb.h"
#include "IM.Live.pb.h"
#include "IM.RPC.pb.h"
#include "IM.Other.pb.h"
#include "ConfigFileReader.h"

	


//using namespace boost::asio::ip::tcp;
//using namespace boost;
using namespace boost::asio;
using namespace boost::system;

using namespace  IM::BaseDefine;






//----------------------------------------------------------------------

typedef std::deque<CImPdu> chat_message_queue;

//----------------------------------------------------------------------

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const CImPdu& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const CImPdu& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

private:
  std::set<chat_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class chat_session
  : public chat_participant,
    public std::enable_shared_from_this<chat_session>
{
public:
  chat_session(boost::asio::ip::tcp::socket socket, chat_room& room)
    : socket_(std::move(socket)),
      room_(room)
  {
     //socket_(io_service);
  }

  void start()
  {
    room_.join(shared_from_this());
	std::cout<<"start()"<<std::endl;
	if(socket_.is_open()){
		 std::cout<<"stocket open true "<<std::endl;
	}
    do_read_header();
	
  }

  void deliver(const CImPdu& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
 
  virtual void HandleAllRequest(CImPdu *pPdu){
      std::cout<<"endter HandleAll"<<std::endl;
	  switch (pPdu->GetCommandId())
		  {
		    case CID_OTHER_HEARTBEAT:{
				std::cout<<"heartbeat"<<std::endl;
                break;
			
		    	}
			case CID_LIVE_GET_ROOM_FORBIDDEN_STATUS_REQUEST:{
				std::cout<<"getLiveroomForbidden request"<<std::endl;
				break;
				}
			default:
				{
				  break;
				}
		  }
       
  
  }
  void do_read_header()
  {
    auto self(shared_from_this());
	//std::cout<<"do_read_header"<<std::endl;
	char *pbuffer=new char[1024];
    boost::asio::async_read(socket_,
                            boost::asio::buffer(pbuffer, 16),
                            [this, self,pbuffer](const boost::system::error_code ec, std::size_t /*length*/){
          if (!ec)
          { 
            //std::cout<<"read msg header"<<read_msg_.GetBuffer()<<std::endl;
			std::cout<<pbuffer<<"xxxxpbuffer...."<<std::endl;
			//HandleAllRequest(&read_msg_);
            //do_read_body();
            delete  pbuffer;
			do_read_header();
          }
          else
          { 
            std::cout<<"do_read_header error"<<std::endl;
            room_.leave(shared_from_this());
          }
        });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.GetBodyData(), read_msg_.GetBodyLength()),
        [this, self](const boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          { 
            
		    std::cout<<"read msg body"<<read_msg_.GetBuffer()<<std::endl;
            room_.deliver(read_msg_);
            do_read_header();
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().GetBuffer(),
          write_msgs_.front().GetLength()),
        [this, self](const boost::system::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            room_.leave(shared_from_this());
          }
        });
  }

  boost::asio::ip::tcp::socket socket_;
  chat_room& room_;
  CImPdu read_msg_;
  chat_message_queue write_msgs_;

//  boost::asio::io_service io_service;
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(boost::asio::io_service& io_service1,
      const boost::asio::ip::tcp::endpoint& endpoint)
    : acceptor_(io_service1, endpoint)    
    //,socket_(io_service1)
  {

    do_accept();
  }


private:


  
  void do_accept()
  {
    boost::asio::ip::tcp socket_(acceptor_.get_executor().context());   
    acceptor_.async_accept(socket_,
        [this](const boost::system::error_code ec)
        {
          if (!ec)
          { 

               std::make_shared<chat_session>(socket_, room_)->start();
			
          }

          do_accept();
        });
  }
  
  boost::asio::ip::tcp::acceptor acceptor_;
  chat_room room_;
  //boost::asio::ip::tcp::socket socket_;
};

//----------------------------------------------------------------------



int main(int argc, char* argv[])
{
  try
  {
    /*if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }*/


	
	boost::asio::io_service io_service;

	
    std::list<chat_server>  servers;
	CConfigFileReader config("server_netinfo.conf");
    char* ipstr=config.GetConfigName("ServerIP1");
	char* portstr=config.GetConfigName("ServerPort1");
		
    boost::asio::ip::tcp::endpoint endpoints(boost::asio::ip::address::from_string(ipstr), std::atoi(portstr));

	servers.emplace_back(io_service, endpoints);

    io_service.run();
	
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

