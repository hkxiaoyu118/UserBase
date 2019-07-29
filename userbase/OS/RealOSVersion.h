#pragma once

namespace ubase
{
	bool GetRealOsVersion(unsigned int* const major, unsigned int* const minor, unsigned int* const build);
	bool GetFakeOsVersion(unsigned int* const major, unsigned int* const minor, unsigned int* const build);
}
