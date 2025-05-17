#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"



int main(int argc,char** argv){
    //整个程序启动后，如果要使用mprpc框架，一定要调用框架的初始化函数
    MprpcApplication::Init(argc,argv);

    //演示调用远程发布的rpc方法Login,MprpcChannel重写CallMethod方法
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    //stub.Login(),RpcChannel->RpcChannel::CallMethod() 
    fixbug::GetFriendsListRequest request;
    request.set_userid(10000);
    fixbug::GetFriendsListResponse response;
    //这里转到Channel的CallMethod,实际调用派生类MprpcChannel重写的CallMethod,结果返回到response
    //MprpcController是抽象类，controller用于接受错误信息
    MprpcController controller;
    stub.GetFriendsList(&controller, &request, &response, nullptr);
    if(controller.Failed()){
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else{
        //一次rpc调用完成，读取调用结果
        if(response.result().errcode() == 0){
            //成功
            std::cout<<"rpc GetFriendList response success"<<std::endl;
            int size = response.friends_size();
            for(int i=0;i<size;i++){
                std::cout<<"index:"<<i+1<<response.friends(i)<<std::endl;
            }
        }else{
            std::cout<<"rpc GetFriendList error:"<<response.result().errmsg()<<std::endl;
        }
    }

    return 0;
}