#include "log_file.h"
#include <string>
#include <vector>
#include <functional>
#include <intrin.h>

namespace us
{

//////////////////////////////////////////////////////////////////////////
namespace
{

const wchar_t HEADER_TIMESTAMP_W[] = L"[%04u-%02u-%02u %02u:%02u:%02u]";
const wchar_t HEADER_THREADID_W[]  = L"|[%04x]";
const wchar_t HEADER_STRING_W[]    = L"|[%s]";
const wchar_t TAIL_STRING_W[]      = L"\0";
const wchar_t TAIL_LINE_W[]        = L"\r\n\0";

const char    HEADER_TIMESTAMP_A[] = "[%04u-%02u-%02u %02u:%02u:%02u]";
const char    HEADER_THREADID_A[]  = "|[%04x]";
const char    HEADER_STRING_A[]    = "|[%s]";
const char    TAIL_STRING_A[]      = "\0";
const char    TAIL_LINE_A[]        = "\r\n\0";

const char    FILE_NAME[]          = "%s\\%s.log";
const char    FILE_POSTFIX[]       = ".log";


const int LOG_NAME_LEN    = 260;        // 名称长度
const int LOG_PATH_LEN    = 512;        // 日志路径长度
const int LOG_BUFFER_LEN  = 1024;       // 日志缓存默认长度
const int LOG_TAIL_LEN    = 3;          // 日志结尾长度
const int MB_UNIT         = 1024*1024;  // 1M单位

const int LOG_DEF_FILE_SIZE = 1024;     // 日志默认大小为1024M
const int LOG_DEF_LIVE_DAY  =  10;      // 日志默认保存10天
const int LOG_DEF_TYPE = LogScreen | LogVSDebugWindow | LogDiskFile;
const int LOG_DEF_LEVEL = LogStartup | LogShutdown | LogInfo | LogTrace | LogDebug | LogWarning | LogError | LogFatal;

//////////////////////////////////////////////////////////////////////////

// 获得2数之间的倍数
unsigned int get_multiple(unsigned int num1, unsigned int num2)
{
    int mult = num1 / num2;
    if (0 != (num1 % num2))
    {
        ++mult;
    }
    return mult;
}

// 宽字符转窄字符
bool wchar_to_mbyte(const wchar_t* wide_str, char* multi_str, DWORD multi_size)
{
    DWORD min_size = WideCharToMultiByte(CP_OEMCP, NULL, wide_str, -1, NULL, 0, NULL, FALSE);
    if(multi_size < min_size)
    {
        return false;
    }
    WideCharToMultiByte(CP_OEMCP, NULL, wide_str, -1, multi_str, multi_size, NULL, FALSE);
    return true;
}

// 窄字符转宽字符
bool mbyte_to_wchar(const char* multi_str, wchar_t* wide_str, int wide_size)
{
    int min_size = MultiByteToWideChar(CP_ACP, 0, multi_str, -1, NULL, 0);
    if(wide_size < min_size)
    {
        return false;
    }
    MultiByteToWideChar(CP_ACP, 0, multi_str, -1, wide_str, min_size);
    return true;
}

// 获得文件长度
bool get_file_size(HANDLE handle, unsigned long long& length)
{
    if (INVALID_HANDLE_VALUE == handle)
    {
        return false;
    }

    ULARGE_INTEGER uiSize;
    uiSize.LowPart = ::GetFileSize(handle, &uiSize.HighPart);
    if (INVALID_FILE_SIZE == uiSize.LowPart)
    {
        if (::GetLastError() != NO_ERROR)
        {
            return false;
        }
    }

    length = uiSize.QuadPart;

    return true;
}

// 创建完整目录
bool create_full_directory(const char* directory)
{
    if (NULL == directory)
    {
        return false;
    }

    char* temp = NULL;
    char* start_pos = NULL;
    char path_name[512] = {0};
    strcpy_s(path_name, sizeof(path_name), directory);

    for (start_pos = path_name; NULL != *start_pos && (start_pos - path_name) < (int)strlen(path_name); start_pos = temp + 1)
    {
        temp = strchr(start_pos, '\\');
        if (temp > path_name && *(temp-1) == ':')
        {
            continue;
        }

        if (temp == path_name && *(temp+1) == '\\')
        {
            temp = strchr(temp+2, '\\');
            if (NULL == temp)
            {
                break;
            }
            continue;
        }

        if (NULL != temp)
        {
            *temp = 0;
            if (!CreateDirectoryA(path_name, NULL))
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    return false;
                }
            }
            *temp = '\\';
        }
        else
        {
            if (path_name[strlen(path_name)-1] != ':' && !CreateDirectoryA(path_name, NULL))
            {
                if (GetLastError() != ERROR_ALREADY_EXISTS)
                {
                    return false;
                }
            }
            break;
        }
    }

