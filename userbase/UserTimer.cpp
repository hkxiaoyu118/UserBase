#include "UserTimer.h"
#include <time.h>

namespace ubase
{
	__int64 TimerGetUnixTimeNow()
	{
		time_t t = time(0);
		return t;
	}
}
