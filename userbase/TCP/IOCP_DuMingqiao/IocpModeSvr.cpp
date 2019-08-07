#include <stdio.h>
#include "IocpModeSvr.h"
#include <MSTcpIP.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#pragma  warning(disable:4800)

DWORD WINAPI ServerWorkerProc(LPVOID lParam)
{
	CIocpModeSvr* pSvr = (CIocpModeSvr*)lParam;
	HANDLE CompletionPort = pSvr->CompletionPort;
	DWORD ByteTransferred;
	LPPER_HANDLE_DATA PerHandleData;
	PPER_IO_OPERATION_DATA PerIoData;
	DWORD RecvByte;
	while (true)
	{
		bool bSuccess = GetQueuedCompletionStatus(CompletionPort,
			&ByteTransferred,
			(LPDWORD)& PerHandleData,
			(LPOVERLAPPED*)& PerIoData,
			INFINITE);
		//退出信号到达，退出线程
		if (ByteTransferred == -1 && PerIoData == NULL)
		{
			return 1L;
		}
		//客户机已经断开连接或者连接出现错误
		if (ByteTransferred == 0 &&
			(PerIoData->OperType == RECV_POSTED || PerIoData->OperType == SEND_POSTED))
		{
			//将该客户端数据删除
			bool bFind = false;
			::EnterCriticalSection(&pSvr->cInfoSection);
			PER_HANDLE_DATA pPerHandleData;
			for (pSvr->ClientList.Reset(); !pSvr->ClientList.EndOfList(); pSvr->ClientList.Next())
			{//遍历
				memset(&pPerHandleData, 0, sizeof(PER_HANDLE_DATA));
				pPerHandleData = pSvr->ClientList.Data();
				if ((pPerHandleData.IpAddr == PerHandleData->IpAddr) &&
					(pPerHandleData.sClient == PerHandleData->sClient))
				{
					bFind = true;
					pSvr->ClientList.DeleteAt();
					break;
				}
			}
			::LeaveCriticalSection(&pSvr->cInfoSection);
			if (bFind)
			{
				//Client退出
				in_addr in_A;
				in_A.S_un.S_addr = PerHandleData->IpAddr;
				printf("\nSocket : %d Disconneted,%d in the ClientList now", PerHandleData->sClient, pSvr->ClientList.ListSize());
				//调用回调函数，通知上层该客户端已经断开
				pSvr->m_pProcessRecvData(PerHandleData->IpAddr,
					PerHandleData->sClient,
					NULL,
					0,
					pSvr->m_pContext);
				//关闭套接口
				closesocket(PerHandleData->sClient);
				GlobalFree(PerHandleData);
				GlobalFree(PerIoData);
			}
			continue;
		}
		//为读操作完成，处理数据
		if (PerIoData->OperType == RECV_POSTED)
		{
			//调用回调函数，处理数据
			pSvr->m_pProcessRecvData(PerHandleData->IpAddr,
				PerHandleData->sClient,
				PerIoData->Buf,
				ByteTransferred,
				pSvr->m_pContext);
			//将源数据置空
			memset(PerIoData->Buf, 0, BUFFER_SIZE);
			ByteTransferred = 0;
			//重置IO操作数据
			unsigned long Flag = 0;
			ZeroMemory(&(PerIoData->OverLapped), sizeof(OVERLAPPED));

			PerIoData->DataBuf.buf = PerIoData->Buf;
			PerIoData->DataBuf.len = BUFFER_SIZE;
			PerIoData->OperType = RECV_POSTED;
			//提交另一个Recv请求
			WSARecv(PerHandleData->sClient,
				&(PerIoData->DataBuf),
				1,
				&RecvByte,
				&Flag,
				&(PerIoData->OverLapped),
				NULL);
		}
		//发送完成，置空缓冲区，释放缓冲区
		if (PerIoData->OperType == SEND_POSTED)
		{
			memset(PerIoData, 0, sizeof(PER_IO_OPERATION_DATA));
			GlobalFree(PerIoData);
			ByteTransferred = 0;
		}

	}
	return 0L;
}

