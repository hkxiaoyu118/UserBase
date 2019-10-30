#pragma once
#include <Windows.h>
#include <Dbt.h>

//如果需要监控磁盘卷的添加和删除,则简历窗口,把此函数作为回调函数
BOOL CALLBACK ProgMainDlg(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
