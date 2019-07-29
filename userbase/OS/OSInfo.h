#pragma once

namespace ubase
{
	enum WIN_OS_VERSION
	{
		WIN_UNKNOWN=0,
		WIN_2000,
		WIN_XP,
		WIN_2003,
		WIN_VISTA,
		WIN_7x32,
		WIN_7x64,
		WIN_8x32,
		WIN_8x64
	};

	class OSInfo
	{
	public:
		OSInfo();
		~OSInfo();
		WIN_OS_VERSION GetOSVersion(); //这个方法只能准确定位到win8以下包括win8的系统版本
	};
}