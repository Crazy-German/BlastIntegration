#pragma once
#include "Logger/logger.h"

#if _DEBUG
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, FeatherEngine, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogGameEngine, GameWorld, Warning);
#endif

#define MVLOG(Verbosity, Message, ...) LOG(LogGameEngine, Verbosity, Message, ##__VA_ARGS__)

struct ApplicationData
{
	// Application State data goes here for easy access.
};

inline ApplicationData Application;