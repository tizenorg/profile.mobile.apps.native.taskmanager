/*
 * org.tizen.taskmgr
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */




#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <Eina.h>
#include <ail.h>
#include <aul.h>
#include <rua.h>

#include "taskmanager.h"
#include "_util_log.h"

#define TASKMANAGER_ICON_NAME	"org.tizen.taskmgr.png"

static int grp_cnt[TS_MAX];

void _init_grp_cnt(void)
{
	int i;

	for(i = 0; i < TS_MAX; i++) {
		grp_cnt[i] = 0;
	}
}

int _get_grp_cnt(int which)
{
	return grp_cnt[which];
}

int runapp_info_get(const aul_app_info *ainfo, void *data)
{
	ail_appinfo_h handle;
	ail_error_e ret;

	char *valc;
	bool valb;
	char buf[1024] = { 0, };
	struct appdata *ad = data;
	struct _task_info *info;

	retvm_if(ainfo == NULL, -1, "Invalid argument: ainfo is NULL\n");
	retvm_if(data == NULL, -1, "Invalid argument: data is NULL\n");

	retvm_if(ainfo->pid <= 0, -1, "Invalid pid(%u)\n", ainfo->pid);

	/* filtering */
	if (ainfo->pid == getpid()) {
		return 0;
	}

	retvm_if(ainfo->pkg_name == NULL, 0, "Invalid pkg_name(%s)\n", ainfo->pkg_name);

//	_D("running app is (%s)\n", ainfo->pkg_name);
	ret = ail_package_get_appinfo(ainfo->pkg_name, &handle);
	retvm_if(ret != AIL_ERROR_OK, -1,
			"Failed to get appinfo, pkg_name:%s\n", ainfo->pkg_name);

	ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &valb);
	if (valb == 0) {
		goto exit;
	}
	info = calloc(1, sizeof(struct _task_info));
	if (info == NULL) {
		_E("Failed to calloc task_info\n");
		goto exit;
	}
	info->pkg_name = strdup(ainfo->pkg_name);

	ret = ail_appinfo_get_str(handle, AIL_PROP_NAME_STR, &valc);
	if (valc == NULL) {
		_D("%s: Failed to get ail name\n", ainfo->pkg_name);
		valc = "Unknown";
	}
	info->app_name = strdup(valc);

	ret = ail_appinfo_get_str(handle, AIL_PROP_ICON_STR, &valc);
	if (valc == NULL || (ecore_file_exists(valc) == EINA_FALSE)) {
		_D("%s: Failed to get ail icon\n", ainfo->pkg_name);
		valc = TASKMANAGER_ICON_NAME;
	}

	snprintf(buf, sizeof(buf), "%s", valc);
	info->icn_path = strdup(buf);
//	_D("get app name[%s] set [%s], icon path[%s]\n", ainfo->pkg_name, info->app_name, buf);

	info->ad = ad;
	info->pid = ainfo->pid;
	info->category = TS_INUSE;
//	info->mem_total = ad->mem_total;
	_D("%s/pid(%d)\n", info->app_name, info->pid);

	ad->applist[TS_INUSE] = eina_list_prepend(ad->applist[TS_INUSE], info);
	grp_cnt[TS_INUSE]++;

 exit:
	ret = ail_package_destroy_appinfo(handle);
	retvm_if(ret != AIL_ERROR_OK, -1, "Failed to destroy appinfo\n");
	return 0;
}