    return true;
}

// 搜索目录，并执行回调
bool search_directory(const char* directory, bool is_in_subdir, std::function<bool(const char*, const WIN32_FIND_DATAA&)> filterfile)
{
    char find_path[512] = {0};
    sprintf_s(find_path, sizeof(find_path), "%s\\*", directory);

    WIN32_FIND_DATAA fd = {0};
    HANDLE find_handle = FindFirstFileA(find_path, &fd);
    if (INVALID_HANDLE_VALUE == find_handle)
    {
        return false;
    }

    while (FindNextFileA(find_handle, &fd))
    {
        if (0 == _strcmpi(fd.cFileName, ".") || 0 == _strcmpi(fd.cFileName, ".."))
        {
            continue;
        }

        if ((0 != (fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) ||
                (0 != (fd.dwFileAttributes & FILE_ATTRIBUTE_VIRTUAL)) ||
                (0 != (fd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)))
        {
            continue;
        }

        sprintf_s(find_path, sizeof(find_path), "%s\\%s", directory, fd.cFileName);

        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            if (is_in_subdir && !search_directory(find_path, is_in_subdir, filterfile))
            {
                break;
            }
        }
        else
        {
            if (!filterfile(find_path, fd))
            {
                break;
            }
        }
    }

    FindClose(find_handle);
    return true;
}

// 获得当前模块路径名
void get_module_name(char* module_name, int size)
{
    HMODULE hModule = NULL;
    void* callerAddress = _ReturnAddress();
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (char*)callerAddress, &hModule))
    {
        GetModuleFileNameA(hModule, module_name, size);
    }
}

// 得到模块目录
bool get_log_directory(std::string& dir)
{
    char module_dir[LOG_PATH_LEN] = {0};
    get_module_name(module_dir, LOG_PATH_LEN);
    char* pFind = strrchr(module_dir, '\\');
    if (NULL != pFind)
    {
        *pFind = '\0';
        dir = module_dir;
        dir.append("\\log");
        return true;
    }
    return false;
}

// 得到模块名称
bool get_log_name(std::string& name)
{
    char dir[LOG_PATH_LEN] = {0};
    get_module_name(dir, LOG_PATH_LEN);
    char* pFind = strrchr(dir, '.');
    if (NULL != pFind)
    {
        *pFind = '\0';
        pFind = strrchr(dir, '\\');
        if (NULL != pFind)
        {
            name = (pFind+1);
            return true;
        }
    }
    return false;
}

} // namespace

//////////////////////////////////////////////////////////////////////////

struct LogFile::impl
{
    HANDLE log_handle_;
    CRITICAL_SECTION lock_;

    bool is_encrypt_;				// 是否加密
    int ouput_level_;               // 输出信息级别
    int ouput_type_;                // 日志输出类型
    int reserve_day_;               // 日志保存天数
    int log_size_;                  // 日志大小
    int log_index_;                 // 日志索引
    SYSTEMTIME log_time_;		    // 当前日志时间

    std::string directory_;         // 日志目录
    std::string log_name_;          // 日志名称
    std::vector<char> buffer_a_;    // 日志信息
    std::vector<wchar_t> buffer_w_; // 日志信息

    encrypt_func_t encrypt_func_;	// 加密函数

    //////////////////////////////////////////////////////////////////////////

    impl()
        : log_handle_(INVALID_HANDLE_VALUE)
        , ouput_type_(LOG_DEF_TYPE)
        , ouput_level_(LOG_DEF_LEVEL)
        , reserve_day_(LOG_DEF_LIVE_DAY)
        , log_size_(LOG_DEF_FILE_SIZE)
        , log_index_(0)
        , buffer_a_(LOG_BUFFER_LEN, 0)
        , buffer_w_(LOG_BUFFER_LEN, 0)
        , is_encrypt_(false)
        , encrypt_func_(nullptr)
    {
        ::InitializeCriticalSection(&lock_);
    }
    ~impl()
    {
        uninit();
        ::DeleteCriticalSection(&lock_);
    }

