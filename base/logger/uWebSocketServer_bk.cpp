#include "uWebSocketServer.h"
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
//using std::chrono::seconds;
mutex conn_q_lock;
mutex msg_q_lock;
condition_variable cond; 
bool uWebSocketServer::instanciated=false;
map<uint32_t, imid_ws_map> live_imid_ws_map;
map<uint32_t, ws_info> anonymous_conn_map;

uWebSocketServer::uWebSocketServer(string ws_host, uint32_t ws_port, uint32_t listener_num, uint32_t consumer_num):ws_host(ws_host), ws_port(ws_port), listener_num(listener_num), consumer_num(consumer_num) {
    if(instanciated)
        return;
    consumerTerminate=false;
    INFO("Start instanciate uWebSocketServer inst");
    INFO("ws_host：%s", ws_host.c_str());
    INFO("ws_port：%d", ws_port);
    INFO("consumer_num: %d", consumer_num);
    INFO("listener_num: %d", listener_num);
}

uWebSocketServer::~uWebSocketServer() {
    DEBUG("Delete listener_threads and consummer_threads");
    for(auto &i_listener:listener_threads) {
        delete i_listener;
    }
    for(auto &i_consumer: consumer_threads) {
        delete i_consumer;
    }

}

void uWebSocketServer::run() {
    listenerInit();
    sleep(1);
    consumerInit();
    uWebSocketServer::instanciated=true;
}

/*
bool uWebSocketServer::userExisted(uint32_t live_id, uint32_t user_imid) {
    if(!roomExisted(live_id)) {
        return false;
    }
    //unique_lock<mutex> lck(conn_q_lock);
    return live_imid_ws_map[live_id].end() != live_imid_ws_map[live_id].find(user_imid);
}

void uWebSocketServer::addUserConn(uint32_t live_id, uint32_t user_imid, p_ws ws) {
    user_ws_elem temp;
    temp[user_imid] = ws;
    if(!roomExisted(live_id))
        unique_lock<mutex> lck(conn_q_lock);
        live_imid_ws_map.emplace(live_id, temp);
    else {
        live_imid_ws_map[live_id][user_imid] = ws;
    }
    ws_info ws_conn = ws_info(ws, false, true); 
    live_imid_ws_map[live_id][user_imid] = ws_conn;
}

void uWebSocketServer::delUserConn(uint32_t live_id, uint32_t user_imid) {
    if (roomExisted(live_id) && userExisted(live_id, user_imid)) {
        //unique_lock<mutex> lck(conn_q_lock);
        live_imid_ws_map[live_id].erase(live_imid_ws_map[live_id].find(user_imid));
    }
}
*/

bool uWebSocketServer::roomExisted(uint32_t live_id) {
    //unique_lock<mutex> lck(conn_q_lock);
    return live_imid_ws_map.end() != live_imid_ws_map.find(live_id);
}

void uWebSocketServer::delRoom(uint32_t live_id) {
    if (roomExisted(live_id)) {
        //unique_lock<mutex> lck(conn_q_lock);
        live_imid_ws_map.erase(live_imid_ws_map.find(live_id));
    }
}

void uWebSocketServer::kickUser(uint32_t live_id, uint32_t user_imid) {
    //ws_conn has existed in live_imid_ws_map which means validate request has sent to route, close previous conn
    if(live_imid_ws_map[live_id].end() != live_imid_ws_map[live_id].find(user_imid)) {
        ws_info pre_conn = live_imid_ws_map[live_id][user_imid];
        pre_conn.ws->setUserData((void *)nullptr);
        //send kick notify
        //string resp = createKickUserResponse(live_id, user_imid);
        if(!pre_conn.ws->isClosed() && !pre_conn.ws->isShuttingDown()) {
            //pre_conn.ws->send(resp.c_str(), resp.size(), uWS::TEXT);
            pre_conn.ws->close();
        }
    }
}

vector<std::string> uWebSocketServer::split(const string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
       // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
    }
    return elems;
}

void uWebSocketServer::terminated() {
    consumerTerminate=true;
}

uint32_t uWebSocketServer::getThreadId() {
    //convert thread_t to uint32_t type
    ostringstream oss;
    oss << this_thread::get_id();
    string stid = oss.str();
    uint32_t tid = stoull(stid);
    return tid;
}


void uWebSocketServer::pushMsgQueue(liveMsg &msg) {
    std::unique_lock<std::mutex> lck(msg_q_lock);
    message_queue.push_back(msg); 
    cond.notify_one();
};

