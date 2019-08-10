#include "TcpClientGo.h"
#include <Ws2tcpip.h>
#include <process.h>
#include "../Common/MyCriticalSection.h"
#include "../Log/log_file.h"
#include "../Integer/UserInteger.h"
#include "../Crypto/aes_helper.h"

#pragma comment(lib,"ws2_32.lib")

TcpClientGo::TcpClientGo(FunProcessRecvData* pProcessRecvData, std::string ipAddr, unsigned int port, std::string key) :m_connected(false)
{
	m_clientSocket = NULL;
	m_pProcessRecvData = pProcessRecvData;
	m_ipAddr = ipAddr;//连接的IP地址
	m_port = port;//连接的端口
	m_aesKey = key;//加密的密钥
	m_packedHeader = "www.yuyincun.com";//封包头标志
	m_packedHeaderLength = m_packedHeader.size();//封包头标志的长度
	::InitializeCriticalSection(&m_connectCS);
	::InitializeCriticalSection(&m_dataCS);
	::InitializeCriticalSection(&m_trueDataCS);

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(sockVersion, &data);

	_beginthread(MinitorConnectThread, 0, this);//连接状态监控/重连
}

TcpClientGo::~TcpClientGo()
{
	UnInit();
	::DeleteCriticalSection(&m_connectCS);
	::DeleteCriticalSection(&m_dataCS);
	::DeleteCriticalSection(&m_trueDataCS);
}

bool TcpClientGo::Init()
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
	serAddr.sin_port = htons(m_port);
	inet_pton(AF_INET, m_ipAddr.c_str(), (void*)& serAddr.sin_addr.S_un.S_addr);
	//与指定IP地址和端口的服务端连接
	if (connect(m_clientSocket, (sockaddr*)& serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		closesocket(m_clientSocket);
		return initResult;
	}

	//创建处理封包数据的线程
	_beginthread(DealRecvData, 0, this);
	//创建处理真实数据的线程
	_beginthread(DealTrueData, 0, this);
	//创建接收消息的线程
	_beginthread(RecvThread, 0, this);

	ubase::MyCriticalSection myCriticalSection(&m_connectCS);
	m_connected = true;
	initResult = true;

	return initResult;
}

void TcpClientGo::UnInit()
{
	if (m_clientSocket != NULL)
		closesocket(m_clientSocket);
	WSACleanup();
}

