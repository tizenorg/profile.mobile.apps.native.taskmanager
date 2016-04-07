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

#ifndef __TASK_MGR_SCROLLER_H__
#define __TASK_MGR_SCROLLER_H__

#include "util.h"



extern task_mgr_error_e scroller_push_item(Evas_Object *scroller, Evas_Object *item);
extern void scroller_pop_item(Evas_Object *scroller, Evas_Object *item, int terminate);

extern task_mgr_error_e scroller_push_all_item(Evas_Object *scroller, Eina_List *list);
extern void scroller_pop_all_item(Evas_Object *scroller, int terminate);

extern int scroller_count(Evas_Object *scroller);
extern Eina_Bool scroller_is_scrolling(Evas_Object *scroller);

extern void scroller_freeze(Evas_Object *scroller);
extern void scroller_unfreeze(Evas_Object *scroller);

extern Evas_Object *scroller_create(Evas_Object *layout);
extern void scroller_destroy(Evas_Object *scroller);



#endif //__TASK_MGR_SCROLLER_H__
