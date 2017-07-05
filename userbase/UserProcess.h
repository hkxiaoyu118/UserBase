#pragma once
#include <windows.h>

bool EnableDebugPriv();//提升当前进程权限函数("SeDebugPrivilege"读、写控制权限)
bool EnableShutdownPriv();//提升当前进程权限函数("SeShutdownPrivilege"关机权限)
bool EnableBackupPriv();//提升当前进程权限函数("SeBackupPrivilege"备份数据权限)
bool EnableRestorePriv();//提升当前进程权限函数("SeRestorePrivilege"恢复数据权限)