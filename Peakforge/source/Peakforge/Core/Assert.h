#pragma once

#if defined(PF_ENABLE_ASSERTS)
	#define PF_ASSERT(x, ...) { if(!(x)) {LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define PF_CORE_ASSERT(x, ...) { if(!(x)) {LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PF_ASSERT(...)
	#define PF_CORE_ASSERT(...)
#endif