/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */




#ifndef __TASKMANAGER_LOGIC_H__
#define __TASKMANAGER_LOGIC_H__

#include "taskmanager.h"

int _dead_cb(int pid, void *data);
Eina_Bool _create_idler_cb(void *data);
int _app_create(struct appdata *ad);
void _ok_response_cb(void *data, Evas_Object *obj, void *event_info);
void _cancel_response_cb(void *data, Evas_Object *obj, void *event_info);

#endif
/* __TASKMANAGER_LOGIC_H__ */
