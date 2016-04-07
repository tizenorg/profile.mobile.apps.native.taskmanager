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
#include <context_history.h>

#include "list.h"
#include "log.h"
#include "main.h"
#include "util.h"
#include "conf.h"

typedef struct pkginfo {
	char *appid;
	bool taskmanage;
	char *pkgid;
	char *icon;
	char *name;
	bool nodisplay;
} private_pkginfo_s;

typedef struct {
	char *appid;
	int pid;
} private_pid_s;


static void _pkglist_unretrieve_item(list_type_default_s *default_info)
{
	if (!default_info) {
		return;
	}

	free(default_info->name);
	default_info->name = NULL;

	free(default_info->icon);
	default_info->icon = NULL;

	free(default_info->pkgid);
	default_info->pkgid = NULL;

	free(default_info->arg);
	default_info->arg = NULL;

	free(default_info->appid);
	default_info->appid = NULL;

	free(default_info);
	default_info = NULL;
}

static task_mgr_error_e _get_app_launchtime(Eina_List *pkg_list)
{

	_D("");

	int ret = CONTEXT_HISTORY_ERROR_NONE;
	int rec_size = 0;
	int last_launch_time;
	char *context_app_id;
	unsigned pkg_size;
	unsigned cxt_size;
	Eina_List *l;
	list_type_default_s *pkg_info = NULL;

	context_history_h history_handle;
	context_history_list_h history_app_list;
	context_history_record_h history_record;

	ret = context_history_create(&history_handle);
	retv_if(ret != CONTEXT_HISTORY_ERROR_NONE, TASK_MGR_ERROR_FAIL);

	ret = context_history_get_list(history_handle, CONTEXT_HISTORY_RECENTLY_USED_APP, NULL, &history_app_list);

	if (ret != CONTEXT_HISTORY_ERROR_NONE) {
		context_history_destroy(history_handle);
		return TASK_MGR_ERROR_FAIL;
	}

	ret = context_history_list_get_count(history_app_list, &rec_size);
	if (ret != CONTEXT_HISTORY_ERROR_NONE) {
		context_history_list_destroy(history_app_list);
		context_history_destroy(history_handle);
		return TASK_MGR_ERROR_FAIL;
	}

	_D("Num of records: %d", rec_size);

	int i;
	for (i = 0; i < rec_size; ++i) {

		context_history_list_get_current(history_app_list, &history_record);
		context_history_record_get_int(history_record, CONTEXT_HISTORY_LAST_TIME, &last_launch_time);
		context_history_record_get_string(history_record, CONTEXT_HISTORY_APP_ID, &context_app_id);

		if (!context_app_id) {
			_D("Context_app_id == NULL");
			context_history_record_destroy(history_record);
			context_history_list_move_next(history_app_list);
			continue;
		}

		EINA_LIST_FOREACH(pkg_list, l, pkg_info) {

			if (!pkg_info || !pkg_info->appid)
				continue;

			pkg_size = strlen(pkg_info->appid);
			cxt_size = strlen(context_app_id);

			if (!strncmp(pkg_info->appid, context_app_id,
					pkg_size > cxt_size ? pkg_size : cxt_size)
					&& strlen(pkg_info->appid) == strlen(context_app_id))

				pkg_info->launch_time = last_launch_time;
		}

		free(context_app_id);

		context_history_record_destroy(history_record);
		context_history_list_move_next(history_app_list);
	}

	context_history_list_destroy(history_app_list);
	context_history_destroy(history_handle);

	return TASK_MGR_ERROR_NONE;

}

static void _release_pkg_info(list_type_default_s *pkg_info)
{
	_D("");

	if (!pkg_info)
		return;

	free(pkg_info->name);
	free(pkg_info->icon);
	free(pkg_info->pkgid);
	free(pkg_info->appid);
	free(pkg_info);

}

