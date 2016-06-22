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
#include <rua.h>

#include "list.h"
#include "log.h"
#include "main.h"
#include "util.h"
#include "conf.h"

static Eina_List *app_list;

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

	free(default_info->appid);
	default_info->appid = NULL;

	free(default_info);
	default_info = NULL;
}

static list_type_default_s *_pkglist_item_create(app_info_h app_info)
{
	if (!app_info)
		return NULL;

	list_type_default_s *list_item = calloc(1, sizeof(list_type_default_s));

	int ret = app_info_get_app_id(app_info, &list_item->appid);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_get_app_id failed[%d]: %s", ret, get_error_message(ret));
		_pkglist_unretrieve_item(list_item);
		return NULL;
	}

	ret = app_info_get_package(app_info, &list_item->pkgid);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_get_package failed[%d]: %s", ret, get_error_message(ret));
		_pkglist_unretrieve_item(list_item);
		return NULL;
	}

	ret = app_info_get_label(app_info, &list_item->name);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_get_app_id failed[%d]: %s", ret, get_error_message(ret));
		_pkglist_unretrieve_item(list_item);
		return NULL;
	}

	ret = app_info_get_icon(app_info, &list_item->icon);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_get_app_id failed[%d]: %s", ret, get_error_message(ret));
		_pkglist_unretrieve_item(list_item);
		return NULL;
	}

	return list_item;
}

static int _launch_time_sort_cb(const void *d1, const void *d2)
{
	list_type_default_s *tmp1 = (list_type_default_s *) d1;
	list_type_default_s *tmp2 = (list_type_default_s *) d2;

	if (!tmp1) return -1;
	if (!tmp2) return 1;

	return tmp1->launch_time >= tmp2->launch_time ? -1 : 1;
}


static bool _pkglist_item_append(app_info_h app_info, void *user_data)
{
	struct rua_rec *record = user_data;
	retvm_if(!record, false, "record data == NULL");

	list_type_default_s *list_item = _pkglist_item_create(app_info);
	if (!list_item)
		return true;

	list_item->launch_time = record->launch_time;

	app_list = eina_list_sorted_insert(app_list, _launch_time_sort_cb, list_item);

	return false;
}

static int _pkglist_filter_create(app_info_filter_h *filter)
{
	int ret = app_info_filter_create(filter);
	retvm_if(ret != APP_MANAGER_ERROR_NONE, TASK_MGR_ERROR_FAIL, "app_info_filter_create failed");

	ret = app_info_filter_add_bool(*filter, PACKAGE_INFO_PROP_APP_TASKMANAGE, true);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_filer_add_bool failed[%d]: %s", ret, get_error_message(ret));
		app_info_filter_destroy(*filter);
		return TASK_MGR_ERROR_FAIL;
	}

	return TASK_MGR_ERROR_NONE;
}

static int _pkglist_find_and_append_app(struct rua_rec *record, app_info_filter_h *filter)
{
	int ret = app_info_filter_add_string(*filter, PACKAGE_INFO_PROP_APP_ID, record->pkg_name);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_filter_add_string failed[%d]: %s", ret, get_error_message(ret));
		return TASK_MGR_ERROR_FAIL;
	}

	ret = app_info_filter_foreach_appinfo(*filter, _pkglist_item_append, record);
	if (ret != APP_MANAGER_ERROR_NONE) {
		_E("app_info_filter_foreach_app_info failed[%d]: %s", ret, get_error_message(ret));
		return TASK_MGR_ERROR_FAIL;
	}

	return TASK_MGR_ERROR_NONE;
}

extern Eina_List *list_pkg_list_get(void)
{
	return app_list;
}

extern task_mgr_error_e list_init(void)
{
	char **table = NULL;
	int rows = 0;
	int cols = 0;
	struct rua_rec record;

	app_info_filter_h filter;

	if (rua_init()) {
		_E("rua_init failed");
		return TASK_MGR_ERROR_FAIL;
	}

	if (rua_history_load_db(&table, &rows, &cols) || !table) {
		_E("rua_history_load_db failed");
		return TASK_MGR_ERROR_FAIL;
	}

	int ret = _pkglist_filter_create(&filter);
	retvm_if(ret != TASK_MGR_ERROR_NONE, ret, "_pkglist_taskmanage_filter_create failed");

	int row;
	for (row = 0; row < rows; ++row) {
		rua_history_get_rec(&record, table, rows, cols, row);
		ret = _pkglist_find_and_append_app(&record, &filter);
		if (ret != TASK_MGR_ERROR_NONE)
			_W("Could not find or append application to the pkg list");
	}

	rua_history_unload_db(&table);
	app_info_filter_destroy(filter);

	if (rua_fini())
		_E("rua_fini failed");

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
