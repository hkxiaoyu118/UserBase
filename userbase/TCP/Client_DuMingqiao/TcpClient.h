#pragma once
#include <WinSock2.h>

typedef void __stdcall ProcessRecvData(char* data, unsigned int dataLength);

class TcpClient
{
public:
	TcpClient(ProcessRecvData* pProcessRecvData);
	~TcpClient();
	bool Init(char* ip, unsigned short port);
	void UnInit();
	bool SendData(char* data, unsigned int length);
	bool IsConnected();//判断客户端是否处于连接状态

protected:
	static void RecvThread(LPVOID args);
private:
	SOCKET m_clientSocket;
	ProcessRecvData* m_pProcessRecvData;
	bool m_connected;
	CRITICAL_SECTION m_criticalSection;
};