static bool _get_pkginfo_cb(app_info_h app_handle, void *user_data)
{
	_D("");

	bool is_running = false;
	Eina_List **pkg_list = (Eina_List **) user_data;

	list_type_default_s *pkg_info = calloc(1, sizeof(list_type_default_s));
	retv_if(!pkg_info, false);

	if (app_info_get_app_id(app_handle, &pkg_info->appid) != APP_MANAGER_ERROR_NONE
			|| !pkg_info->appid) {

		_release_pkg_info(pkg_info);
		return false;
	}

	if (app_manager_is_running(pkg_info->appid, &is_running) != APP_MANAGER_ERROR_NONE
			|| !is_running) {

		_D("Application %s is not running", pkg_info->appid);
		_release_pkg_info(pkg_info);
		return true;
	}

	if (app_info_get_package(app_handle, &pkg_info->pkgid) != APP_MANAGER_ERROR_NONE
			|| !pkg_info->pkgid) {

		_release_pkg_info(pkg_info);
		return false;
	}

	if (app_info_get_label(app_handle, &pkg_info->name) != APP_MANAGER_ERROR_NONE
			|| !pkg_info->name) {

		_release_pkg_info(pkg_info);
		return false;
	}

	if (app_info_is_nodisplay(app_handle, &pkg_info->nodisplay) != APP_MANAGER_ERROR_NONE) {

		_release_pkg_info(pkg_info);
		return false;
	}

	if (app_info_get_icon(app_handle, &pkg_info->icon) != APP_MANAGER_ERROR_NONE
			|| !pkg_info->icon) {

		_release_pkg_info(pkg_info);
		return false;
	}

	if (0 != access(pkg_info->icon, F_OK)) {

		_D("Fail to access icon path");
		pkg_info->icon = strdup(util_get_file_path(APP_DIR_RESOURCE, ITEM_LAYOUT_EDJ));
	}

	_D("AppID: %s", pkg_info->appid);
	_D("PkgID: %s", pkg_info->pkgid);
	_D("Icon: %s", pkg_info->icon);
	_D("Label: %s", pkg_info->name);

	*pkg_list = eina_list_append(*pkg_list, pkg_info);

	return true;

}

static int _launch_time_sort_cb(const void *d1, const void *d2)
{
	list_type_default_s *tmp1 = (list_type_default_s *) d1;
	list_type_default_s *tmp2 = (list_type_default_s *) d2;

	if (!tmp1) return -1;
	if (!tmp2) return 1;

	if (!tmp1->appid) return 1;
	else if (!tmp2->appid) return -1;

	return tmp1->launch_time >= tmp2->launch_time ? -1 : 1;
}

static task_mgr_error_e _get_running_apps(Eina_List **pkg_list)
{
	_D("");

	app_info_filter_h handle;
	int ret = 0;

	ret = app_info_filter_create(&handle);
	if (ret != APP_MANAGER_ERROR_NONE)
		return TASK_MGR_ERROR_FAIL;

	ret = app_info_filter_add_bool(handle, PACKAGE_INFO_PROP_APP_TASKMANAGE, 1);
	if (ret != APP_MANAGER_ERROR_NONE) {
		app_info_filter_destroy(handle);
		return TASK_MGR_ERROR_FAIL;
	}

	ret = app_info_filter_foreach_appinfo(handle, _get_pkginfo_cb, pkg_list);
	if (ret != APP_MANAGER_ERROR_NONE) {
		app_info_filter_destroy(handle);
		return TASK_MGR_ERROR_FAIL;
	}

	app_info_filter_destroy(handle);

	return TASK_MGR_ERROR_NONE;
}

Eina_List *list_sort(Eina_List *list, int (*_sort_cb)(const void *d1, const void *d2))
{
	retv_if(!list, NULL);

	list = eina_list_sort(list, eina_list_count(list), _sort_cb);
	retv_if(!list, NULL);

	return list;
}

extern task_mgr_error_e list_create(Eina_List **pkg_list)
{
	int ret = TASK_MGR_ERROR_NONE;

	ret = _get_running_apps(pkg_list);
	if (ret != TASK_MGR_ERROR_NONE || !*pkg_list) {
		list_destroy(*pkg_list);
		return TASK_MGR_ERROR_FAIL;
	}

	ret = _get_app_launchtime(*pkg_list);
	if (ret != TASK_MGR_ERROR_NONE || !*pkg_list) {
		list_destroy(*pkg_list);
		return TASK_MGR_ERROR_FAIL;
	}

	*pkg_list = list_sort(*pkg_list, _launch_time_sort_cb);
	if (!*pkg_list) {
		list_destroy(*pkg_list);
		return TASK_MGR_ERROR_FAIL;
	}

	return TASK_MGR_ERROR_NONE;
}

extern void list_destroy(Eina_List *pkg_list)
{
	_D("");
	list_type_default_s *default_info = NULL;

	if (!pkg_list) {
		_D("pkg_list is null");
		return;
	}

	EINA_LIST_FREE(pkg_list, default_info) {
		if (!default_info) {
			continue;
		}
		_pkglist_unretrieve_item(default_info);
	}

	pkg_list = NULL;
}