DWORD WINAPI ListenProc(LPVOID lParam)
{
	CIocpModeSvr* pSvr = (CIocpModeSvr*)lParam;
	SOCKET Accept;

	while (true)
	{
		//接收客户的请求
		Accept = WSAAccept(pSvr->ListenSocket, NULL, NULL, NULL, 0);
		//申请新的句柄操作数据

		unsigned long chOpt;
		*((char*)& chOpt) = 1;


		if (setsockopt(Accept, SOL_SOCKET, SO_KEEPALIVE, (char*)& chOpt, sizeof(chOpt)) != 0)
		{
			printf("setsockopt() error\n", WSAGetLastError());
		}

		// 设置超时详细信息
		tcp_keepalive klive;
		klive.onoff = 1; // 启用保活
		klive.keepalivetime = 1000 * 30; // 30秒探测一次;
		klive.keepaliveinterval = 1000 * 10; // 重试间隔为10秒 Resend if No-Reply
		WSAIoctl
		(
			Accept,
			SIO_KEEPALIVE_VALS,
			&klive,
			sizeof(tcp_keepalive),
			NULL,
			0,
			(unsigned long*)& chOpt,
			0,
			NULL
		);

		LPPER_HANDLE_DATA PerHandleData = (LPPER_HANDLE_DATA) \
			GlobalAlloc(GPTR,
				sizeof(PER_HANDLE_DATA)
			);
		//取得客户端信息
		sockaddr soad;
		sockaddr_in in;
		int len = sizeof(soad);
		if (getpeername(Accept, &soad, &len) == SOCKET_ERROR)
		{
		}
		else {
			memcpy(&in, &soad, sizeof(sockaddr));
		}
		//句柄数据
		PerHandleData->sClient = Accept;
		PerHandleData->IpAddr = in.sin_addr.S_un.S_addr;

		//存储客户信息
		::EnterCriticalSection(&pSvr->cInfoSection);
		pSvr->ClientList.InsertAfter(*PerHandleData);
		::LeaveCriticalSection(&pSvr->cInfoSection);

		printf("\nUserIP: %s ,Socket : %d Connected!%d in the ClientList now", inet_ntoa(in.sin_addr), Accept, pSvr->ClientList.ListSize());

		//关联客户端口到完成端口，句柄数据在此时被绑定到完成端口
		CreateIoCompletionPort((HANDLE)Accept,
			pSvr->CompletionPort,
			(DWORD)PerHandleData,
			0);
		//Io操作数据标志

		PPER_IO_OPERATION_DATA PerIoData = (PPER_IO_OPERATION_DATA) \
			GlobalAlloc(GPTR,
				sizeof(PER_IO_OPERATION_DATA));
		unsigned long  Flag = 0;
		DWORD RecvByte;
		ZeroMemory(&(PerIoData->OverLapped), sizeof(OVERLAPPED));

		PerIoData->DataBuf.buf = PerIoData->Buf;
		PerIoData->DataBuf.len = BUFFER_SIZE;
		PerIoData->OperType = RECV_POSTED;
		//提交首个接收数据请求
		//这时
		//如果客户端断开连接
		//则也可以以接收数据时得到通知	
		WSARecv(PerHandleData->sClient,
			&(PerIoData->DataBuf),
			1,
			&RecvByte,
			&Flag,
			&(PerIoData->OverLapped),
			NULL);
	}
}


CIocpModeSvr::CIocpModeSvr()
{
	IsStart = false;
	m_pContext = NULL;
	ClientList.ClearList();
}

CIocpModeSvr::~CIocpModeSvr()
{

}

//提交发送消息请求，
//如果提交发送消息失败，
//则将导致在工作线程里将目标客户端的连接切断
bool CIocpModeSvr::SendMsg(SOCKET sClient, char* pData, unsigned long Length)
{
	if (sClient == INVALID_SOCKET || pData == NULL || Length == 0 || !IsStart)return false;

	//申请操作键
	PPER_IO_OPERATION_DATA PerIoData = (PPER_IO_OPERATION_DATA) \
		GlobalAlloc(GPTR,
			sizeof(PER_IO_OPERATION_DATA));

	//准备缓冲
	unsigned long  Flag = 0;
	DWORD SendByte;
	ZeroMemory(&(PerIoData->OverLapped), sizeof(OVERLAPPED));
	memcpy(PerIoData->Buf, pData, Length);
	PerIoData->DataBuf.buf = PerIoData->Buf;
	PerIoData->DataBuf.len = Length;
	PerIoData->OperType = SEND_POSTED;
	int bRet = WSASend(sClient,
		&(PerIoData->DataBuf),
		1,
		&SendByte,
		Flag,
		&(PerIoData->OverLapped),
		NULL);
	if (bRet == SOCKET_ERROR && GetLastError() != WSA_IO_PENDING)
	{
		return false;
	}
	else return true;

	return false;
}

