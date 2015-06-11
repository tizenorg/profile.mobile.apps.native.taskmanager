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

#ifndef __TASK_MGR_LIST_H__
#define __TASK_MGR_LIST_H__

#include "util.h"

typedef struct {

	char *pkgid;
	char *appid;
	char *name;
	char *icon;
	char *arg;

	int pid;
	Eina_Bool nodisplay;
	Eina_Bool taskmanage;
	Eina_Bool unmounted;
	Eina_Bool multi_launch;
	Eina_Bool isAlreadySet;

	time_t launch_time;

} list_type_default_s;



extern task_mgr_error_e list_create(Eina_List **pkg_list);
extern void list_destroy(Eina_List *pkg_list);

extern task_mgr_error_e list_sort(Eina_List *pkg_list, int (*_sort_cb)(const void *d1, const void *d2));

#endif //__TASK_MGR_LIST_H__