    // 初始化
    bool init(int type, int level, int reserve_day, int size, const char* directory, const char* name, bool is_encrypt)
    {
        if (0 != (type & LOG_DEF_TYPE)) ouput_type_ = type;
        if (0 != (level & LOG_DEF_LEVEL)) ouput_level_ = level;
        if (0 != reserve_day) reserve_day_ = reserve_day;
        if (0 != size) log_size_ = size;
        is_encrypt_ = is_encrypt;

        if (NULL != directory && 0 != strlen(directory))
        {
            directory_ = directory;
        }
        else
        {
            if (!get_log_directory(directory_))
            {
                return false;
            }
        }

        if (NULL != name && 0 != strlen(name))
        {
            log_name_ = name;
        }
        else
        {
            if (!get_log_name(log_name_))
            {
                return false;
            }
        }

        // 创建目录
        if (!create_full_directory(directory_.c_str()))
        {
            return false;
        }

        setlocale(LC_ALL, "");

        // 获得当前时间
        GetLocalTime(&log_time_);

        // 清理老的日志
        clear_log(log_time_);

        // 得到最大索引，如果木有，则默认为0
        log_index_ = get_max_index(log_time_);

        // 打开日志文件
        return create_log(log_time_);
    }

    // 卸载
    void uninit()
    {
        if (INVALID_HANDLE_VALUE == log_handle_)
        {
            CloseHandle(log_handle_);
            log_handle_ = INVALID_HANDLE_VALUE;
        }
    }