bool CIocpModeSvr::SendMsgToAll(char* pData, unsigned long Length)
{
	if (pData == NULL || Length == 0 || !IsStart)return false;

	::EnterCriticalSection(&cInfoSection);
	PER_HANDLE_DATA pPerHandleData;
	for (ClientList.Reset(); !ClientList.EndOfList(); ClientList.Next())
	{//遍历
		memset(&pPerHandleData, 0, sizeof(PER_HANDLE_DATA));
		pPerHandleData = ClientList.Data();
		SendMsg(pPerHandleData.sClient, pData, Length);
	}
	::LeaveCriticalSection(&cInfoSection);
	return true;
}

bool CIocpModeSvr::Init(ProcessRecvData* pProcessRecvData, void* pContext,
	unsigned long iSvrPort)
{
	if (IsStart || pProcessRecvData == NULL)return false;

	m_SvrPort = iSvrPort;

	::InitializeCriticalSection(&cInfoSection);

	m_pProcessRecvData = pProcessRecvData;
	int  bRet = InitNetWork(iSvrPort, HostIpAddr);
	if (bRet == 0)
	{
		m_pContext = pContext;
		IsStart = true;
		return true;
	}
	else
		return false;
}

void CIocpModeSvr::RemoveSocket(SOCKET sClient)
{
	::EnterCriticalSection(&cInfoSection);

	PER_HANDLE_DATA pPerHandleData;
	for (ClientList.Reset(); !ClientList.EndOfList(); ClientList.Next()) //遍历
	{
		memset(&pPerHandleData, 0, sizeof(PER_HANDLE_DATA));
		pPerHandleData = ClientList.Data();
		if (pPerHandleData.sClient == sClient)
		{
			ClientList.DeleteAt();
			break;
		}
	}

	::LeaveCriticalSection(&cInfoSection);
}

void CIocpModeSvr::DisConnectAll()
{
	if (!IsStart)return;

	::EnterCriticalSection(&cInfoSection);

	PER_HANDLE_DATA pPerHandleData;
	for (ClientList.Reset(); !ClientList.EndOfList(); ClientList.Next())
	{
		memset(&pPerHandleData, 0, sizeof(PER_HANDLE_DATA));
		pPerHandleData = ClientList.Data();
		shutdown(pPerHandleData.sClient, 1);
		closesocket(pPerHandleData.sClient);
		ClientList.DeleteAt();
	}

	::LeaveCriticalSection(&cInfoSection);
}

bool CIocpModeSvr::DisConnectClient(SOCKET sClient)
{
	if (!IsStart || sClient == INVALID_SOCKET)return false;

	::EnterCriticalSection(&cInfoSection);

	PER_HANDLE_DATA pPerHandleData;
	for (ClientList.Reset(); !ClientList.EndOfList(); ClientList.Next())
	{
		memset(&pPerHandleData, 0, sizeof(PER_HANDLE_DATA));
		pPerHandleData = ClientList.Data();
		if (pPerHandleData.sClient == sClient)
		{
			shutdown(pPerHandleData.sClient, 1);
			closesocket(pPerHandleData.sClient);
			ClientList.DeleteAt();
			::LeaveCriticalSection(&cInfoSection);
			return true;
		}
	}
	::LeaveCriticalSection(&cInfoSection);
	return false;
}

