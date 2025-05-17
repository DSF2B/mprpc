#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"



int main(int argc,char** argv){
    //整个程序启动后，如果要使用mprpc框架，一定要调用框架的初始化函数
    MprpcApplication::Init(argc,argv);

    //演示调用远程发布的rpc方法Login,MprpcChannel重写CallMethod方法
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    //stub.Login(),RpcChannel->RpcChannel::CallMethod() 
    fixbug::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("123");
    fixbug::LoginResponse response;
    //这里转到Channel的CallMethod,实际调用派生类MprpcChannel重写的CallMethod,结果返回到response
    stub.Login(nullptr, &request, &response, nullptr);
    //一次rpc调用完成，读取调用结果
    if(response.result().errcode() == 0){
        //成功
        std::cout<<"rpc login response:"<<response.success()<<std::endl;
    }else{
        std::cout<<"rpc login error:"<<response.result().errmsg()<<std::endl;
    }

    //注册
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("zhangsan66");
    req.set_pwd("666666");
    fixbug::RegisterResponse rsp;
    stub.Register(nullptr, &req, &rsp, nullptr);
    if(rsp.result().errcode() == 0){
        //成功
        std::cout<<"rpc register response:"<<rsp.success()<<std::endl;
    }else{
        std::cout<<"rpc register error:"<<rsp.result().errmsg()<<std::endl;
    }
    return 0;
}