    // 输出ANSI
    void output_log(const char* trace, int level, const char* format, va_list argptr)
    {
        if (0 == (ouput_level_ & level))
        {
            return;
        }

        ::EnterCriticalSection(&lock_);

        SYSTEMTIME st;
        GetLocalTime(&st);

        int pos = 0;
        pos += sprintf_s(&buffer_a_[pos],  buffer_a_.size()-pos, HEADER_TIMESTAMP_A, st.wYear,st.wMonth,st.wDay,st.wHour, st.wMinute, st.wSecond);
        pos += sprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, HEADER_THREADID_A, GetCurrentThreadId());
        pos += sprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, HEADER_STRING_A, get_level_string(level));
		if (NULL != trace)
		{
			pos += sprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, HEADER_STRING_A, trace);
		}
        pos += sprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, "%s", " ");

        size_t buffer_size = _vscprintf(format, argptr) + pos + LOG_TAIL_LEN;
        if (buffer_size > buffer_a_.size())
        {
            int mult = get_multiple(buffer_size, LOG_BUFFER_LEN);
            buffer_a_.resize(LOG_BUFFER_LEN*mult);
        }

        pos += _vsnprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, _TRUNCATE, format, argptr);
        sprintf_s(&buffer_a_[pos], buffer_a_.size()-pos, is_encrypt_ ? TAIL_STRING_A : TAIL_LINE_A);

        ouput_by_type();

        buffer_a_.assign(buffer_a_.size(), 0);

        ::LeaveCriticalSection(&lock_);
    }

    // 输出UNICODE
    void output_log(const wchar_t* trace, int level, const wchar_t* format, va_list argptr)
    {
        if (0 == (ouput_level_ & level))
        {
            return;
        }

        ::EnterCriticalSection(&lock_);

        SYSTEMTIME st;
        GetLocalTime(&st);

        int pos = 0;
        pos += swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, HEADER_TIMESTAMP_W, st.wYear,st.wMonth,st.wDay,st.wHour, st.wMinute, st.wSecond);
        pos += swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, HEADER_THREADID_W, GetCurrentThreadId());
        pos += swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, HEADER_STRING_W, get_level_wstring(level));
        if (NULL != trace)
        {
            pos += swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, HEADER_STRING_W, trace);
        }
        pos += swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, L"%s", L" ");

        size_t buffer_size = _vscwprintf(format, argptr) + pos + LOG_TAIL_LEN;
        if (buffer_size > buffer_w_.size())
        {
            int mult = get_multiple(buffer_size, LOG_BUFFER_LEN);
            buffer_w_.resize(LOG_BUFFER_LEN*mult);
            buffer_a_.resize(LOG_BUFFER_LEN*mult);
        }

        pos += _vsnwprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, _TRUNCATE, format, argptr);
        swprintf_s(&buffer_w_[pos], buffer_w_.size()-pos, is_encrypt_ ? TAIL_STRING_W : TAIL_LINE_W);

        wchar_to_mbyte(&buffer_w_[0], &buffer_a_[0], buffer_a_.size());
        ouput_by_type();

        buffer_a_.assign(buffer_a_.size(), 0);
        buffer_w_.assign(buffer_w_.size(), 0);

        ::LeaveCriticalSection(&lock_);
    }

    // 根据输出类型输出
    void ouput_by_type()
    {
        if (ouput_type_ & LogScreen)
        {
            printf_s("%s", buffer_a_.data());
        }
        if (ouput_type_ & LogVSDebugWindow)
        {
            OutputDebugStringA(buffer_a_.data());
        }
        if (ouput_type_ & LogDiskFile)
        {
            output_file();
        }
    }

    // 把日志信息输出到文件
    void output_file()
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        if (st.wDay != log_time_.wDay)
        {
            if (INVALID_HANDLE_VALUE != log_handle_)
            {
                CloseHandle(log_handle_);
                log_handle_ = INVALID_HANDLE_VALUE;
            }

            clear_log(st);

            // 重置时间和索引
            log_time_ = st;
            log_index_ = 0;
        }

        if (INVALID_HANDLE_VALUE == log_handle_)
        {
            create_log(st);
        }

        if (INVALID_HANDLE_VALUE != log_handle_)
        {
            DWORD bytes = 0;

            if (is_encrypt_ && nullptr != encrypt_func_)
            {
                std::string encrypt_data(encrypt_func_(&buffer_a_[0]));
                encrypt_data.append("\r\n");
                ::WriteFile(log_handle_, encrypt_data.c_str(), encrypt_data.length(), &bytes, NULL);
            }
            else
            {
                ::WriteFile(log_handle_, &buffer_a_[0], strlen(&buffer_a_[0]), &bytes, NULL);
            }

            // 如果文件大小到警戒值，则索引加1，关闭当前日志文件，创建新的日志文件
            if (is_file_oversize(log_handle_, log_size_))
            {
                ++log_index_;
                create_log(st);
            }
        }
    }

    // 创建日志文件
    bool create_log(const SYSTEMTIME& st)
    {
        char log_path[LOG_PATH_LEN] = {0};
        sprintf_s(log_path, LOG_PATH_LEN, FILE_NAME, directory_.c_str(), log_name_.c_str()/*, st.wYear, st.wMonth, st.wDay, log_index_*/);

        if (INVALID_HANDLE_VALUE != log_handle_)
        {
            CloseHandle(log_handle_);
            log_handle_ = INVALID_HANDLE_VALUE;
        }

        log_handle_ = CreateFileA(log_path, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,
                                  NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (INVALID_HANDLE_VALUE != log_handle_)
        {
            ::SetFilePointer(log_handle_, 0, NULL, FILE_END);
            return true;
        }
        return false;
    }

	//清空日志文件内容
	bool clear_log_content()
	{
		char log_path[LOG_PATH_LEN] = {0};
		sprintf_s(log_path, LOG_PATH_LEN, FILE_NAME, directory_.c_str(), log_name_.c_str()/*, st.wYear, st.wMonth, st.wDay, log_index_*/);
		if (INVALID_HANDLE_VALUE != log_handle_)
		{
			DWORD dwPtrLow = ::SetFilePointer(log_handle_, 0, NULL, FILE_BEGIN);
			if (dwPtrLow == INVALID_SET_FILE_POINTER && 
				GetLastError() != NO_ERROR)
			{
				return false;
			}
			::SetEndOfFile(log_handle_);
			return true;
		}
		return false;
	}

    // 清理日志文件
    void clear_log(const SYSTEMTIME& st)
    {
        search_directory(directory_.c_str(), false, [&](const char* file_path, const WIN32_FIND_DATAA& fd)->bool
        {
            if (0 == _strnicmp(fd.cFileName + strlen(fd.cFileName) - strlen(FILE_POSTFIX), FILE_POSTFIX, strlen(FILE_POSTFIX)))
            {
                int diff_day = get_diff_days(fd.ftCreationTime, st);
                if (diff_day < 0 || diff_day > reserve_day_)
                {
                    DeleteFileA(file_path);
                }
            }
            return true;
        });
    }

    // 得到当前最大索引
    int get_max_index(const SYSTEMTIME& st)
    {
        int log_index = 0;
        unsigned long long file_size = 0;

        search_directory(directory_.c_str(), false, [&](const char* /*file_path*/, const WIN32_FIND_DATAA& fd)->bool
        {
            std::string filename(fd.cFileName);
            if (filename.size() <= 15)
            {
                return true;
            }

            size_t postfix_pos = filename.find(FILE_POSTFIX, 0);
            if (postfix_pos == std::string::npos)
            {
                return true;
            }

            size_t name_pos = filename.find(log_name_, 0);
            if (name_pos == std::string::npos)
            {
                return true;
            }

            SYSTEMTIME st_file;
            FILETIME ft_file = fd.ftCreationTime;
            FileTimeToSystemTime(&ft_file, &st_file);

            if (st.wYear == st_file.wYear && st.wMonth == st_file.wMonth && st.wDay == st_file.wDay)
            {
                size_t separator = filename.find_last_of('_', postfix_pos) + 1;
                if (separator == std::string::npos || 1 != (postfix_pos - separator))
                {
                    return true;
                }

                std::string sub_str = filename.substr(separator, 1);
                int index = atoi(sub_str.c_str());
                if (log_index < index)
                {
                    log_index = index;
                    file_size = fd.nFileSizeLow + ((unsigned long long)fd.nFileSizeHigh << 32);
                }
            }

            return true;
        });

        if (file_size >= (log_size_*MB_UNIT))
        {
            ++log_index;
        }

        return log_index;
    }

    // 是否文件超过警戒值
    bool is_file_oversize(HANDLE handle, unsigned int size)
    {
        unsigned long long file_size;
        if (!get_file_size(handle, file_size))
        {
            return false;
        }

        if (file_size >= (size*MB_UNIT))
        {
            return true;
        }
        return false;
    }

    // 获得两个时间之间的差别天数
    int get_diff_days(const FILETIME &ftOld, const SYSTEMTIME &stNow)
    {
        FILETIME ftNow;
        SystemTimeToFileTime(&stNow, &ftNow);

        ULARGE_INTEGER uiOldTime, uiNowTime;
        uiOldTime.LowPart = ftOld.dwLowDateTime;
        uiOldTime.HighPart = ftOld.dwHighDateTime;
        uiNowTime.LowPart = ftNow.dwLowDateTime;
        uiNowTime.HighPart = ftNow.dwHighDateTime;

        LONGLONG diff_time = uiNowTime.QuadPart - uiOldTime.QuadPart;
        return int((diff_time / 10000000) / (60*60*24));
    }

    const char* get_level_string(int level)
    {
        static const char* level_string[] = { "Startup", "Shutdown", "Info", "Trace", "Debug", "Warning", "Error", "Fatal", "Unkown" };
        switch (level)
        {
        case LogStartup:
            return level_string[0];
        case LogShutdown:
            return level_string[1];
        case LogInfo:
            return level_string[2];
        case LogTrace:
            return level_string[3];
        case LogDebug:
            return level_string[4];
        case LogWarning:
            return level_string[5];
        case LogError:
            return level_string[6];
        case LogFatal:
            return level_string[7];
        }
        return level_string[8];
    }

    const wchar_t* get_level_wstring(int level)
    {
        static const wchar_t* level_string[] = { L"Startup", L"Shutdown", L"Info", L"Trace", L"Debug", L"Warning", L"Error", L"Fatal", L"Unkown" };
        switch (level)
        {
        case LogStartup:
            return level_string[0];
        case LogShutdown:
            return level_string[1];
        case LogInfo:
            return level_string[2];
        case LogTrace:
            return level_string[3];
        case LogDebug:
            return level_string[4];
        case LogWarning:
            return level_string[5];
        case LogError:
            return level_string[6];
        case LogFatal:
            return level_string[7];
        }
        return level_string[8];
    }

    void set_encrypt_handle(encrypt_func_t encrypt_func)
    {
        encrypt_func_ = encrypt_func;
    }
};

