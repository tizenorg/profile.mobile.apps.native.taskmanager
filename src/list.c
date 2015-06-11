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
#include <pkgmgr-info.h>
#include <rua.h>

#include "list.h"
#include "log.h"
#include "main.h"
#include "util.h"

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

static struct {
	Eina_Hash *pkginfo_table;
} private_table_s = {
	.pkginfo_table = NULL,
};



static void _pkglist_unretrieve_item(list_type_default_s *default_info)
{
	if (!default_info) {
		return;
	}

	if (default_info->name) {
		free(default_info->name);
		default_info->name = NULL;
	}
	if (default_info->icon) {
		free(default_info->icon);
		default_info->icon = NULL;
	}
	if (default_info->pkgid) {
		free(default_info->pkgid);
		default_info->pkgid = NULL;
	}
	if (default_info->arg) {
		free(default_info->arg);
		default_info->arg = NULL;
	}
	if (default_info->appid) {
		free(default_info->appid);
		default_info->appid = NULL;
	}
	if (default_info) {
		free(default_info);
		default_info = NULL;
	}
}



static list_type_default_s *_pkglist_retrieve_item(const char *appid, const char *arg, time_t launch_time)
{
	retv_if(!appid, NULL);

	private_pkginfo_s *pkg_info = NULL;
	list_type_default_s *default_info = NULL;

	pkg_info = eina_hash_find(private_table_s.pkginfo_table, appid);
	if (!pkg_info) {
		_D("app(%s) is not taskmanage app", appid);
		return NULL;
	}

	if (!pkg_info->taskmanage) {
		_D("app(%s) is not taskmanage app", appid);
		return NULL;
	}

	default_info = calloc(1, sizeof(*default_info));
	retv_if(!default_info, NULL);

	default_info->taskmanage = pkg_info->taskmanage;
	default_info->launch_time = launch_time;
	default_info->nodisplay = pkg_info->nodisplay;

	default_info->appid = strdup(appid);
	goto_if(!default_info->appid, ERROR);

	if (arg) {
		default_info->arg = strdup(arg);
		goto_if(!default_info->arg, ERROR);
	}

	if (pkg_info->pkgid) {
		default_info->pkgid = strdup(pkg_info->pkgid);
		goto_if(!default_info->pkgid, ERROR);
	} else {
		_E("Fail to get pkgid from pkg info table");
		goto ERROR;
	}

	if (pkg_info->icon) {
		default_info->icon = strdup(pkg_info->icon);
		goto_if(!default_info->icon, ERROR);
	} else {
		_E("Fail to get icon from pkg info table");
		goto ERROR;
	}

	if (pkg_info->name) {
		default_info->name = strdup(pkg_info->name);
		goto_if(!default_info->name, ERROR);
	} else {
		_E("Fail to get name from pkg info table");
		goto ERROR;
	}

	_D("list add id : [%s], icon : [%s], name : [%s]", pkg_info->pkgid, pkg_info->icon, pkg_info->name);

	return default_info;

ERROR:

	_pkglist_unretrieve_item(default_info);
	return NULL;
}



int _get_pkginfo_cb(pkgmgrinfo_appinfo_h app_handle, void *user_data)
{
	char *appid = NULL;
	char *pkgid = NULL;
	char *name = NULL;
	char *icon = NULL;
	private_pkginfo_s *pkg_info = NULL;

	pkg_info = calloc(1, sizeof(*pkg_info));
	retv_if(!pkg_info, PMINFO_R_ERROR);

	memset(pkg_info, 0, sizeof(private_pkginfo_s));

	pkgmgrinfo_appinfo_get_appid(app_handle, &appid);
	goto_if(!appid, ERROR);

	pkg_info->appid = strdup(appid);
	goto_if(!pkg_info->appid, ERROR);

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_is_taskmanage(app_handle, &pkg_info->taskmanage)) {
		goto ERROR;
	}

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_get_pkgid(app_handle, &pkgid)) {
		goto ERROR;
	}
	pkg_info->pkgid = strdup(pkgid);
	goto_if(!pkg_info->pkgid, ERROR);

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_get_icon(app_handle, &icon)) {
		goto ERROR;
	}
	if (icon) {
		pkg_info->icon= strdup(icon);
		goto_if(!pkg_info->icon, ERROR);
	}

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_get_label(app_handle, &name)) {
		goto ERROR;
	}
	if (name) {
		pkg_info->name= strdup(name);
		goto_if(!pkg_info->name, ERROR);
	}

	if (PMINFO_R_OK != pkgmgrinfo_appinfo_is_nodisplay(app_handle, &pkg_info->nodisplay)) {
		goto ERROR;
	}

	eina_hash_add(private_table_s.pkginfo_table, pkg_info->appid, pkg_info);

	return PMINFO_R_OK;

