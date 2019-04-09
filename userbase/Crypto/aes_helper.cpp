#include "aes_helper.h"
#include "base64.h"
#include "../UserString.h"
//#include "VMProtectSDK.h"

#pragma comment(lib,"Lib/libcrypto.lib")


//ecb aes解密
int aes_decrypt(char* in, unsigned int size, char* key, char* out)
{
	if (!in || !key || !out) return 0;
	AES_KEY aes;
	if (AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
	{
		return 0;
	}
	int len = size, en_len = 0;
	while (en_len < len)
	{
		AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}
	return 1;
}

//ecb aes加密
int aes_encrypt(char* in, unsigned int size, char* key, char* out)
{
	if (!in || !key || !out) return 0;
	AES_KEY aes;
	if (AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
	{
		return 0;
	}
	int len = size, en_len = 0;
	while (en_len < len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制
	{
		AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}
	return 1;
}

void ControlEnLength(std::string &strtext)
{
	unsigned int length = strtext.length();
	if (length % AES_BLOCK_SIZE != 0)
	{
		unsigned int addcount = AES_BLOCK_SIZE -length%AES_BLOCK_SIZE;
		unsigned int newsize = addcount + length;
		strtext.resize(newsize);
	}
}

void ControlDeLength(std::string &strtext)
{
	char *p = (char *)strtext.c_str();
	unsigned int size = 0;
	while (*p != '\0')
	{
		size++;
		p++;
	}
	strtext.resize(size + 1);
}

std::string Encoder(std::string srctext, std::string key)
{
	ControlEnLength(srctext);
	std::string entext;
	entext.resize(srctext.length());
	aes_encrypt((char *)srctext.c_str(), srctext.length(), (char *)key.c_str(), (char *)entext.c_str());//aes加密
	std::string base64text = base64_encode((unsigned char *)entext.c_str(), srctext.length());//base64编码
	return base64text;
}

std::string Decoder(std::string strctext, std::string key)
{
	std::string detext;
	std::string entext = base64_decode(strctext);
	detext.resize(entext.length());
	aes_decrypt((char *)entext.c_str(), entext.length(), (char *)key.c_str(), (char *)detext.c_str());
	ControlDeLength(detext);
	return detext;
}

//cbc加解密
std::string cbcEncoder(std::string strtext, std::string key)
{
	std::string base64text;
	ControlEnLength(strtext);
	std::string entext;
	AES_KEY aesKey;
	if (AES_set_encrypt_key((const unsigned char *)key.c_str(), 128, &aesKey) >= 0)
	{
		entext.resize(strtext.length());
		AES_cbc_encrypt((unsigned char *)strtext.c_str(), (unsigned char *)entext.c_str(), strtext.length(), &aesKey, (unsigned char *)key.c_str(), AES_ENCRYPT);
		base64text = base64_encode((unsigned char *)entext.c_str(), entext.length());//base64编码
	}
	return base64text;
}

std::string cbcDecoder(std::string strtext, std::string key)
{
	std::string result;
	std::string detext;
	std::string entext = base64_decode(strtext);
	detext.resize(entext.length());
	AES_KEY aesKey;
	if (AES_set_decrypt_key((const unsigned char *)key.c_str(), 128, &aesKey) >= 0)
	{
		AES_cbc_encrypt((unsigned char *)entext.c_str(), (unsigned char *)detext.c_str(), entext.length(), &aesKey, (unsigned char *)key.c_str(), AES_DECRYPT);
		//ControlDeLength(detext);
		result = detext;
	}
	return result;
}

int encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key,
	unsigned char* iv, unsigned char* ciphertext)
{
	EVP_CIPHER_CTX* ctx;

	int len;

	int ciphertext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
		return -1;

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		return -1;
	ciphertext_len = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		return -1;
	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

int decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key,
	unsigned char* iv, unsigned char* plaintext)
{
	EVP_CIPHER_CTX* ctx;

	int len;

	int plaintext_len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new()))
		return -1;

	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, iv))
		return -1;

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		return -1;
	plaintext_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		return -1;
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return plaintext_len;
}

std::string ctrEncoder(std::string data, std::string key)
{
	std::string iv = ubase::StrGetRandomStr(16);
	std::string resultData;
	std::string cryptData;
	cryptData.resize(data.length());
	int result = encrypt((unsigned char*)data.c_str(), data.length(), (unsigned char*)key.c_str(), (unsigned char*)iv.c_str(), (unsigned char*)cryptData.c_str());
	if (result != -1)
	{
		resultData.resize(data.length() + iv.size());
		memcpy((char*)resultData.c_str(), iv.c_str(), iv.size());
		memcpy((char*)(resultData.c_str() + iv.size()), cryptData.c_str(), cryptData.length());
		resultData = ubase::BinToHex(resultData);
	}
	return resultData;
}

std::string ctrDecoder(std::string data, std::string key)
{
	std::string resultData;
	std::string binData = ubase::HexToBin(data);
	if (binData.empty() == false)
	{
		std::string iv = binData.substr(0, 16);//前16个字符是
		std::string trueData = binData.substr(16);//后面真正的数据
		resultData.resize(trueData.length());
		int result = decrypt((unsigned char*)trueData.c_str(), trueData.length(), (unsigned char*)key.c_str(), (unsigned char*)iv.c_str(), (unsigned char*)resultData.c_str());
		if (result != -1)
		{
			;
		}
	}
	return resultData;
}