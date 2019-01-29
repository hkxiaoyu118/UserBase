#include <stdio.h>
#include "FtpUtility.h"

namespace ubase
{
	struct FtpFile
	{
		char *filename;
		FILE *stream;
	};

	FtpUtility::FtpUtility()
	{
		curl_global_init(CURL_GLOBAL_ALL);
	}


	FtpUtility::~FtpUtility()
	{
		curl_global_cleanup();
	}

	static size_t ReadFunc(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		curl_off_t nread;
		size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);
		nread = (curl_off_t)retcode;
		return retcode;
	}

	static size_t WriteFunc(void *buffer, size_t size, size_t nmemb, void *stream)
	{
		return fwrite(buffer,size,nmemb,(FILE *)stream);
	}


	static int GetFileSize(FILE *file)
	{
		int size = 0;
		fseek(file, 0L, SEEK_END);
		size = ftell(file);
		fseek(file, 0L, SEEK_SET);
		return size;
	}

	/* parse headers for Content-Length */
	static size_t GetContentLength(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		int r;
		long len = 0;
		/* _snscanf() is Win32 specific */
		//r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);   
		r = sscanf((const char*)ptr, "Content-Length: %ld\n", &len);
		if (r) /* Microsoft: we don't read the specs */
			*((long *)stream) = len;
		return size * nmemb;
	}

	bool FtpUtility::FtpUpload(std::string ftpUrl,std::string filepath,std::string userkey,unsigned int timeout)
	{
		bool result = false;
		CURL *curl = curl_easy_init();
		FILE *filehandle = fopen(filepath.c_str(), "rb");
		long filesize = GetFileSize(filehandle);
		CURLcode res;

		if (curl)
		{
			curl_easy_setopt(curl,CURLOPT_USERPWD,userkey.c_str());		//用户名密码 user:key
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadFunc);		//读回调函数
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);					//打开上传开关
			curl_easy_setopt(curl,CURLOPT_URL,ftpUrl.c_str());			//设置url地址
			curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,timeout);		//设置超时
			curl_easy_setopt(curl,CURLOPT_FTP_CREATE_MISSING_DIRS,1);	//自动建立缺失的目录
			curl_easy_setopt(curl, CURLOPT_READDATA, filehandle);		//设置文件句柄
			curl_easy_setopt(curl,CURLOPT_INFILESIZE,filesize);			//设置文件大小
			res = curl_easy_perform(curl);

			if (res == CURLE_OK)
			{
				result = true;
			}
			curl_easy_cleanup(curl);
		}
		fclose(filehandle);
		return result;
	}

	bool FtpUtility::FtpDownload(std::string ftpUrl, std::string filepath, std::string userkey, unsigned int timeout)
	{
		bool result = true;
		CURL *curl;
		CURLcode res;

		FILE *filehandle = fopen(filepath.c_str(), "wb");
		if (filehandle == NULL)
		{
			return result;
		}

		curl = curl_easy_init();
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_USERPWD, userkey.c_str());	//用户名密码 user:key
			curl_easy_setopt(curl,CURLOPT_URL,ftpUrl.c_str());			//设置url
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);	//设置写回调
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, filehandle);		
			curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,timeout);		//设置超时
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			if (res != CURLE_OK)
			{
				result = false;
			}
		}
		fclose(filehandle);

		return result;
	}

	bool FtpUtility::FtpDownloadEx(std::string ftpUrl, std::string filepath, std::string userkey, unsigned int timeout)
	{
		bool result = false;
		CURL *curl;
		CURLcode res;
		FILE *filehandle = fopen(filepath.c_str(),"ab+");
		if (filehandle == NULL)
		{
			return result;
		}

		curl_off_t local_file_size = GetFileSize(filehandle);
		int user_resume = local_file_size > 0 ? 1 : 0;
		long filesize = 0;

		curl = curl_easy_init();
		if (curl)
		{
			//设置用户名密码
			curl_easy_setopt(curl, CURLOPT_USERPWD, userkey.c_str());			
			//设置URL
			curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());				
			//设置超时
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, timeout);			
			//设置头处理函数
			curl_easy_setopt(curl,CURLOPT_HEADERFUNCTION,GetContentLength);	
			curl_easy_setopt(curl,CURLOPT_HEADERDATA,&filesize);
			//设置断点续传
			curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, user_resume?local_file_size:0);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, filehandle);
			curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);				
			res = curl_easy_perform(curl);
			//清理
			curl_easy_cleanup(curl);
			if (res == CURLE_OK)
			{
				result = true;
			}
		}
		fclose(filehandle);

		return result;
	}

	bool FtpUtility::FtpDeleteFile(std::string ftpUrl, std::vector<std::string> folderlist, std::string remotepath, std::string userKey)
	{
		bool result = true;
		CURL *curl = NULL;
		CURLcode res = CURLE_GOT_NOTHING;

		curl = curl_easy_init();
		if (curl)
		{
			struct curl_slist *headers = NULL;
			for (auto item = folderlist.begin(); item != folderlist.end(); item++)
			{
				std::string folder = *item;
				std::string command = "CWD " + folder;
				headers = curl_slist_append(headers, command.c_str());
			}
			std::string strtemp = "DELE " + remotepath;
			headers = curl_slist_append(headers, strtemp.c_str());

			curl_easy_setopt(curl, CURLOPT_QUOTE, headers);
			curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_USERPWD, userKey.c_str());
			res = curl_easy_perform(curl);

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
			if (res != CURLE_OK)
			{
				result = false;
			}
		}
		return result;
	}

	std::string GetTempName()
	{
		char tempPath[MAX_PATH] = { 0 };
		::GetTempPathA(MAX_PATH, tempPath);
		std::string strTempPath = tempPath;
		strTempPath += "ftptempzhoujielun";
		return strTempPath;
	}

	bool FtpUtility::FtpGetList(std::string ftpUrl,std::vector<std::string> subfolderlist, std::string userKey, std::vector<std::string> &filelist)
	{
		CURLcode res;
		bool result = false;
		CURL *curl = curl_easy_init();
		FILE *filehandle = NULL;
		std::string tempfile = GetTempName();

		filehandle = fopen(tempfile.c_str(), "wb");
		if (filehandle == NULL)
		{
			return false;
		}

		if (curl)
		{
			struct curl_slist *headers = NULL;
			for (auto item = subfolderlist.begin(); item != subfolderlist.end(); item++)
			{
				std::string subfolder = *item;
				std::string strcommand = "CWD " + subfolder;
				headers = curl_slist_append(headers, strcommand.c_str());
			}
		
			curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
			curl_easy_setopt(curl, CURLOPT_USERPWD, userKey.c_str());
			curl_easy_setopt(curl, CURLOPT_QUOTE, headers);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, filehandle);
			res = curl_easy_perform(curl);
			if (res == CURLE_OK)
			{
				result = true;
			}
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
		}
		fclose(filehandle);

		//从临时文件中获取文件列表
		std::ifstream infile(tempfile.c_str());
		if (!infile.is_open())
		{
			return false;
		}

		std::string filename;
		while (getline(infile, filename))
		{
			int tag = filename.rfind(" ");
			std::string name = filename.substr(tag + 1);
			filelist.push_back(name);
		}

		infile.close();
		return result;
	}

	bool FtpUtility::FtpCreateDir(std::string ftpUrl, std::string userkey, std::string foldername,std::vector<std::string> folderlist)
	{
		CURLcode res;
		bool result = false;
		CURL *curl = curl_easy_init();
		if (curl)
		{
			struct curl_slist *headers = NULL;
			for (auto item = folderlist.begin(); item != folderlist.end(); item++)
			{
				std::string folder = *item;
				std::string command = "CWD " + folder;
				headers = curl_slist_append(headers, command.c_str());
			}
			std::string commandstr = "MKD " + foldername;
			headers = curl_slist_append(headers, commandstr.c_str());

			curl_easy_setopt(curl, CURLOPT_QUOTE, headers);
			curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
			curl_easy_setopt(curl,CURLOPT_USERPWD,userkey.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFunc);
			res = curl_easy_perform(curl);

			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);

			if (res == CURLE_OK)
			{
				result = true;
			}
		}

		return result;
	}
};