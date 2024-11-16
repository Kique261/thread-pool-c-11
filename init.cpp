#include "thread_pool/thread_pool.h"
#include <fstream>
#include "timer/timer_container.h"
#include<chrono>
#include <iomanip>
#include <ctime>
#include "mysql_connect_pool/conn_pool.h"
#include <barrier>
#include<IOsystem/epoller.h>

using namespace std;

barrier b1(500);

void print_cur_time(int id) {
    cout << "this id is: " << id << endl;
    auto now = chrono::system_clock::now();
    auto now_time_t = chrono::system_clock::to_time_t(now);
    auto now_tm = *localtime(&now_time_t);
    auto now_ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    b1.arrive_and_drop();
    cout << put_time(&now_tm, "%Y-%m-%d %H:%M:%S")
         << '.' << setfill('0') << setw(3) << now_ms.count() << endl;
}
void add(Mysql_pool* instance,int i){
    auto command=make_shared<Mysql_command>(command_type::INSERT,"jyx","123456");
    cout<<"already build\n";
    b1.arrive_and_wait();
    auto result=instance->command_in(command);
    cout<<(result.get()==1?"success":"failed")<<endl;
    cout<<i<<endl;
}

void make_listen(const char* ip,int port){
    epoller ep(1024);
    int listenfd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in addr;
    addr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&addr.sin_addr);
    addr.sin_port=htons(port);
    bind(listenfd, reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
    listen(listenfd,SOMAXCONN);
    ep.addfd(listenfd,EPOLLIN|EPOLLET);
    ep.wait(1000);
}


int main(){
    // auto t=TimerContainer::getInstance();
     auto p = thread_pool::getInstance();
    // auto sp= Mysql_pool::getInstance();
    // vector<thread> threads;
    // for(int i=0;i<500;i++){
    //     threads.emplace_back(add,sp,i);
    // }
    // for (auto& th : threads) {
    //     if (th.joinable()) {
    //         th.join();
    //     }
    // }
    //sp->shutdown();
    //this_thread::sleep_for(chrono::seconds(5));
    //t->shutdown();
    const char* ip="127.0.0.1";
    p->task_in(make_listen,ip,8080);
    
}