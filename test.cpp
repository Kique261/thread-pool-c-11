#include "thread_pool/thread_pool.h"
#include <fstream>
#include "timer/timer_container.h"
#include<chrono>
#include <iomanip>
#include <ctime>

using namespace std;

void print_cur_time() {
    auto now = chrono::system_clock::now();
    auto now_time_t = chrono::system_clock::to_time_t(now);
    auto now_tm = *localtime(&now_time_t);
    auto now_ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    cout << put_time(&now_tm, "%Y-%m-%d %H:%M:%S")
         << '.' << setfill('0') << setw(3) << now_ms.count() << endl;
}
int add(int x,int y){
    cout <<x<< " add " <<y<< endl;
    return x+y;
}

int main(){
    auto& t=TimerContainer::getInstance();
    t.add_timer(10000,print_cur_time);
    t.add_timer(500,print_cur_time);
    t.add_timer(10,print_cur_time);
    sleep(10);
    t.shutdown();
}