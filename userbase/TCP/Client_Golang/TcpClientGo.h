#pragma once
#include <vector>
#include <string>
#include <WinSock2.h>

typedef void __stdcall FunProcessRecvData(char* data, unsigned int dataLength);

class TcpClientGo
{
public:
	TcpClientGo(FunProcessRecvData* pProcessRecvData, std::string ipAddr, unsigned int port, std::string key = "");
	~TcpClientGo();
	bool Init();//初始化
	void UnInit();//反初始化
	bool SendData(std::string data);//发送数据
	bool IsConnected();//判断客户端是否处于连接状态

protected:
	static void MinitorConnectThread(LPVOID args);//进程TCP连接状态的线程
	static void RecvThread(LPVOID args);//接受消息线程
	static void DealRecvData(LPVOID args);//处理消息线程(设计成两个线程,提高性能)
	static void DealTrueData(LPVOID args);//处理真正的数据的线程
	void AddRecvData(char* data, unsigned int dataLength);//将接收到的原始数据加入到缓存队列
	bool GetRecvData(std::vector<std::string>& data);//将缓存队列中的原始数据全部取出
	void AddTrueData(std::string& trueData);//将解封包后的真实数据添加到缓存队列
	bool GetTrueData(std::vector<std::string>& trueData);//将缓存队列中的真实数据全部取出
	void UnPack(std::string& data);//解封包
	std::string Pack(std::string& data);//封包
	std::string UnCryptData(std::string data);//解密
	std::string CryptData(std::string data);//加密
private:
	SOCKET m_clientSocket;
	bool m_connected;
	std::string m_ipAddr;//IP地址
	unsigned int m_port;//端口号
	std::vector<std::string> m_originalData;//原始包内容
	std::vector<std::string> m_trueData;//解包后的真实数据
	std::string m_aesKey;//AES加密的密钥,如果为空,则不对数据内容进行加密
	std::string m_tmpData;//遗留的包数据
	std::string m_packedHeader;//包头内容
	unsigned int m_packedHeaderLength;//包头内容长度
	CRITICAL_SECTION m_connectCS;//TCP连接状态临界区
	CRITICAL_SECTION m_dataCS;//接受数据临界区
	CRITICAL_SECTION m_trueDataCS;//真实数据临界区
	FunProcessRecvData* m_pProcessRecvData;//用户传入的接受数据的回调函数(stdcall)
};
