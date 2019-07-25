// Network utility function

#ifndef BASE_NETWORK_NETWORK_UTIL_H_
#define BASE_NETWORK_NETWORK_UTIL_H_

#include "nio_base.h"
#include <vector>
#include <list>

namespace ThirdParty 
{
// 获取本机IP地址
bool GetIpAddressList(std::vector<uint32_t> &ip_addresses);
bool GetIpAddressList(std::vector<std::string> &ip_addresses);
    
// 获取本机MAC地址
bool GetMacAddress(std::string &mac_address);
    
// 是否内网IP
bool IsInternalIP(const uint32_t ip);
bool IsInternalIP(const std::string &ip);
    
// 字符串IP地址转换成Number地址
uint32_t InetStringToNumber(const char *ip);
uint32_t InetStringToNumber(const std::string &ip);
    
// 字符串IP地址转换成Number地址
std::string InetNumberToString(uint32_t ip);
    
// 把IP地址和端口拼成字符串
std::string MakeNetAddress(const uint32_t ip, const uint16_t port);
std::string MakeNetAddress(const std::string &ip, const uint16_t port);
    
// 从地址字符串中获取IP地址和端口    
bool AnalyzeNetAddress(const std::string &address, uint32_t &out_ip, uint16_t &out_port);
}  // namespace nbase

#endif  // BASE_NETWORK_NETWORK_UTIL_H_
