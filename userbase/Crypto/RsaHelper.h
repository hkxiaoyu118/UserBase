#pragma once
#include <string>
#include <stdint.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

void generateRSAKey(std::string strKey[2]);//生成公私密钥对
std::string PubEncrypt(const std::string data, const std::string pubKey);//使用公钥加密
std::string PriDecrypt(const std::string cipherText, const std::string priKey);//使用私钥解密
std::string PriEncrypt(const std::string data, const std::string priKey);//使用私钥加密
std::string PubDecrypt(const std::string cipherText, const std::string pubKey);//使用公钥解密

