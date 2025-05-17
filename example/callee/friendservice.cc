#include <iostream>
#include <string>
#include "../friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class FriendService:public fixbug::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid){
        std::cout<<"do GetFriendList service,userid:"<<userid<<std::endl;
        std::vector<std::string> vec;
        vec.push_back("gaoyang");
        vec.push_back("zhangsan");
        vec.push_back("wangshuo");
        return vec;
    }

    void GetFriendsList(::google::protobuf::RpcController* controller,
        const ::fixbug::GetFriendsListRequest* request,
        ::fixbug::GetFriendsListResponse* response,
        ::google::protobuf::Closure* done){
            uint32_t userid  = request->userid();
            std::vector<std::string> friendslist = GetFriendsList(userid);

            response->mutable_result()->set_errcode(0);
            response->mutable_result()->set_errmsg("");
            for(std::string &name : friendslist){
                std::string * p = response->add_friends();
                *p = name;
            }
            done->Run();            
        }
private:
};

int main(int argc,char** argv){
    //大框架
    MprpcApplication::Init(argc,argv);
    //服务提供者
    RpcProvider provider;
    //注册服务
    provider.NotifyService(new FriendService());
    //启动服务
    provider.Run();
    return 0;
}