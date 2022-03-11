#pragma once
#ifndef DLL_EXPORT
#include <windows/AllowWindowsPlatformTypes.h>
#include <windows/PreWindowsApi.h>
#endif

#include "Common/CommonInclude.h"
#include "JobSystem/JobInclude.h"
#include "TinyXML/tinyxml.h"
#include "Utility/FileUtility.h"
#include "Utility/MGuid.h"
#include "Utility/Path.h"
#include "Utility/Random.h"
#include "Utility/StringUtility.h"
#include "Utility/TaskThread.h"
#include "Utility/ThreadResetEvent.h"
#include "Common/DynamicDLL.h"
#ifndef DLL_EXPORT
#include <windows/PostWindowsApi.h>
#include <windows/HideWindowsPlatformTypes.h>
#endif