liveMsg uWebSocketServer::popMsgQueue() {
    //std::unique_lock<std::mutex> lck(msg_q_lock);
    liveMsg msg = *message_queue.begin();
    message_queue.erase(message_queue.begin()); 
    return msg;
};
void uWebSocketServer::broadcastMsg(uint32_t live_id, uint32_t sender_imid, liveMsg& msg) {
    //unique_lock<mutex> lck(conn_q_lock);
    for(auto &ws_conn: live_imid_ws_map[live_id]) {
        if(sender_imid != ws_conn.first && !ws_conn.second.ws->isClosed() && !ws_conn.second.ws->isShuttingDown()) {
            ws_conn.second.ws->send(msg.data.c_str(), msg.data.size(), uWS::TEXT);
        }
    }
}

uint32_t uWebSocketServer::listenerInit() {
    listener_threads.assign(listener_num, NULL);
    std::transform(listener_threads.begin(), listener_threads.end(), listener_threads.begin(), [&](std::thread *t) {
        return new std::thread([&]() {
            uWS::Hub h;
            /*
            uS::Async * listener_async = new uS::Async(h.getLoop());
            //init async data to nullptr
            listener_async->setData(nullptr);
            listener_async->start([&](uS::Async *p_async) {
                DEBUG("receive Asycn");
                liveMsg* p_notify = (liveMsg*)p_async->getData();
                if(nullptr != p_notify) {
                    uint32_t sender_imid = p_notify->sender_imid;
                    uint32_t live_id = p_notify->live_id;
                    p_notify = (liveMsg*)p_async->getData();
                    sender_imid = p_notify->sender_imid;
                    live_id = p_notify->live_id;
                    DEBUG("sender_imid: %d, live_id: %d, data: %s", sender_imid, live_id, p_notify->data.c_str());
                
                    if((0 < p_notify->data.size()) && (0 < live_id)) {
                        unique_lock<mutex> lck(conn_q_lock);
                        for(inner_it i_it = live_imid_ws_map[live_id].begin(); i_it != live_imid_ws_map[live_id].end(); ++i_it) {
                            if(sender_imid != i_it->first) {
                                i_it->second.ws->send(p_notify->data.c_str(), p_notify->data.size(), uWS::TEXT);
                            }
                        }
                    }
                    //reset async data to nullptr
                    p_async->setData(nullptr);
                }
            });
            {
            	unique_lock<mutex> lck(conn_q_lock);
            	async_list.push_back(listener_async);
            }nonymous_conn_map
            */
            /*
            uS::Timer *login_request_timer = new uS::Timer(h.getLoop());
            login_request_timer->start([&](uS::Timer *timer) {
                DEBUG("t_login_request timer test");
                chrono::high_resolution_clock::time_point cur_timepoint = chrono::high_resolution_clock::now();
                vector<p_ws> conn_need_closed;
                for(auto &i_it: anonymous_conn_map) {
                    chrono::high_resolution_clock::time_point pre_timepoint = i_it.second.last_heartbeat_timepoint;
                    milliseconds interval = chrono::duration_cast<chrono::milliseconds>(cur_timepoint - pre_timepoint);
                    DEBUG("t_login_request interval: %d", interval.count());
                    if (FIRST_LOGIN_REQUEST_INTERVAL < interval.count()) {
                        //no heartbeat response
                        //i_it>ws->send();
                        conn_need_closed.push_back(i_it.second.ws);
                    }
                }
                unique_lock<mutex> lck(conn_q_lock);
                for(auto &anonymous_conn: conn_need_closed) {
                    anonymous_conn_map.erase(anonymous_conn->getFd());
                    if(!anonymous_conn->isClosed() && !anonymous_conn->isShuttingDown()) {
                        anonymous_conn->close();
                    }
                }
            }, 0, 5000);

            uS::Timer *heart_beat_timer = new uS::Timer(h.getLoop());
            heart_beat_timer->start([&](uS::Timer *timer) {
                DEBUG("t_heart_beat timer test");
                chrono::high_resolution_clock::time_point cur_timepoint = chrono::high_resolution_clock::now();
                unique_lock<mutex> lck(conn_q_lock);
                vector<p_ws> conn_need_closed;
                for(auto &o_it: live_imid_ws_map) {
                    int live_id = o_it.first;
                    for(auto &i_it: live_imid_ws_map[live_id]) {
                        chrono::high_resolution_clock::time_point pre_timepoint = i_it.second.last_heartbeat_timepoint;
                        milliseconds internal = chrono::duration_cast<chrono::milliseconds>(cur_timepoint - pre_timepoint);
                        DEBUG("t_heart_beat interval: %d", internal.count());
                        if (HEARTBEAT_INTERVAL < internal.count()) {
                            //no heartbeat response
                            //i_it->ws->send();
                            conn_need_closed.push_back(i_it.second.ws);
                        }
                    }
                }
                for(auto &conn: conn_need_closed) {
                    if(!conn->isClosed() && !conn->isShuttingDown()) {
                        conn->close();
                    }
                }
            }, 0, 5000);
            */


            h.onMessage([&](p_ws ws, char *message, int length, uWS::OpCode opCode) {
                if(0 < length) {
                    string msg = string(message, length);
                    DEBUG("onMessage: %s, length: %d",  msg.c_str(), length);
                    unique_lock<mutex> lck(conn_q_lock);
                    handleWsMsg(msg.c_str(), ws);
                }
            });

            h.onConnection([&](p_ws ws, uWS::HttpRequest req) {
                DEBUG("onConnection");
                //store anonymous conn 
                unique_lock<mutex> lck(conn_q_lock);
                anonymous_conn_map[ws->getFd()] = ws_info(ws, false, true);
            });

            h.onDisconnection([&](p_ws ws, int length, char *message, size_t close_code) {
                DEBUG("onDisconnection");
                //TODO: send close notify to clien
                if( nullptr != ws->getUserData()) {
                    string* user_data = (string*)ws->getUserData();
                    if (0 < user_data->size()) {
                        DEBUG("user data: %s", user_data->c_str());
                        uint32_t sep_pos=  user_data->find_first_of("_");
                        uint32_t live_id = atoi(user_data->substr(0, sep_pos).c_str());
                        uint32_t user_imid = atoi(user_data->substr(sep_pos + 1).c_str());
                        DEBUG("live_id: %d, user_imid: %d", live_id, user_imid);
                        live_imid_ws_map[live_id].erase(user_imid);
                        if (0 == live_imid_ws_map[live_id].size()) {
                            delRoom(live_id);
                        }
                    }
                    delete user_data;
                }
                DEBUG("onDisconnection finish");
            });

            // This makes use of the SO_REUSEPORT of the Linux kernel
            // Other solutions include listening to one port per thread
            // with or without some kind of proxy inbetween
            if (!h.listen(ws_host.c_str(), ws_port, nullptr, uS::ListenOptions::REUSE_PORT)) {
                ERROR("Failed to listen");
            }
            h.run();
        });
    });
    for_each(listener_threads.begin(), listener_threads.end(), [](thread *t) {
        t->detach();
    });
    return 0;
}