//////////////////////////////////////////////////////////////////////////
// class LogFile

LogFile::LogFile(void)
{
}

LogFile::~LogFile(void)
{
}

bool LogFile::Init(int type, int level, int reserve_day, int size, const char* directory, const char* name, bool is_encrypt)
{
    if (NULL != impl_)
    {
        return true;
    }

    impl_.reset(new impl);
    if (NULL == impl_)
    {
        return false;
    }

    if (!impl_->init(type, level, reserve_day, size, directory, name, is_encrypt))
    {
        impl_.release();
        return false;
    }

    return true;
}

void LogFile::SetEncryptHandle(encrypt_func_t encrypt_func)
{
    impl_->set_encrypt_handle(encrypt_func);
}

void LogFile::Uninit()
{
    if (NULL != impl_)
    {
        impl_->uninit();
        impl_.release();
    }
}

// 设置输出类型
void LogFile::SetOuputType(int type)
{
    if (NULL != impl_ && 0 != (type & LOG_DEF_TYPE))
    {
        impl_->ouput_type_ = type;
    }
}

// 设置输出级别
void LogFile::SetOuputLevel(int level)
{
    if (NULL != impl_ && 0 != (level & LOG_DEF_LEVEL))
    {
        impl_->ouput_level_ = level;
    }
}
//清空当前日志内容
void LogFile::ClearLogContent()
{
	if(NULL != impl_)
	{
		impl_->clear_log_content();
	}
}
// 设置日志保存天数
void LogFile::SetReserveDay(int reserve_day)
{
    if (NULL != impl_ && 0 != reserve_day)
    {
        impl_->reserve_day_ = reserve_day;

        SYSTEMTIME st;
        GetLocalTime(&st);
        impl_->clear_log(st);
    }
}