void CIocpModeSvr::UnInit()
{
	if (!IsStart)return;
	//退出工作线程
	SYSTEM_INFO sys_Info;
	GetSystemInfo(&sys_Info);
	for (int i = 0; i < sys_Info.dwNumberOfProcessors * 2 + 2; i++)
	{
		//寄出退出消息
		PostQueuedCompletionStatus(CompletionPort,
			-1,
			-1,
			NULL);
	}
	//退出侦听线程
	::TerminateThread(ListenThreadHandle, 1L);
	::WaitForSingleObject(ListenThreadHandle, 10);
	CloseHandle(ListenThreadHandle);
	//关闭网络的侦听
	shutdown(ListenSocket, 0);
	closesocket(ListenSocket);
	//切断当前所有连接
	DisConnectAll();
	::DeleteCriticalSection(&cInfoSection);
	m_pProcessRecvData = NULL;
	IsStart = false;
}

int CIocpModeSvr::InitNetWork(unsigned int SvrPort,
	char* pHostIpAddress)
{
	//启动网络
	int Error = 0;
	WSADATA wsaData;
	char Name[100];
	hostent* pHostEntry;
	in_addr rAddr;
	//Net Start Up
	Error = WSAStartup(MAKEWORD(0x02, 0x02), &wsaData);
	if (Error != 0)
	{
		Error = WSAGetLastError();
		strcpy(pHostIpAddress, "");

		printf("\nWSAStartUp Faild With Error: %d", Error);

		return Error;
	}
	//Make Version
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();

		printf("The Local Net Version Is not 2");

		return -1;
	}
	//Get Host Ip
	Error = gethostname(Name, sizeof(Name));
	if (0 == Error)
	{
		pHostEntry = gethostbyname(Name);
		if (pHostEntry != NULL)
		{
			memcpy(&rAddr, pHostEntry->h_addr_list[0], sizeof(struct in_addr));
			strcpy(pHostIpAddress, inet_ntoa(rAddr));
		}
		else
		{
			Error = WSAGetLastError();

			printf("\nGetHostIp faild with Error: %d", Error);

			return Error;

		}
	}
	else
	{
		Error = WSAGetLastError();

		printf("\ngethostname faild with Error: %d", Error);

		return Error;
	}
	if (0 == Error)
	{
		//创建侦听端口
		ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (ListenSocket == INVALID_SOCKET)
		{
			Error = WSAGetLastError();

			printf("\nCreateSocket faild with Error: %d", Error);

			return Error;
		}
	}
	//绑定到目标地址
	if (0 == Error)
	{
		sockaddr_in InternetAddr;
		InternetAddr.sin_family = AF_INET;
		InternetAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		InternetAddr.sin_port = htons(SvrPort);
		if (bind(ListenSocket,
			(PSOCKADDR)& InternetAddr,
			sizeof(InternetAddr)) == SOCKET_ERROR)
		{
			Error = GetLastError();

			printf("\nbind Socket faild with Error: %d", Error);

			return Error;
		}
	}
	//侦听端口上的连接请求
	if (0 == Error)
	{
		if (listen(ListenSocket, 5) == SOCKET_ERROR)
		{
			Error = GetLastError();

			printf("\nlisten Socket faild with Error: %d", Error);

			return Error;
		}
	}
	//创建完成端口句柄
	if (0 == Error)
	{
		CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (CompletionPort == INVALID_HANDLE_VALUE)
		{
			Error = GetLastError();

			printf("\nCreateIoCompletionPort faild with Error: %d", Error);

			return Error;
		}
	}
	//启动工作线程，线程数为CPU处理器数量*2+2
	if (0 == Error)
	{
		SYSTEM_INFO sys_Info;
		GetSystemInfo(&sys_Info);
		for (int i = 0; i < sys_Info.dwNumberOfProcessors * 2 + 2; i++)
		{
			HANDLE ThreadHandle;
			DWORD ThreadID;

			ThreadHandle = CreateThread(NULL,
				0,
				ServerWorkerProc,
				this,
				0,
				&ThreadID);
			if (ThreadHandle == NULL)
			{
				Error = WSAGetLastError();

				printf("\nCreate Server Work Thread faild with Error: %d", Error);

				return Error;
			}
			CloseHandle(ThreadHandle);
		}
	}
	//启动侦听线程
	if (0 == Error)
	{
		DWORD thID;
		ListenThreadHandle = CreateThread(NULL,
			0,
			ListenProc,
			this,
			0,
			&thID);
		if (ListenThreadHandle == NULL)
		{
			Error = WSAGetLastError();

			printf("\nCreate Listen Thread faild with Error: %d", Error);

			return Error;
		}
	}
	return Error;
}
