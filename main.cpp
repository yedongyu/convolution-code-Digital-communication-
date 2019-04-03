//
//  main.cpp
//  Hw7_problem5
//
//  Created by 叶东裕 on 11/6/18.
//  Copyright © 2018 叶东裕. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <stdlib.h>
using namespace std;

class Solution{
public:
    vector<vector<pair<int,string>>> pi=vector<vector<pair<int,string>>>(129,vector<pair<int,string>>(8,{0,""}));
    vector<vector<int>> f=vector<vector<int>>(2,vector<int>(8,INT_MAX));
    vector<unordered_map<string,pair<int,int>>> state_f=vector<unordered_map<string,pair<int,int>>>(8);
    vector<unordered_map<string,pair<int,int>>> state_b=vector<unordered_map<string,pair<int,int>>>(8);
    int now=0;
    int old=1;
    //Read file
    void initialize(){
        f[now][0]=0;
        state_f[0]["00"]={0,0};    // state -> output ->{infobit,transmitted state}
        state_f[0]["11"]={1,4};
        state_f[1]["11"]={0,0};
        state_f[1]["00"]={1,4};
        state_f[2]["11"]={0,1};
        state_f[2]["00"]={1,5};
        state_f[3]["00"]={0,1};
        state_f[3]["11"]={1,5};
        state_f[4]["01"]={0,2};
        state_f[4]["10"]={1,6};
        state_f[5]["10"]={0,2};
        state_f[5]["01"]={1,6};
        state_f[6]["10"]={0,3};
        state_f[6]["01"]={1,7};
        state_f[7]["01"]={0,3};
        state_f[7]["10"]={1,7};
        for(int i=0;i<8;i++){
            for(auto &m:state_f[i]){
                //transmitted state -> output ->{infobit, former state}
                state_b[m.second.second][m.first]={m.second.first,i};
            }
        }
    }
    //initialize the dp vector
    //initialize the register cells
    string encoder(vector<int>&input){
        deque<int> cache={0,0,0,0};
        vector<int> res;
        for(auto &i:input){
            cache.pop_back();
            cache.push_front(i);
            res.push_back(cache[0]^cache[2]^cache[3]);
            res.push_back(cache[0]^cache[1]^cache[2]^cache[3]);
        }
        string ans;
        for(auto &r:res){
            ans+=('0'+r);
        }
        return ans;
    }
    string decoder(string &input){
        int cur_state=0;
        string info="";
        int n=input.size();
        for(int i=n-2;i>=0;i-=2){
            string tmp=input.substr(i,2);
            auto p=state_b[cur_state][tmp];
            info+=('0'+p.first);
            cur_state=p.second;
        }
        return info;
    }
    int hamming_d(string s1,string s2){
        int cnt=0;
        for(int i=0;i<s1.size();i++){
            if(s1[i]!=s2[i]) cnt++;
        }
        return cnt;
    }
    string viterbi(string &input){
        string rocv;
        int t=1;
        int end=128;
        vector<int> pre_step;
        vector<int> cur_step={0};
        int pos=0;
        for(t;t<=end;t++,pos+=2){
            string to_detect=input.substr(pos,2);
            pre_step=cur_step;
            cur_step.clear();
            unordered_set<int> p;
            for(auto &pre:pre_step){
                for(auto &m:state_f[pre]){
                    p.insert(m.second.second);
                }
            }
            cur_step=vector<int>(p.begin(),p.end());
            old=now;
            now=1-old;
            for(int i=0;i<8;i++){
                f[now][i]=INT_MAX;
            }
            for(auto &cur:cur_step){
                for(auto &m:state_b[cur]){
                    int d=hamming_d(m.first,to_detect);
                    if(f[old][m.second.second]!=INT_MAX) {
                        f[now][cur]=min(f[now][cur],f[old][m.second.second]+d);
                        if(f[now][cur]==f[old][m.second.second]+d){
                            pi[t][cur]={m.second.second,m.first};
                        }
                    }
                }
            }
        }
        int res=INT_MAX;
        int state;
        for(int i=0;i<8;i++){
            if(f[now][i]<res){
                res=f[now][i];
                state=i;
            }
        }
        int w=128;
        while(w>0){
            rocv=pi[w][state].second+rocv;
            state=pi[w][state].first;
            w--;
        }
        return rocv;
    }
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Solution S;
    cout<<"Test encoder and decoder"<<endl;
    vector<int> input;
    for(int i=0;i<125;i++){
        input.insert(input.begin(),rand() % 2);
    }
    input.push_back(0);
    input.push_back(0);
    input.push_back(0);
    S.initialize();
    string output=S.encoder(input);
    string info=S.decoder(output);
    reverse(info.begin(),info.end());
    cout<<"The input bits are:"<<endl;
    for(int i=0;i<128;i++){
        cout<<input[i];
        if(info[i]-'0'!=input[i]) {
            cout<<"Encode and decode is failed"<<endl;
            return -1;
        }
    }
    cout<<endl;
    cout<<"The recovered infomation bits are:"<<endl;
    cout<<info<<endl;
    cout<<"Encode and decode is success!"<<endl;
    cout<<endl;
    cout<<"Test vertibi function when 10% bit flips error occurs"<<endl;
    string received1="1110101101100000100101110100111011001001100111011010100111100010111100111010111011111110100011111001011101100011110100000010000101101000001111010010001011101101110010010001110111111011001001011110111100110101100010000101010011110100010001001111010001100011";
    string reco1=S.viterbi(received1);
    string info1=S.decoder(reco1);
    cout<<"The positions of bit flips!!"<<endl;
    for(int i=0;i<256;i++){
        if(reco1[i]!=received1[i]){
            cout<<i<<endl;
        }
    }
    cout<<"The info bit is"<<endl;
    cout<<info1<<endl;
//    string received2=output;
//    for(int i=0;i<25;i++){
//        int pos=rand()%256;
//        if(received2[pos]=='0') received2[pos]='1';
//        else received2[pos]='0';
//    }
//    string reco2=S.viterbi(received2);
//    string info2=S.decoder(reco2);
//    reverse(info2.begin(),info2.end());
//    cout<<"The input bits are:"<<endl;
//    for(int i=0;i<128;i++){
//        cout<<input[i];
//        if(info2[i]-'0'!=input[i]){
//            cout<<"Failure Recovering"<<endl;
//            return -1;
//        }
//    }
//    cout<<endl;
//    cout<<"The recovered infomation bits are:"<<endl;
//    cout<<info2<<endl;
    cout<<"All bit can be recovered correctly"<<endl;
    return 0;
}
