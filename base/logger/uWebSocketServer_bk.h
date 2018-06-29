/*#include <map>*/
#include <mutex>
#include <ctime>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>
#include <iostream>
#include <uWS/uWS.h>
#include <algorithm>
#include <condition_variable>  
#include "Singleton.h"
#include "handle_json.h"
#include "DBServConn.h"
#include "RouteServConn.h"
#include "../base/pb/protocol/IM.BaseDefine.pb.h"
#include "../base/pb/protocol/IM.Live.pb.h"
  
using namespace std;
using namespace uWS;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
//using std::chrono::seconds;
#define ROOM_ID_IDX 0
#define USER_IMID_IDX 1

#define HEARTBEAT_INTERVAL 10000     //ms
#define FIRST_LOGIN_REQUEST_INTERVAL 15000     //ms

typedef uWS::WebSocket<uWS::SERVER> * p_ws;
class ws_info;
typedef map<uint32_t, ws_info> imid_ws_map;
typedef map<uint32_t, imid_ws_map>::iterator outer_it;
typedef map<uint32_t, ws_info>::iterator inner_it;

//imid mapped to ws
class ws_info {
public:
    ws_info(): ws(nullptr), validated(false), empty_conn(true), last_heartbeat_timepoint(high_resolution_clock::now()) {}
    ws_info(p_ws ws, bool validated=false, bool empty_conn=true): ws(ws), validated(validated), empty_conn(empty_conn), last_heartbeat_timepoint(high_resolution_clock::now()) {}
    ~ws_info(){}
public:
    // web socket conn
    p_ws ws;
    // if client validated
    bool validated;
    bool empty_conn;
    chrono::high_resolution_clock::time_point last_heartbeat_timepoint;
    //chrono::system_clock::time_point login_timepoint;
};

class uWebSocketServer{
public:
    uWebSocketServer(string host, uint32_t ws_port, uint32_t listener_num, uint32_t consumer_num);
    virtual ~uWebSocketServer();
    string getHost() const { return ws_host; };
    uint32_t getPort() const { return ws_port; };
    uint32_t getListenerNum() const { return listener_num; };
    uint32_t getMsgConsummerNum() const { return consumer_num; };
    void pushMsgQueue(liveMsg&);
    void run();
    void terminated();

private:
    enum USER_CONN_OP_CODE: char {
        ROOM_NOT_EXISTED,
        USER_NOT_EXISTED,
        USER_EXISTED
    } user_conn_op_code;

    bool consumerTerminate;

    const string ws_host;
    const uint32_t ws_port;
    uint32_t listener_num;
    uint32_t consumer_num;

    static bool instanciated;
    //bool consumer_ready;

    vector<p_ws> client_ws_conn_list; 

    // msg which will be sent to client
    vector<liveMsg> message_queue; 

    vector<thread *> listener_threads;
    vector<thread *> consumer_threads;

    //vector<uS::Async *> async_list;
private:
    uint32_t listenerInit();
    uint32_t consumerInit();
    liveMsg popMsgQueue();
    //bool userExisted(uint32_t room_id, uint32_t user_imid);
    //void delUserConn(uint32_t room_id, uint32_t user_imid);
    //void addUserConn(uint32_t room_id, uint32_t user_imid, p_ws ws);
    bool roomExisted(uint32_t room_id);
    void delRoom(uint32_t room_id);
    uint32_t getThreadId();
    vector<std::string> split(const string &s, char delim);
    void handleLiveMsg(liveMsg& msg);
    void handleWsMsg(const char* msg, p_ws ws);
    void kickUser(uint32_t live_id, uint32_t sender_imid);
    void broadcastMsg(uint32_t live_id, uint32_t sender_imid, liveMsg& msg);
};

