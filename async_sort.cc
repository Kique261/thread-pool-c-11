#include<iostream>
#include<thread>
#include<bits/stdc++.h>

using namespace std;

list<int> quick_sort(list<int> ls){
    if(ls.empty()) return ls;
    list<int> res;
    int const& pivot=*ls.begin();
    res.splice(res.end(),ls,ls.begin());
    auto divide=partition(ls.begin(),ls.end(),[pivot](int const& val){return val<pivot;});
    list<int> low_part;
    low_part.splice(low_part.end(),ls,ls.begin(),divide);
    list<int> high_part;
    future<list<int>> low_sort(async(quick_sort,move(low_part)));
    auto high_sort(quick_sort(move(ls)));
    res.splice(res.end(),high_sort);
    res.splice(res.begin(),low_sort.get());
    return res;
}


int main(){
    list<int> nums={5,7,8,3,2,1,10,4};
    auto res=quick_sort(nums);
    for(auto ch:res){
        cout<<ch<<endl;
    }
}
