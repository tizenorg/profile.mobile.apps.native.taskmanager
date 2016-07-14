/*
 *  Task Manager
 *
 * Copyright (c) 2000 - 2015 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __TASK_MGR_LOG_H__
#define __TASK_MGR_LOG_H__

#include <unistd.h>
#include <dlog.h>

/**
 * @defgroup Utils Utilities
 */

/**
 * @addtogroup Utils
 * @{
 */

#undef LOG_TAG
#define LOG_TAG "TASK_MGR"

#if !defined(_SD)
#define _SD(fmt, arg...) SECURE_LOGD(fmt"\n", ##arg)
#endif

#if !defined(_SW)
#define _SW(fmt, arg...) SECURE_LOGW(fmt"\n", ##arg)
#endif

#if !defined(_SE)
#define _SE(fmt, arg...) SECURE_LOGE(fmt"\n", ##arg)
#endif

#define retvm_if_timer(timer, expr, val, fmt, arg...) do { \
	if (expr) { \
		_E(fmt, ##arg); \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		timer = NULL; \
		return (val); \
	} \
} while (0)

#define retvm_if(expr, val, fmt, arg...) do { \
	if(expr) { \
		_E(fmt, ##arg); \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return val; \
	} \
} while (0)

#define retv_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#define retm_if(expr, fmt, arg...) do { \
	if(expr) { \
		_E(fmt, ##arg); \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)

#define ret_if(expr) do { \
	if(expr) { \
		_E("(%s) -> %s() return", #expr, __FUNCTION__); \
		return; \
	} \
} while (0)

#define goto_if(expr, val) do { \
	if(expr) { \
		_E("(%s) -> goto", #expr); \
		goto val; \
	} \
} while (0)

#define break_if(expr) { \
	if(expr) { \
		_E("(%s) -> break", #expr); \
		break; \
	} \
}

#define continue_if(expr) { \
	if(expr) { \
		_E("(%s) -> continue", #expr); \
		continue; \
	} \
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#if !defined(_D)
#define _D(fmt, arg...) dlog_print(DLOG_DEBUG, LOG_TAG, "%s: %s[%d] " #fmt "\n", __FILENAME__, __func__, __LINE__, ##arg)
#endif

#if !defined(_W)
#define _W(fmt, arg...) dlog_print(DLOG_WARN, LOG_TAG, "%s: %s[%d] " #fmt "\n", __FILENAME__, __func__, __LINE__, ##arg)
#endif

#if !defined(_E)
#define _E(fmt, arg...) dlog_print(DLOG_ERROR, LOG_TAG, "%s: %s[%d] " #fmt "\n", __FILENAME__, __func__, __LINE__, ##arg)
#endif

/**
 * @}
 */

#endif //__TASK_MGR_LOG_H__