ERROR:
	if (pkg_info->name) free(pkg_info->name);
	if (pkg_info->icon) free(pkg_info->icon);
	if (pkg_info->pkgid) free(pkg_info->pkgid);
	if (pkg_info->appid) free(pkg_info->appid);
	free(pkg_info);

	return PMINFO_R_ERROR;
}



static task_mgr_error_e _create_pkginfo_table(void)
{
	_D("");
	pkgmgrinfo_appinfo_filter_h handle;

	int ret = 0;

	private_table_s.pkginfo_table = eina_hash_string_superfast_new(NULL);

	ret = pkgmgrinfo_appinfo_filter_create(&handle);
	if (PMINFO_R_OK != ret) {
		return TASK_MGR_ERROR_FAIL;
	}

	ret = pkgmgrinfo_appinfo_filter_add_bool(handle, PMINFO_APPINFO_PROP_APP_TASKMANAGE, 1);
	if (PMINFO_R_OK != ret) {
		pkgmgrinfo_appinfo_filter_destroy(handle);
		return TASK_MGR_ERROR_FAIL;
	}

	ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(handle, _get_pkginfo_cb, NULL);
	if (ret != PMINFO_R_OK) {
		pkgmgrinfo_appinfo_filter_destroy(handle);
		return TASK_MGR_ERROR_FAIL;
	}

	pkgmgrinfo_appinfo_filter_destroy(handle);

	return TASK_MGR_ERROR_NONE;
}



static Eina_Bool _remove_pkginfo(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
	retv_if(!data, EINA_FALSE);

	private_pkginfo_s *pkg_info = data;

	if (pkg_info->name) free(pkg_info->name);
	if (pkg_info->icon) free(pkg_info->icon);
	if (pkg_info->pkgid) free(pkg_info->pkgid);
	if (pkg_info->appid) free(pkg_info->appid);

	return EINA_TRUE;
}



static void _destroy_pkginfo_table(void)
{
	_D("");

	eina_hash_foreach(private_table_s.pkginfo_table, _remove_pkginfo, NULL);
	eina_hash_free(private_table_s.pkginfo_table);
	private_table_s.pkginfo_table = NULL;
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



extern task_mgr_error_e list_sort(Eina_List *list, int (*_sort_cb)(const void *d1, const void *d2))
{
	retv_if(!list, TASK_MGR_ERROR_INVALID_PARAMETER);

	list = eina_list_sort(list, eina_list_count(list), _sort_cb);
	retv_if(!list, TASK_MGR_ERROR_FAIL);

	return TASK_MGR_ERROR_NONE;
}



extern task_mgr_error_e list_create(Eina_List **pkg_list)
{
	_D("");

	rua_init();
	char **table = NULL;
	list_type_default_s *default_info = NULL;

	int nrows = 0, ncols = 0;
	int row = 0;

	retv_if (TASK_MGR_ERROR_NONE != _create_pkginfo_table(), TASK_MGR_ERROR_FAIL);

	if (-1 == rua_history_load_db(&table, &nrows, &ncols)) {
		if (table) {
			rua_history_unload_db(&table);
		}
		return TASK_MGR_ERROR_FAIL;
	}

	_D("Apps in rua history is %d", nrows);

	for (; row < nrows; row++) {
		struct rua_rec rec_result = {0, };
		rua_history_get_rec(&rec_result, table, nrows, ncols, row);
		default_info = _pkglist_retrieve_item(rec_result.pkg_name, rec_result.arg, rec_result.launch_time);
		if (default_info) {
			*pkg_list = eina_list_append(*pkg_list, default_info);
		}
	}

	if (*pkg_list && TASK_MGR_ERROR_NONE != list_sort(*pkg_list, _launch_time_sort_cb)) {
		_E("Cannot sort pkg_list");
	}

	if (!eina_list_count(*pkg_list)) {
		_D("list is empty.");
		_pkglist_unretrieve_item(default_info);
		return TASK_MGR_ERROR_NO_DATA;

	}

	return TASK_MGR_ERROR_NONE;
}



extern void list_destroy(Eina_List *pkg_list)
{
	_D("");
	list_type_default_s *default_info = NULL;

	_destroy_pkginfo_table();

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
	rua_fini();
}
