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
//#include <app_manager_extension.h>
#include <app_control.h>
#include <stdbool.h>

#include "log.h"
#include "util.h"



extern Eina_Bool util_kill_app(const char *appid)
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



extern Eina_Bool util_launch_app(const char *appid)
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



//End of file
