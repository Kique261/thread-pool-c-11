#include "thread_pool/thread_pool.h"
#include <fstream>

using namespace std;

void print(){
    cout<<"printing"<<endl;
}

int add(int x,int y){
    cout << "hello" << endl;
    return x+y;
}

int main(){
    thread_pool p;
    ofstream outfile("out.out");
    for(int i=0;i<500;i++){
        p.task_in(print);
    }
}