#include "RsaHelper.h"

#define RSA_KEY_LENGTH  2048           //RSA密钥长度

// 函数方法生成密钥对   
void generateRSAKey(std::string strKey[2])
{
	// 公私密钥对    
	size_t pri_len;
	size_t pub_len;
	char *pri_key = NULL;
	char *pub_key = NULL;

	// 生成密钥对    
	RSA *keypair = RSA_generate_key(RSA_KEY_LENGTH, RSA_3, NULL, NULL);

	BIO *pri = BIO_new(BIO_s_mem());
	BIO *pub = BIO_new(BIO_s_mem());

	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);

	// 获取长度    
	pri_len = BIO_pending(pri);
	pub_len = BIO_pending(pub);

	// 密钥对读取到字符串    
	pri_key = (char *)malloc(pri_len + 1);
	pub_key = (char *)malloc(pub_len + 1);

	BIO_read(pri, pri_key, pri_len);
	BIO_read(pub, pub_key, pub_len);

	pri_key[pri_len] = '\0';
	pub_key[pub_len] = '\0';

	// 存储密钥对    
	strKey[0] = pub_key;
	strKey[1] = pri_key;

	// 内存释放  
	RSA_free(keypair);
	BIO_free_all(pub);
	BIO_free_all(pri);

	free(pri_key);
	free(pub_key);
}

//公钥加密
std::string PubEncrypt(const std::string data, const std::string pubKey)
{
	std::string result;
	BIO *bio = NULL;
	RSA *publicRsa = NULL;
	if ((bio = BIO_new_mem_buf((void *)pubKey.c_str(), -1)) != NULL)
	{
		if ((publicRsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL)) != NULL)
		{
			int rsaLen = RSA_size(publicRsa);
			result.resize(rsaLen);
			if (RSA_public_encrypt(data.length(), (const unsigned char*)data.c_str(), (unsigned char*)result.c_str(), publicRsa, RSA_PKCS1_PADDING) < 0)
			{
				//加密失败
			}
			RSA_free(publicRsa);
		}
		BIO_free_all(bio);
	}
	return result;
}

//私钥解密    
std::string PriDecrypt(const std::string cipherText, const std::string priKey)
{
	std::string strRet;
	RSA *rsa = NULL;
	BIO *keybio = NULL;
	keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);

	if (keybio != NULL)
	{
		rsa = PEM_read_bio_RSAPrivateKey(keybio, NULL, NULL, NULL);
		if (rsa != NULL)
		{
			int len = RSA_size(rsa);
			char *decryptedText = (char *)malloc(len + 1);
			memset(decryptedText, 0, len + 1);
			//解密函数  
			int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
			if (ret >= 0)
				strRet = std::string(decryptedText, ret);
			//释放内存  
			free(decryptedText);
			RSA_free(rsa);
		}
		BIO_free_all(keybio);
	}
	return strRet;
}

std::string PriEncrypt(const std::string data, const std::string priKey) 
{
	std::string strRet;
	RSA *rsa = NULL;
	BIO *keybio = NULL;
	keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);

	if (keybio != NULL)
	{
		rsa = PEM_read_bio_RSAPrivateKey(keybio, NULL, NULL, NULL);
		if (rsa != NULL)
		{
			int len = RSA_size(rsa);
			char *decryptedText = (char *)malloc(len + 1);
			memset(decryptedText, 0, len + 1);
			int ret = RSA_private_encrypt(data.length(), (const unsigned char*)data.c_str(), (unsigned char*)decryptedText, rsa, RSA_PKCS1_PADDING);
			if (ret >= 0)
				strRet = std::string(decryptedText, ret);
			//释放内存  
			free(decryptedText);
			RSA_free(rsa);
		}
		BIO_free_all(keybio);
	}
	return strRet;
}

//公钥解密
std::string PubDecrypt(const std::string cipherText, const std::string pubKey)
{
	std::string result;
	BIO *bio = NULL;
	RSA *publicRsa = NULL;
	if ((bio = BIO_new_mem_buf((void *)pubKey.c_str(), -1)) != NULL)
	{
		if ((publicRsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL)) != NULL)
		{
			int rsaLen = RSA_size(publicRsa);
			result.resize(rsaLen);
			if (RSA_public_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)result.c_str(), publicRsa, RSA_PKCS1_PADDING) < 0)
			{
				//加密失败
			}
			RSA_free(publicRsa);
		}
		BIO_free_all(bio);
	}
	return result;
}