void uWebSocketServer::handleWsMsg(const char* msg, p_ws ws) {
    Document msg_doc;
    CImPdu msg_pdu;
    if (!str_to_json_pb(msg, msg_doc, msg_pdu)) {
        WARN("convert str:%s to json and pb failed", msg);
        return;
    }
    uint32_t web_cmd = msg_doc["cmd"].GetInt();
    if (LIVE_HEARTBEAT == web_cmd) {
        uint32_t user_imid = msg_doc["imId"].GetInt();
        uint32_t live_id = msg_doc["liveId"].GetInt();
        DEBUG("web_cmd: %d, live_id, user_imid: %d", web_cmd, live_id, user_imid);
        if (anonymous_conn_map.end() != anonymous_conn_map.find(ws->getFd())) {
            anonymous_conn_map[ws->getFd()].last_heartbeat_timepoint = chrono::high_resolution_clock::now();
        }

        if (live_imid_ws_map.end() != live_imid_ws_map.find(live_id)) {
            if (live_imid_ws_map[live_id].end() != live_imid_ws_map[live_id].find(user_imid)) {
                live_imid_ws_map[live_id][user_imid].last_heartbeat_timepoint = chrono::high_resolution_clock::now();
                string resp = createHeartbeatResponse(user_imid);
                if(!ws->isClosed() && !ws->isShuttingDown()) {
                    ws->send(resp.c_str(), resp.size(), uWS::TEXT);
                }
            }
        }
    }
    //process if user login in the same ws proxy server
    else {
        if (LIVE_LOGIN_CHAT_ROOM == web_cmd) {
            Value &Object = msg_doc["data"];
            uint32_t user_imid = Object["imId"].GetInt();
            uint32_t live_id = Object["liveId"].GetInt();
            DEBUG("web_cmd: %d, user_imid: %d, live_id: %d", web_cmd, user_imid, live_id);

            // set user data which will be used for disconnection
            string *user_data = new string(to_string(live_id) + "_" + to_string(user_imid));
            ws->setUserData((void *)user_data);

            if (anonymous_conn_map.end() != anonymous_conn_map.find(ws->getFd())) {
                ws_info ws_conn = anonymous_conn_map[ws->getFd()];
                //set conn to not empty, since login request received
                ws_conn.empty_conn = false;
                if(live_imid_ws_map[live_id].end() == live_imid_ws_map[live_id].find(user_imid)) {
                    live_imid_ws_map[live_id][user_imid] = ws_conn;
                    //remove conn from anonymous_conn_map
                    anonymous_conn_map.erase(ws_conn.ws->getFd());
                }
                else {
                    //ws_conn has existed in live_imid_ws_map which means validate request has sent to route, close previous conn
                    ws_info pre_conn = live_imid_ws_map[live_id][user_imid];
                    if (pre_conn.validated) {
                        live_imid_ws_map[live_id][user_imid] = ws_info(ws, true, false);
                    }
                    else {
                        live_imid_ws_map[live_id][user_imid] = ws_info(ws, false, false);
                    }

                    pre_conn.ws->setUserData(nullptr);
					
                    //send kick notify
                    string resp = createKickUserResponse(live_id, user_imid);
                    if(!pre_conn.ws->isClosed() && !pre_conn.ws->isShuttingDown()) {
                        pre_conn.ws->send(resp.c_str(), resp.size(), uWS::TEXT);
                        pre_conn.ws->close();
                    }
					
                    return;
                }
            }
            else {
                // login request already sent
                return;
            }
        }
        CDBServConn* pDBConn = get_db_serv_conn();
        
        if (pDBConn) {
            DEBUG("Got valid DB conn");
            pDBConn->SendPdu(&msg_pdu);
        }
        else {
            ERROR("No valid DB conn");
            //send error resp to client
            //ws->send();
            ws->close();
        }
    }
}