void LogFile::OutputLog(int level, const char* format, ...)
{
    if (NULL != impl_)
    {
        va_list Args;
        va_start(Args, format);
        impl_->output_log(NULL, level, format, Args);
        va_end(Args);
    }
}

void LogFile::OutputLog(int level, const wchar_t* format, ...)
{
    if (NULL != impl_)
    {
        va_list Args;
        va_start(Args, format);
        impl_->output_log(NULL, level, format, Args);
        va_end(Args);
    }
}

void LogFile::OutputLog(const char* trace, int level, const char* format, ...)
{
    if (NULL != impl_)
    {
        va_list Args;
        va_start(Args, format);
        impl_->output_log(trace, level, format, Args);
        va_end(Args);
    }
}

void LogFile::OutputLog(const wchar_t* trace, int level, const wchar_t* format, ...)
{
    if (NULL != impl_)
    {
        va_list Args;
        va_start(Args, format);
        impl_->output_log(trace, level, format, Args);
        va_end(Args);
    }
}


void LogFile::OutputArgLog(int level, const char* format, va_list argptr)
{
    if (NULL != impl_)
    {
        impl_->output_log(NULL, level, format, argptr);
    }
}
void LogFile::OutputArgLog(int level, const wchar_t* format, va_list argptr)
{
    if (NULL != impl_)
    {
        impl_->output_log(NULL, level, format, argptr);
    }
}
void LogFile::OutputArgLog(const char* trace, int level, const char* format, va_list argptr)
{
    if (NULL != impl_)
    {
        impl_->output_log(trace, level, format, argptr);
    }
}
void LogFile::OutputArgLog(const wchar_t* trace, int level, const wchar_t* format, va_list argptr)
{
    if (NULL != impl_)
    {
        impl_->output_log(trace, level, format, argptr);
    }
}

//////////////////////////////////////////////////////////////////////////
// class ModuleLog

ModuleLog::ModuleLog(const char* module_name)
{
    strcpy_s(module_name_, sizeof(module_name_), module_name);
}

ModuleLog::~ModuleLog(void)
{
}

void ModuleLog::TraceLog(int level, const char* format, ...) const
{
    va_list Args;
    va_start(Args, format);
    us::LogFile::Instance().OutputArgLog(module_name_, level, format, Args);
    va_end(Args);
}

void ModuleLog::TraceLog(int level, const wchar_t* format, ...) const
{
    wchar_t module_name[260] = {0};
    mbyte_to_wchar(module_name_, module_name, 260);

    va_list Args;
    va_start(Args, format);
    us::LogFile::Instance().OutputArgLog(module_name, level, format, Args);
    va_end(Args);
}

}