#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <openssl/aes.h>
#include <windows.h>

std::string Encoder(std::string srctext, std::string key);
std::string Decoder(std::string strctext, std::string key);
void ControlEnLength(std::string &strtext);
void ControlDeLength(std::string &strtext);
int aes_decrypt(char* in, unsigned int size, char* key, char* out);//加密
int aes_encrypt(char* in, unsigned int size, char* key, char* out);//解密
std::string cbcEncoder(std::string strtext, std::string key);//AES cbc模式加密
std::string cbcDecoder(std::string strtext, std::string key);//AES cbc模式解密
