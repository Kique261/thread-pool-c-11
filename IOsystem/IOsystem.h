#include "heads.h"
class IOSystem{
public:
    IOSystem();
    ~IOSystem();

private:
    bool init_listen();
    static bool setnonblock(int fd);
    void init_event_mode(int type);
    void working();
    void deal_listen();
    void accept_connect(int fd,sockaddr_in addr);
    void deal_read(); //参数为http状态机处理类；还未写；
    void deal_write();
    void extent_time();
    void close_connect();
    void send_error();
    void on_read();
    void on_write();
    void on_process();
};