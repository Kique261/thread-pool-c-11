#include "thread_pool/thread_pool.h"
#include <fstream>

using namespace std;

void print(){
    cout<<"printing"<<endl;
}

int add(int x,int y){
    return x+y;
}

int main(){
    thread_pool p;
    ofstream outfile("out.out");
    for(int i=0;i<500;i++){
        cout<<p.task_in(add,i,i+1).get()<<endl;
    }
}