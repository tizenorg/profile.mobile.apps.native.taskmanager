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




#ifndef __TASKMANAGER_EINA_H__
#define __TASKMANAGER_EINA_H__

#include <aul.h>

#include "taskmanager.h"

void _init_grp_cnt(void);
int _get_grp_cnt(int which);
int runapp_info_get(const aul_app_info *ainfo, void *data);
int taskmanager_get_history_app_info(void *data);
int _free_einalist_all(struct appdata *ad);
int _subt_einalist_item(struct appdata *ad, int pid);

#endif
/* __TASKMANAGER_EINA_H__ */