int taskmanager_get_history_app_info(void *data)
{
	struct appdata *ad = data;
	struct _task_info *info, *info_r;
	Eina_List *l_r;
	int flag = 0;

	struct rua_rec rec_result = { 0, };
	char **table = NULL;
	char buf[1024] = { 0, };
	int nrows = 0, ncols = 0;
	int row = 0;

	ail_appinfo_h handle;
	ail_error_e ret;
	bool valb;
	char *valc;

	retvm_if(data == NULL, -1, "Invalid argument: data is NULL\n");
	retvm_if(rua_init() == -1, -1, "Failed to rua_init\n");

	if (rua_history_load_db(&table, &nrows, &ncols) == -1) {
		rua_fini();
		return -1;
	}

	if (nrows == 0) {
		rua_history_unload_db(&table);
		rua_fini();
		return 0;
	}

	ad->applist[TS_HISTORY] = eina_list_nth_list(ad->applist[TS_HISTORY], 0);
	for (row = 0; row < nrows; row++) {
		rua_history_get_rec(&rec_result, table, nrows, ncols, row);

		/* filtering
		 * pkg_name could be NULL or 0 length because it is launch by fork.
		 */
		if (rec_result.pkg_name == NULL
		    || strlen(rec_result.pkg_name) < 1) {
			continue;
		}

		_D("%s\n", rec_result.pkg_name);
		ret = ail_package_get_appinfo(rec_result.pkg_name, &handle);
		if (ret != AIL_ERROR_OK) {
			_D("Failed to get appinfo(%d)\n", ret);
			continue;
		}

		ret = ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &valb);
		if(valb == 0) {
			_D("ail_appinfo_get_bool(handle, AIL_PROP_X_SLP_TASKMANAGE_BOOL, &valb) => valb is 0");
			goto cont;
		}

		EINA_LIST_FOREACH(ad->applist[TS_INUSE], l_r, info_r) {
			if (info_r != NULL) {
				if (!strcmp
				    (rec_result.pkg_name, info_r->pkg_name)) {
					flag = 1;
					break;
				}
			}
		}

		if (flag == 0) {
			info = calloc(1, sizeof(struct _task_info));
			retvm_if(info == NULL, -1, "Failed to calloc _task_info\n");

			info->pkg_name = strdup(rec_result.pkg_name);

			ret = ail_appinfo_get_str(handle, AIL_PROP_NAME_STR, &valc);
			if (valc == NULL) {
				_D("Failed to get ail name\n");
				valc = "Unknown";
			}
			info->app_name = strdup(valc);
			_D("%s\n", info->app_name);

			ret = ail_appinfo_get_str(handle, AIL_PROP_ICON_STR, &valc);
			if (valc == NULL || (ecore_file_exists(valc) == EINA_FALSE)) {
				_D("Failed to get ail icon\n");
				valc = TASKMANAGER_ICON_NAME;
			}
			snprintf(buf, sizeof(buf), "%s", valc);
			info->icn_path = strdup(buf);
			_D("%s\n", info->icn_path);

			info->ad = ad;
			info->pid = 0;
			info->category = TS_HISTORY;

			if (rec_result.arg != NULL) {
				if (strlen(rec_result.arg) > 0) {
					info->b = bundle_decode(
							(const bundle_raw *)rec_result.arg,
							strlen(rec_result.arg));
				}
			}

			ad->applist[TS_HISTORY] =
			    eina_list_append(ad->applist[TS_HISTORY], info);
			grp_cnt[TS_HISTORY]++;

		}

		flag = 0;

cont:
		ret = ail_package_destroy_appinfo(handle);
	}

	rua_history_unload_db(&table);
	rua_fini();

	return 0;
}

int _free_einalist_all(struct appdata *ad)
{
	Eina_List *l;
	struct _task_info *info = NULL;
	int i;

	if (ad == NULL) {
		printf("[Error] Invalid argument: appdata is NULL\n");
		return -1;
	}

	for (i = 0; i < TS_MAX; i++) {
		if (ad->applist[i] == NULL)
			continue;

		EINA_LIST_FOREACH(ad->applist[i], l, info) {
			if (info != NULL) {
				if (info->b)
					bundle_free(info->b);

				taskmanager_free_info(info);
				info = NULL;
			}
		}

		eina_list_free(ad->applist[i]);
		ad->applist[i] = NULL;
	}

	return 0;
}

int _subt_einalist_item(struct appdata *ad, int pid)
{
_D("func\n");
	Eina_List *l;
	int ret = -1;
	struct _task_info *info;

	retvm_if(ad == NULL, -1, "Invalid argument: appdata is NULL\n");
	retvm_if(ad->applist[TS_INUSE] == NULL, -1, "applist is NULL\n");

	EINA_LIST_FOREACH(ad->applist[TS_INUSE], l, info) {
		_D("pid(%u):(%u)\n", pid, info->pid);
		if (info == NULL) {
			_E("Failed to get info\n");
			continue;
		}

		if (pid > 0 && pid == info->pid) {
			if(info->app_name)	_D("killed [%s]\n", info->app_name);
			ad->applist[TS_INUSE] =
			    eina_list_remove_list(ad->applist[TS_INUSE], l);
			taskmanager_free_info(info);
			info = NULL;
			ret = 0;
		}
	}
	return ret;
}




