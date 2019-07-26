#pragma once
#include <iostream>
#include <windows.h>
#include <Wincrypt.h>

namespace ubase
{
	bool IntGetRandomUInt(unsigned int &randomNum);//获取一个真随机数
	void ConvertUIntToBytes(unsigned int number, unsigned char numBytes[], bool bigEndian);//将unsigned int转为bytes
	unsigned int ConvertBytesToUInt(unsigned char numBytes[], bool bigEndian);//将bytes转为unsigned int
}
