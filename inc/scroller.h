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

/**
 * @defgroup Scroller Scroller
 */

/**
 * @addtogroup Scroller
 * @{
 */

/**
 * @brief Pushes new item into the scroller.
 *
 * @param scroller The Evas_Object for the scroller
 * @param item The object of the item
 *
 * @return task_mgr_error_e type value.
 */
extern task_mgr_error_e scroller_push_item(Evas_Object *scroller, Evas_Object *item);

/**
 * @brief Pops item from the scroller.
 *
 * @param scroller The Evas_Object for the scroller
 * @param item The object of the item
 * @param terminate Termination request
 */
extern void scroller_pop_item(Evas_Object *scroller, Evas_Object *item, int terminate);

/**
 * @brief Pushes all application list items into the scroller.
 *
 * @param list Currently running application list
 * @param scroller The Evas_Object for the scroller
 */
extern task_mgr_error_e scroller_push_all_item(Evas_Object *scroller, Eina_List *list);

/**
 * @brief Pops all application list items from the scroller.
 *
 * @param scroller The Evas_Object for the scroller
 * @param terminate Termination request
 */
extern void scroller_pop_all_item(Evas_Object *scroller, int terminate);

/**
 * @brief Gets number of items in scroller.
 *
 * @param scroller The Evas_Object of the scroller
 */
extern int scroller_count(Evas_Object *scroller);

/**
 * @brief Gets current scroller scrolling state.
 *
 * @param scroller The Evas_Object of the scroller
 */
extern Eina_Bool scroller_is_scrolling(Evas_Object *scroller);

/**
 * @brief Freezes the scroller.
 *
 * @param scroller The Evas_Object of the scroller
 */
extern void scroller_freeze(Evas_Object *scroller);

/**
 * @brief Unfreezes the scroller.
 *
 * @param scroller The Evas_Object of the scroller
 */
extern void scroller_unfreeze(Evas_Object *scroller);

/**
 * @brief Creates scroller.
 *
 * @param layout Parent of the scroller.
 */
extern Evas_Object *scroller_create(Evas_Object *layout);

/**
 * @brief Destroys scroller.
 *
 * @param scroller The Evas_Object of the scroller
 */
extern void scroller_destroy(Evas_Object *scroller);

/**
 * @}
 */

#endif //__TASK_MGR_SCROLLER_H__
