#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

//全局watcher，zkserver给zkclient的通知，单独线程
void global_watcher(zhandle_t *zh, int type, 
    int state, const char *path,void *watcherCtx)
{
    if(type == ZOO_SESSION_EVENT){//回调消息类型
        if(state == ZOO_CONNECTED_STATE){//连接成功
            //如果连接成功，zkclient调用watcher，给sem加一，client继续运行
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}
ZkClient::ZkClient():m_zhandle(nullptr){
}
ZkClient::~ZkClient(){
    if(m_zhandle!=nullptr){
        zookeeper_close(m_zhandle);//关闭句柄
    }
}
//连接zkserver
void ZkClient::Start(){
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr = host + ":"+port;
    /*
    zookeeper_mt多线程版本
    zookeeper提供三个线程
    api调用线程
    网络io线程 poll无需高并发
    watcher线程
    */
    m_zhandle = zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_zhandle == nullptr){
        std::cout<<"zookeeper init error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle,&sem);
    //zkclient向zkserver请求时，由于是异步的，因此发送连接请求后阻塞在sem上
    sem_wait(&sem);
    std::cout<<"zookeeper init success"<<std::endl;

}
//在zkserver上根据指定的path创建znode节点
void ZkClient::Create(const char* path, const char* data, int datalen, int state){
    char path_buffer[128];
    int bufferlen=sizeof(path_buffer);
    //首先判断是否存在节点
    int flag = zoo_exists(m_zhandle,path, 0, nullptr);
    if(flag == ZNONODE){//不存在
        //state是ZOO_EPHEMERAL就是临时节点，
        flag = zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,path_buffer,bufferlen);
        if(flag==ZOK){
            std::cout<<"znode create success...path"<<path<<std::endl;
        }else{
            std::cout<<"flag:"<<flag<<std::endl;
            std::cout<<"znode create error...path"<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
}
//根据参数指定的znode节点路径，获取节点的值
std::string ZkClient::GetData(const char* path){
    char buffer[64];
    int bufferlen=sizeof(buffer);
    int flag = zoo_get(m_zhandle,path,0,buffer,&bufferlen,nullptr);
    if(flag != ZOK){
        std::cout<<"znode get error...path"<<path<<std::endl;
        return "";
    }else{
        return buffer;
    }
}