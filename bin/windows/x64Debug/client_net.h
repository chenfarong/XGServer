#ifndef rsnet_h__
#define rsnet_h__

/*
网络层职责说明
1）提供网络通信加密
2）网络通信数据包完整性
3）网络消息队列
*/
#include <stdint.h>

//#include "RsDefine.h"

#define _USE_LIBARY  1

#ifdef RSNET_EXPORTS
#define RSNET_API __declspec(dllexport)
#else
#if(_USE_LIBARY==0)
#define RSNET_API __declspec(dllimport)
#else
#define RSNET_API 	
#endif

#endif


#define __WINDOWS__  1

#if defined(__WINDOWS__)

#include <ws2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

typedef  SOCKET  rsnet_socket;

#else

#endif

#include <stdint.h>


enum RSNET_HTYPE
{
	HT_EVENT,
	HT_MSGPROC,
};


enum RSNET_EVENT
{
	EV_ERROR,					//错误
	EV_CONNECTED=100,			//网络连接成功
	EV_DISCONNECT,				//网络连接断开
	EV_NOTICE=200,				//通知
	EV_LOGIN_PASS=300,			//登录通过
	EV_LOGIN_E_PCODE,			//协议版本太低
	EV_LOGIN_E_APP,				//非法应用
	EV_LOGIN_PASS_TO_ENTRY,		//验证通过并且重新连接到接入服务器
	EV_LOGIN_TOKEN_PASS,		//令牌检查 这个在连接到接入服务器的时候会得到 参数2=0通过 其他值不通过
};

enum RSNET_SERVER_TYPE
{
	ST_UNKNOW,	//未知
	ST_LOGIN,	//登录服务器
	ST_ENTRY,	//接入服务器
};


//回调函数参数
//消息号 数据 数据长度
typedef void(*RS_MSG_HANDLER)(uint16_t, char*, int);

/**事件回调函数
@param 事件
@param 根据事件不同不同
*/
typedef void(*RS_EVENT_HANDLER)(int, int);

//socket连接
typedef void(*RS_SOCKET_HANDLER)();
typedef void(*RS_SOCKET_CHANGED_HANDLER)(int); //socket状态改变后回调

/**注册回调函数指针 例如 网络连接 消息处理等
@param HT_EVENT=事件处理 HT_MSGPROC=消息处理
@param 回调函数指针
*/
RSNET_API void rsnet_hook_handler(int, void*);

//注册网络消息处理回调函数
RSNET_API void rsnet_hook_handler_msgproc(void*);

//网络事件 连接 断开 错误 ...
RSNET_API void rsnet_hook_handler_event(void*);

//取消回调函数
RSNET_API void rsnet_unhook_handler(void*);


//返回 0 表示一切正常
//RSNET_API int rsnet_auto_login();

//返回网络连接信号情况
RSNET_API float rsnet_speed_rate();

/**
用帐号和密码登录
@param _pcode 协议版本号
@param _tios  连接超时的秒数 

成功返回
   此账号最后一次连接游戏服务器的角色简讯列表
   返回最后一次连接游戏服务器的连接代理服务器地址

   //登录服务器列表通过这个获得 客户端程序启动的时候获取
   ServerList=http://update.zmq.163.com/serverlist.txt?
*/
RSNET_API int  rsnet_login(const char* _ipAddr,int _port,const char* _username, const char* _password,int _pcode);

/**
获得服务器列表信息 这也可以由登录器进行更新此内容
成功返回 内容长度
失败返回 0
*/
RSNET_API unsigned long  rsnet_server_list(char** _des,const char* _url);


//设置帐号和用户名
//RSNET_API void  rsnet_set_account_username(const char*, const char*);


//连接服务器 IP地址 端口
RSNET_API int  rsnet_connect(const char* ipAddr, int port);

RSNET_API int  rsnet_reconnect();


RSNET_API rsnet_socket  rsnet_connect_ex(const char* ipAddr, int port);

RSNET_API void  rsnet_disconnect();

RSNET_API bool rsnet_is_connected();

RSNET_API bool rsnet_global_set(const char* keyname,...);

RSNET_API bool rsnet_token_pass();

/**
获取当前环境信息

	keyname=service 返回当前连接的服务器IP地址和端口  192.168.50.252:8002
	keyname=stage  返回当前阶段 0 位置 1登录服务器 2接入服务器
*/
RSNET_API bool rsnet_global_get(char* buffer,size_t _bufSize,const char* keyname);

/**
设置是否开启网络层通信加密 请在网络连接之前使用
登录消息发送之前调用
*/
RSNET_API void rsnet_enable_ssl(bool _value);

/**
设置接收数据使用的模式
默认为TRUE
*/
RSNET_API void rsnet_enable_thread(bool _value);

//总共接收了多少个数据包
RSNET_API uint64_t rsnet_get_recv_num();

//总共发送了多少个数据包
RSNET_API uint64_t rsnet_get_send_num();


//RSNET_API bool rsnet_thread_recv_active(bool _val);

//设置网络层阶段
//RSNET_API void rsnet_set_stage(int);

//向当前连接的服务器发送数据
RSNET_API void rsnet_send(const void* lpBuf, unsigned long _size);

RSNET_API void rsnet_send_echo(const void* lpBuf, unsigned long _size);

//这个必须在渲染层上调用 以便网络层处理消息
RSNET_API void rsnet_render_call();

//启动网络层
RSNET_API void rsnet_startup();

//关闭并释放网络层
RSNET_API void rsnet_shutdown();

//设置Socket Tcp 连接超时秒数
RSNET_API int rsnet_set_connect_timeout(int _val);

//库版本信息
RSNET_API const char* rsnet_version();

//返回最后一个错误号
RSNET_API int  rsnet_last_error();

#endif // rsnet_h__