bool TcpClientGo::SendData(std::string data)
{
	bool sendResult = false;
	if (IsConnected() == true)//每次发送数据都要验证客户端和工作台的连接状态
	{
		std::string packedData = Pack(data);//封包
		int iResult = send(m_clientSocket, packedData.c_str(), packedData.length(), 0);
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

void TcpClientGo::MinitorConnectThread(LPVOID args)
{
	TcpClientGo* p = (TcpClientGo*)args;
	unsigned int count = 0;
	while (true)
	{
		if (p->IsConnected() == false)//如果断开了连接,则重新连接
		{
			if (count == 60)
			{
				LOG(LogError, "Tcp断开,重新连接...");
				count = 0;
			}
			else
				count++;

			bool reConResult = p->Init();//重新初始化
			if (reConResult == true)
			{
				LOG(LogInfo, "TCP重新连接成功...");
				count = 60;
			}
		}
		Sleep(500);//500ms监视一次
	}
}

void TcpClientGo::RecvThread(LPVOID args)
{
	TcpClientGo* p = (TcpClientGo*)args;
	while (true)
	{
		char buf[1024 * 4] = { 0 }; //4K数据长度,windows经验值
		bool connected = true;
		int ret = recv(p->m_clientSocket, buf, 1024 * 4, 0);
		if (ret > 0 && ret <= 1024 * 4)//包长度限制
		{
			//buf[ret] = 0;
			p->AddRecvData(buf, ret);
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
			ubase::MyCriticalSection myCriticalSection(&p->m_connectCS);
			p->m_connected = false;
			//网络断开,并设置客户端socket为空
			closesocket(p->m_clientSocket);
			p->m_clientSocket = NULL;
			break;
		}
	}
	LOG(LogError, "TcpClientGo::RecvThread TCP断开,线程退出");
}

//判断客户端是否连接
bool TcpClientGo::IsConnected()
{
	ubase::MyCriticalSection myCriticalSection(&m_connectCS);
	return m_connected;
}

void TcpClientGo::DealRecvData(LPVOID args)
{
	TcpClientGo* p = (TcpClientGo*)args;
	while (true)
	{
		if (p->IsConnected() == true)
		{
			std::vector<std::string> vtData;
			bool result = p->GetRecvData(vtData);//一次性取出所有数据
			if (result == true)
			{
				for (std::vector<std::string>::iterator iter = vtData.begin(); iter != vtData.end(); iter++)
				{
					p->UnPack(*iter);//处理数据
				}
			}
		}
		else
		{
			LOG(LogWarning, "TCP链接断开,封包数据处理线程退出...");
			break;
		}

		Sleep(50);
	}
}

void TcpClientGo::DealTrueData(LPVOID args)
{
	TcpClientGo* p = (TcpClientGo*)args;
	while (true)
	{
		if (p->IsConnected() == true)
		{
			std::vector<std::string> vtTrueData;
			bool result = p->GetTrueData(vtTrueData);//一次性取出所有数据
			if (result == true)
			{
				for (std::vector<std::string>::iterator iter = vtTrueData.begin(); iter != vtTrueData.end(); iter++)
				{
					//1.将数据解密
					//2.将数据通过回调函数传递给调用者
					std::string message = p->UnCryptData(*iter);
					p->m_pProcessRecvData((char*)message.c_str(), message.length());
				}
			}
		}
		else
		{
			LOG(LogWarning, "TCP链接断开,真实数据处理线程退出...");
			break;
		}

		Sleep(50);
	}
}

void TcpClientGo::AddRecvData(char* data, unsigned int dataLength)
{
	ubase::MyCriticalSection mtp(&m_dataCS);
	std::string strData;
	strData.resize(dataLength);
	memcpy((char *)strData.c_str(), data, dataLength);
	m_originalData.push_back(strData);
}

bool TcpClientGo::GetRecvData(std::vector<std::string>& data)
{
	ubase::MyCriticalSection mtp(&m_dataCS);
	bool result = false;
	data = m_originalData;
	m_originalData.clear();
	if (data.size() != 0)
		result = true;
	return result;
}

void TcpClientGo::AddTrueData(std::string& trueData)
{
	ubase::MyCriticalSection mtp(&m_trueDataCS);
	m_trueData.push_back(trueData);
}

bool TcpClientGo::GetTrueData(std::vector<std::string>& trueData)
{
	ubase::MyCriticalSection mtp(&m_trueDataCS);
	bool result = false;
	trueData = m_trueData;
	m_trueData.clear();
	if (trueData.size() != 0)
		result = true;
	return result;
}

#define CONST_SAVEDATALENGTH 4 //存储数据长度的字节数

void TcpClientGo::UnPack(std::string& data)
{
	std::string dealData = m_tmpData + data;//这句有问题
	unsigned int length = dealData.length();
	int i = 0;
	for (i = 0; i < length; i++)
	{
		//验证包长度的合法性
		if (length < i + m_packedHeaderLength + CONST_SAVEDATALENGTH)
		{
			break;
		}
		//验证包头标志是否合法
		if (strncmp(dealData.c_str(), m_packedHeader.c_str(), m_packedHeaderLength) == 0)
		{
			//获取实际数据的长度
			unsigned int messageLength = ubase::ConvertBytesToUInt((unsigned char*)(dealData.c_str() + m_packedHeaderLength), true);
			//验证包的长度是否合法
			if (length < i + m_packedHeaderLength + CONST_SAVEDATALENGTH + messageLength)
			{
				break;
			}
			//获取真正的数据
			std::string trueData;
			trueData.resize(messageLength);
			memcpy((char*)trueData.c_str(), (char*)(dealData.c_str() + m_packedHeaderLength + CONST_SAVEDATALENGTH), messageLength);
			//通过消息内容缓存队列传出
			AddTrueData(trueData);
			//移动数据指针到新的位置
			i += m_packedHeaderLength + CONST_SAVEDATALENGTH + messageLength - 1;
		}
	}
	if (i == length)
	{
		m_tmpData = "";
		m_tmpData.resize(0);
	}
	else
	{
		m_tmpData = "";
		m_tmpData.resize(length - i);//剩余数据的长度
		memcpy((char*)m_tmpData.c_str(), (char*)(dealData.c_str() + i), length - i);//将剩余的数据保存
	}
}

std::string TcpClientGo::Pack(std::string& data)
{
	std::string result;
	unsigned char binLength[CONST_SAVEDATALENGTH] = { 0 };
	std::string cryptedData = CryptData(data);
	ubase::ConvertUIntToBytes(cryptedData.length(), binLength, true);
	result.resize(m_packedHeaderLength + CONST_SAVEDATALENGTH + cryptedData.length());//封包长度
	memcpy((char*)result.c_str(), m_packedHeader.c_str(), m_packedHeaderLength);//填充包头标志
	memcpy((char*)(result.c_str() + m_packedHeaderLength), binLength, CONST_SAVEDATALENGTH);//填充数据长度4字节
	memcpy((char*)(result.c_str() + m_packedHeaderLength + CONST_SAVEDATALENGTH), cryptedData.c_str(), cryptedData.length());//填充包数据
	return result;
}

std::string TcpClientGo::UnCryptData(std::string data)
{
	std::string result;
	if (m_aesKey.empty() == false)
	{
		result = ctrDecoder(data, m_aesKey);
	}
	else
	{
		result = data;
	}
	return result;
}

std::string TcpClientGo::CryptData(std::string data)
{
	std::string result;
	if (m_aesKey.empty() == false)
	{
		result = ctrEncoder(data, m_aesKey);
	}
	else
	{
		result = data;
	}
	return result;
}