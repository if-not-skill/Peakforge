#pragma once

#ifdef PF_PLATFORM_WINDOWS
#else
	#error Peakforge only supports Windows!
#endif

#define BIT(x) (1 << x)