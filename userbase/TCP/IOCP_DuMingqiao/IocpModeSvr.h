#if !defined(AFX_IOCPMODESVR_H__46FFF420_23C3_4356_A88D_AEBDA61EA186__INCLUDED_)
#define AFX_IOCPMODESVR_H__46FFF420_23C3_4356_A88D_AEBDA61EA186__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ClientList.h"
#include <winsock2.h>
#pragma  comment(lib,"ws2_32.lib")

//服务端口
#define SVRPORT 80
//缓冲区大小
#define BUFFER_SIZE 1024 * 4//1024 * 200
//接收数据
#define RECV_POSTED 0
//发送数据
#define SEND_POSTED 1
//单句柄数据
typedef struct _PER_HANDLE_DATA
{
	unsigned long IpAddr;
	SOCKET sClient;
}PER_HANDLE_DATA, * LPPER_HANDLE_DATA;
//IO操作数据
typedef struct _PER_IO_OPERATION_DATA
{
	//重叠结构
	OVERLAPPED OverLapped;
	//数据缓冲区
	WSABUF DataBuf;
	char Buf[BUFFER_SIZE];
	//操作类型表示发送、接收或关闭等
	bool OperType;
}PER_IO_OPERATION_DATA, * PPER_IO_OPERATION_DATA;

//回调处理数据函数原型
typedef void __stdcall ProcessRecvData(unsigned long sIP,
	SOCKET sClient,
	char* pData,
	unsigned long DataLength,
	void* pContext);

DWORD WINAPI ServerWorkerProc(LPVOID lParam);
DWORD WINAPI ListenProc(LPVOID lParam);

//#################完成端口socket###################
class CIocpModeSvr
{
public:
	CIocpModeSvr();
	virtual ~CIocpModeSvr();
public:
	void* m_pContext;
	//初始化
	bool Init(ProcessRecvData* pProcessRecvData, void* pContext, unsigned long iSvrPort = SVRPORT);
	//反初始化
	void UnInit();
	/*  用于发送消息的函数组*/
public:

	bool SendMsg(SOCKET sClient, char* pData, unsigned long Length);
	bool SendMsgToAll(char* pData, unsigned long Length);

	bool DisConnectClient(SOCKET sClient);
	void DisConnectAll();

	void RemoveSocket(SOCKET sClient);
public:
	//获得本地Ip地址
	const char* GetLocalIpAdd() {
		if (IsStart)return HostIpAddr;
		else return NULL;
	}
	//获得服务器使用的端口号
	unsigned short GetSvrPort() {
		if (IsStart)return m_SvrPort;
		else return 0;
	}

protected:
	int InitNetWork(unsigned int SvrPort = SVRPORT,
		char* pHostIpAddress = NULL);

	ProcessRecvData* m_pProcessRecvData;
private:
	//完成句柄
	HANDLE CompletionPort;
	//主机IP
	char  HostIpAddr[32];
	//客户信息列表
	DCLinkedList<PER_HANDLE_DATA> ClientList;
	//客户信息临界保护量
	CRITICAL_SECTION cInfoSection;
	//服务是否已经启动
	bool IsStart;
	//侦听SOCKET
	SOCKET ListenSocket;
	//侦听线程句柄，用于反初始化时销毁侦听线程
	HANDLE ListenThreadHandle;
	//服务端口
	unsigned short m_SvrPort;

	friend DWORD WINAPI ServerWorkerProc(LPVOID lParam);
	friend DWORD WINAPI ListenProc(LPVOID lParam);
};

#endif // !defined(AFX_IOCPMODESVR_H__46FFF420_23C3_4356_A88D_AEBDA61EA186__INCLUDED_)
