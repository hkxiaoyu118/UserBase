#include "HashHelper.h"

std::string md5(const std::string &srcStr)
{ 
	unsigned char digest[MD5_DIGEST_LENGTH] = { 0 };
	MD5((const unsigned char *)srcStr.c_str(), srcStr.length(), digest);
	char mdString[MD5_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
	}
	return std::string(mdString);
}

std::string sha1(const std::string &srcStr)
{
	unsigned char digest[SHA_DIGEST_LENGTH] = { 0 };
	SHA1((unsigned char*)srcStr.c_str(), srcStr.length(), digest);
	char mdString[SHA_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	}
	return std::string(mdString);
}

std::string sha256(const std::string &srcStr)
{
	unsigned char digest[SHA256_DIGEST_LENGTH] = { 0 };
	SHA256((unsigned char*)srcStr.c_str(), srcStr.length(), digest);
	char mdString[SHA256_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);
	}
	return std::string(mdString);
}