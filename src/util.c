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
#include <Elementary.h>
#include <app_manager.h>
#include <app_manager_extension.h>
#include <app_control.h>
#include <app_common.h>
#include <stdbool.h>

#include "log.h"
#include "util.h"



Eina_Bool util_kill_app(const char *appid)
{
	app_context_h context = NULL;
	Eina_Bool ret = EINA_TRUE;

	retv_if(!appid, EINA_FALSE);

	ret = app_manager_get_app_context(appid, &context);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("fail to app_manager_get_app_context(%d)", ret);
		return EINA_FALSE;
	}

	ret = app_manager_terminate_app(context);
	app_context_destroy(context);

	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("fail to terminate_app (%d)", ret);
		return EINA_FALSE;
	}

	_D("terminate app = %s", appid);

	return ret;
}



Eina_Bool util_launch_app(const char *appid)
{
	int ret;
	bool running = false;

	if (!appid || strlen(appid) == 0) {
		 _E("Fail to launch, due to Null appid.");
		return EINA_FALSE;
	}
	_D("Launching: %s", appid);
	ret = app_manager_is_running(appid, &running);
	retv_if(APP_MANAGER_ERROR_NONE != ret, EINA_FALSE);

	if (running) {
		_D("THE APP IS RUNNING");

		app_context_h context = NULL;
		ret = app_manager_get_app_context(appid, &context);
		if (ret != APP_MANAGER_ERROR_NONE) {
			_E("fail to app_manager_get_app_context(%d)", ret);
			return EINA_FALSE;
		}

		ret = app_manager_resume_app(context);
		app_context_destroy(context);

		if (ret != APP_MANAGER_ERROR_NONE) {
			_E("fail to app_manager_resume_app(%d)", ret);
			return EINA_FALSE;
		}

	} else {
		_D("THE APP IS NOT RUNNING");

		app_control_h service = NULL;
		retv_if(APP_CONTROL_ERROR_NONE != app_control_create(&service), EINA_FALSE);
		retv_if(!service, EINA_FALSE);

		app_control_set_operation(service, APP_CONTROL_OPERATION_MAIN);
		app_control_set_app_id(service, appid);

		ret = app_control_send_launch_request(service, NULL, NULL);
		(void)app_control_destroy(service);
		retv_if(APP_CONTROL_ERROR_NONE != ret, EINA_FALSE);
	}

	return EINA_TRUE;
}

const char *util_get_file_path(app_subdir dir, const char *relative)
{
	static char buf[PATH_MAX];
	char *prefix;

	switch (dir) {
	case APP_DIR_DATA:
		prefix = app_get_data_path();
		break;
	case APP_DIR_CACHE:
		prefix = app_get_cache_path();
		break;
	case APP_DIR_RESOURCE:
		prefix = app_get_resource_path();
		break;
	case APP_DIR_SHARED_DATA:
		prefix = app_get_shared_data_path();
		break;
	case APP_DIR_SHARED_RESOURCE:
		prefix = app_get_shared_resource_path();
		break;
	case APP_DIR_SHARED_TRUSTED:
		prefix = app_get_shared_trusted_path();
		break;
	case APP_DIR_EXTERNAL_DATA:
		prefix = app_get_external_data_path();
		break;
	case APP_DIR_EXTERNAL_CACHE:
		prefix = app_get_external_cache_path();
		break;
	case APP_DIR_EXTERNAL_SHARED_DATA:
		prefix = app_get_external_shared_data_path();
		break;
	default:
		LOGE("Not handled directory type.");
		return NULL;
	}
	size_t res = eina_file_path_join(buf, sizeof(buf), prefix, relative);
	free(prefix);
	if (res > sizeof(buf)) {
		LOGE("Path exceeded PATH_MAX");
		return NULL;
	}

	return &buf[0];
}

//End of file
