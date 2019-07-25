#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <windows.h>

std::string Encoder(std::string srctext, std::string key);
std::string Decoder(std::string strctext, std::string key);
void ControlEnLength(std::string &strtext);
void ControlDeLength(std::string &strtext);
int aes_decrypt(char* in, unsigned int size, char* key, char* out);//����
int aes_encrypt(char* in, unsigned int size, char* key, char* out);//����
std::string cbcEncoder(std::string strtext, std::string key);//AES cbcģʽ����
std::string cbcDecoder(std::string strtext, std::string key);//AES cbcģʽ����
std::string ctrEncoder(std::string data, std::string key);//AES ctrģʽ����
std::string ctrDecoder(std::string data, std::string key);//AES ctrģʽ����