#include "TcpClient.h"
#include <Ws2tcpip.h>
#include <process.h>
#include "../Common/MyCriticalSection.h"
#include "../Log/log_file.h"

#pragma comment(lib,"ws2_32.lib")

TcpClient::TcpClient(FuncProcessRecvData* pProcessRecvData) :m_connected(false)
{
	m_clientSocket = 0;
	m_pProcessRecvData = pProcessRecvData;
	::InitializeCriticalSection(&m_criticalSection);//初始化临界区

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(sockVersion, &data);
}


TcpClient::~TcpClient()
{
	UnInit();
	::DeleteCriticalSection(&m_criticalSection);//反初始化临界区
}

bool TcpClient::Init(char* ip, unsigned short port)
{
	bool initResult = false;

	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_clientSocket == INVALID_SOCKET)
	{
		return initResult;
	}

	//设置读写超时时间为3S
	int timeout = 3000;
	int ret = setsockopt(m_clientSocket, SOL_SOCKET, SO_SNDTIMEO, (const char*)& timeout, sizeof(timeout));
	if (ret == -1)
	{
		//设置写超时失败,初始化失败
		closesocket(m_clientSocket);
		return initResult;
	}

	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, (void*)& serAddr.sin_addr.S_un.S_addr);
	//与指定IP地址和端口的服务端连接
	if (connect(m_clientSocket, (sockaddr*)& serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		closesocket(m_clientSocket);
		return initResult;
	}

	//创建接收消息的线程
	_beginthread(RecvThread, 0, this);

	ubase::MyCriticalSection myCriticalSection(&m_criticalSection);
	m_connected = true;
	initResult = true;

	return initResult;
}

void TcpClient::UnInit()
{
	closesocket(m_clientSocket);
	WSACleanup();
}

bool TcpClient::SendData(char* data, unsigned int length)
{
	bool sendResult = false;
	if (IsConnected() == true)//每次发送数据都要验证客户端和工作台的连接状态
	{
		int iResult = send(m_clientSocket, data, length, 0);
		if (iResult == SOCKET_ERROR)
		{
			DWORD errorCode = WSAGetLastError();
			LOG(LogError, "send发送数据发生了错误,错误代码:%d", errorCode);
		}
		else
			sendResult = true;
	}
	return sendResult;
}

void TcpClient::RecvThread(LPVOID args)
{
	TcpClient* p = (TcpClient*)args;
	while (true)
	{
		char buf[1024 * 4] = { 0 }; //4K数据长度,windows经验值
		bool connected = true;
		int ret = recv(p->m_clientSocket, buf, 1024 * 4, 0);
		if (ret > 0 && ret < 1024 * 4)//不能是超大包,超大包直接扔掉
		{
			buf[ret] = 0;
			p->m_pProcessRecvData(buf, ret);
		}
		else if (ret == 0)//网络优雅的断开
		{
			connected = false;
		}
		else if (ret == SOCKET_ERROR)
		{
			DWORD errorCode = WSAGetLastError();
			if (errorCode == WSAENOTCONN || errorCode == WSAECONNRESET)//远程网络强制断开
			{
				connected = false;
				LOG(LogError, "远程网络强制断开连接,错误代码:%d", errorCode);
			}
		}

		if (connected == false)
		{
			//设置客户端没有连接
			ubase::MyCriticalSection myCriticalSection(&p->m_criticalSection);
			p->m_connected = false;
			//网络断开,并设置客户端socket为空
			closesocket(p->m_clientSocket);
			p->m_clientSocket = NULL;
			break;
		}
	}
	LOG(LogError, "TcpClient::RecvThread TCP断开,线程退出");
}

//判断客户端是否连接
bool TcpClient::IsConnected()
{
	ubase::MyCriticalSection myCriticalSection(&m_criticalSection);
	return m_connected;
}