#pragma once

#ifndef __US_LOG_FILE_H_
#define __US_LOG_FILE_H_

#include <windows.h>
#include <memory>
#include <functional>


//////////////////////////////////////////////////////////////////////////

enum LogOutputLevel         // 日志输出级别
{
	LogStartup = 1,		// 启动信息：描述模块的启动
	LogShutdown = 2,		// 关闭信息：描述模块的关闭
	LogInfo = 4,		// 普通信息：常态打印，包括启动/停止信息...
	LogWarning = 8,		// 警告信息：程序运行警告状态
	LogTrace = 16,		// 跟踪信息：业务过程跟踪
	LogError = 32,		// 错误信息：不影响程序运行的错误
	LogDebug = 64,		// 调试信息：只用于Debug版本
	LogFatal = 128       // 致命信息：导致程序无法继续运行的错误
};

enum LogOutputType          // 日志输出类型
{
	LogScreen = 1,			// 屏幕输出
	LogVSDebugWindow = 2,	// VC的DebugWindow输出
	LogDiskFile = 4,		// 文件输出
};

// 日志模块对外常用接口
#define InitLog         us::LogFile::Instance().Init
#define UninitLog       us::LogFile::Instance().Uninit
#define GenericLog      us::LogFile::Instance().OutputLog
#define LOG				us::LogFile::Instance().OutputLog
#define ClrLogContent	us::LogFile::Instance().ClearLogContent

//////////////////////////////////////////////////////////////////////////
// 日志实现

namespace us
{
	class LogFile
	{
		struct impl;
		std::unique_ptr<impl> impl_;

	public:
		typedef std::function<std::string(const std::string&)> encrypt_func_t;
		LogFile(void);
		~LogFile(void);

		static LogFile& Instance()
		{
			static LogFile obj;
			return obj;
		}

		/*
		* 功    能：日志初始化
		* 参    数：type：       日志输出类型，默认输出所有类型
		*           level：      日志输出级别，默认输出所有级别
		*           reserve_day：日志存活天数，默认10天
		*           size：       日志最大大小，单位为M，默认1024M
		*           directory：  日志输出目录，默认为当前模块*\\log
		*           name：       日志输出名称，默认为当前模块名称
		* 返 回 值：如果成功则返回true，否则返回false
		* 备    注：如果配置参数为0，则会配置成默认值
		*/
		bool Init(int type, int level, int reserve_day, int size, const char* directory, const char* name, bool is_encrypt = false);

		void SetEncryptHandle(encrypt_func_t);

		/*
		* 功    能：日志卸载，释放内部资源
		* 参    数：
		* 返 回 值：
		*/
		void Uninit();

		// 设置输出类型
		void SetOuputType(int type);
		// 设置输出级别
		void SetOuputLevel(int level);
		//清空当前日志文件内容
		void ClearLogContent();
		// 设置日志保存天数
		void SetReserveDay(int reserve_day);

		void OutputLog(int level, const char* format, ...);
		void OutputLog(int level, const wchar_t* format, ...);

		void OutputLog(const char* trace, int level, const char* format, ...);
		void OutputLog(const wchar_t* trace, int level, const wchar_t* format, ...);

		void OutputArgLog(int level, const char* format, va_list argptr);
		void OutputArgLog(int level, const wchar_t* format, va_list argptr);

		void OutputArgLog(const char* trace, int level, const char* format, va_list argptr);
		void OutputArgLog(const wchar_t* trace, int level, const wchar_t* format, va_list argptr);

	private:
		LogFile(const LogFile&);
		LogFile& operator=(const LogFile&);
	};


	//////////////////////////////////////////////////////////////////////////


	// 模块日志
	class ModuleLog
	{
	public:
		ModuleLog(const char* module_name);
		~ModuleLog(void);

		void TraceLog(int level, const char* format, ...) const;
		void TraceLog(int level, const wchar_t* format, ...) const;

	private:
		ModuleLog(void) {}
		ModuleLog(const ModuleLog&);
		ModuleLog& operator=(const ModuleLog&);

		char module_name_[260];
	};


}// namespace us

#endif // __US_LOG_FILE_H_
