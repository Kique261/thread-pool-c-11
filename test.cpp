#include "thread_pool/thread_pool.h"
#include <fstream>
#include "timer/timer_container.h"
#include<chrono>
#include <iomanip>
#include <ctime>
#include "mysql_connect_pool/conn_pool.h"
#include <barrier>

using namespace std;

barrier b1(10);

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
void add(Mysql_pool* instance){
    auto command=make_shared<Mysql_command>(command_type::INSERT,"jyx","123456");
    auto result=instance->command_in(command);
    cout<<(result.get()==1?"success":"failed")<<endl;
}


int main(){
    //auto t=TimerContainer::getInstance();
    //auto p = thread_pool::getInstance();
    auto sp= Mysql_pool::getInstance();
    for(int i=0;i<50;i++){
        add(sp);
    }
    this_thread::sleep_for(chrono::seconds(5));
    t->shutdown();
    
}