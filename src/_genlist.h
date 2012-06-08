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


#ifndef __TASKMANAGER_GENLIST_H__
#define __TASKMANAGER_GENLIST_H__

#include <Elementary.h>
#include "taskmanager.h"

Eina_Bool alert_app_info(void *data);
void clear_task_manager_list(void *data);
void refresh_app_info(struct appdata *ad);
void load_task_manager_list(struct appdata *ad);
void _set_itc(void);
void _set_genlist(struct appdata *ad);
void _del_popup_timer(struct appdata *ad);
int response_end_inuse(struct appdata *ad);
int response_end_all_inuse(struct appdata *ad);
int response_del_history(struct appdata *ad);
int response_del_all_history(struct appdata *ad);
int response_kill_inuse(struct appdata *ad);
int response_kill_all_inuse(struct appdata *ad);
void _fini_pthread(void);
Eina_Bool _update_list(void *data);
void _restart_pthread(struct appdata *ad);

#endif
/* __TASKMANAGER_GENLIST_H__ */
