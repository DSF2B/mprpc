#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"



class UserService: public fixbug::UserServiceRpc //rpc服务提供者,派生类实现具体业务，提交给框架中的父类，父类进行序列化码和通信，向外发布本机可以实现什么服务
{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"doing local service : Login"<<std::endl;
        std::cout<<"name" <<name<<"pwd"<<pwd<<std::endl;
        return true;
    }
    void Login(google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
        {
            std::string name=request->name();
            std::string pwd=request->pwd();

            bool login_request = Login(name,pwd);

            fixbug::ResultCode *code = response->mutable_result();
            code->set_errcode(0);
            code->set_errmsg("Success");
            response->set_success(login_request);

            done->Run();

        }
        bool Register(uint32_t id,std::string name,std::string pwd){
            std::cout<<"doing local service : Register"<<std::endl;
            std::cout<<"id:"<<id<<"name:" <<name<<"pwd:"<<pwd<<std::endl;
            return true;
        }
        void Register(google::protobuf::RpcController* controller,
            const ::fixbug::RegisterRequest* request,
            ::fixbug::RegisterResponse* response,
            ::google::protobuf::Closure* done)
            {
                uint32_t id = request->id();
                std::string name=request->name();
                std::string pwd=request->pwd();
    
                bool ret = Register(id,name,pwd);
    
                response->mutable_result()->set_errcode(0);
                response->mutable_result()->set_errmsg("Success");
                response->set_success(ret);

                done->Run();
    
            }
};

int main(int argc, char **argv){
    //首先初始化框架，定义通信对象 provider -i config.conf
    MprpcApplication::Init(argc,argv);//单例模式，初始化整个框架参数
    //服务发布对象,把UserService对象及其对象注册到本地provider服务方法表
    RpcProvider provider;//当前节点
    provider.NotifyService(new UserService());
    //启动rpc服务节点，阻塞进程，等待caller调用相关本地业务，完成本地业务后将返回值提交框架，框架进行网络通信和序列化向zk提交服务返回值
    provider.Run();
    return 0;
}