void uWebSocketServer::handleLiveMsg(liveMsg& msg) {
    uint32_t live_cmd = msg.live_cmd;
    uint32_t live_id = msg.live_id;
    uint32_t sender_imid = msg.sender_imid;
    unique_lock<mutex> lck(conn_q_lock);
    if(live_imid_ws_map.end() == live_imid_ws_map.find(live_id)) {
        INFO("No clients of live_id: %d login this proxy server", live_id);
        return;
    }
    
    INFO("live_cmd: %d, live: %d, sender_imid: %d validated successfully", live_cmd, live_id, sender_imid);
    if (CID_LIVE_DATA == live_cmd \
        || CID_LIVE_SET_ROOM_FORBIDDEN_STATUS_RESPONSE == live_cmd \
        || CID_LIVE_GET_ROOM_FORBIDDEN_STATUS_RESPONSE == live_cmd \
        || CID_LIVE_LOGIN_ROOM_RESPONSE == live_cmd \
        || CID_LIVE_LOGOUT_ROOM_RESPONSE == live_cmd) {
        if(live_imid_ws_map[live_id].end() == live_imid_ws_map[live_id].find(sender_imid)) {
            ERROR("Ws conn not found");
            return;
        }

        ws_info sender_ws = live_imid_ws_map[live_id][sender_imid];
        //uint32_t receiver_imid = msg.receiver_imid;
        //ws_info receiver_ws = live_imid_ws_map[live_id][receiver_imid];
        if(!sender_ws.ws->isClosed() && !sender_ws.ws->isShuttingDown()) {
            sender_ws.ws->send(msg.data.c_str(), msg.data.size(), uWS::TEXT);
            if (CID_LIVE_LOGOUT_ROOM_RESPONSE == live_cmd) {
                sender_ws.ws->close();
            }
        }
    }
    else if (CID_LIVE_NOTIFY_NEW_MSG == live_cmd \
        || CID_LIVE_NOTIFY_MEMBER_LOGIN == live_cmd \
        || CID_LIVE_NOTIFY_MEMBER_LOGOUT == live_cmd \
        || CID_LIVE_NOTIFY_ROOM_FORBIDDEN_STATUS == live_cmd) {
        /*
        ws_info sender_ws = live_imid_ws_map[live_id][sender_imid];
        sender_ws.p_async->setData((void *)&msg);
        sender_ws.p_async->send();
        */
        broadcastMsg(live_id, sender_imid, msg);
    }
    else if (CID_LIVE_KICK_USER) {
        kickUser(live_id, sender_imid);
    }
    else{
        ERROR("Wrong live_cmd: %d", live_cmd);
    }
}

uint32_t uWebSocketServer::consumerInit() {
    consumer_threads.assign(consumer_num, NULL);
    transform(consumer_threads.begin(), consumer_threads.end(), consumer_threads.begin(), [&](thread *t) {
        return new thread([&]() {
            uint32_t tid = getThreadId();
            liveMsg live_msg;
            while(!consumerTerminate) {
                unique_lock<mutex> lck(msg_q_lock);
                while (message_queue.empty()) {
                    cond.wait(lck);
                }
                live_msg = popMsgQueue();

                //handle liveMsg from db or route
                handleLiveMsg(live_msg);
            }
        });
    });
    for_each(consumer_threads.begin(), consumer_threads.end(), [](thread *t) {
        t->detach();
    });
    return 0; 
}
