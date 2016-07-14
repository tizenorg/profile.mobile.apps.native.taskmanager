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

#ifndef __TASK_MGR_ITEM_H__
#define __TASK_MGR_ITEM_H__

#include "list.h"
#include "util.h"

/**
 * @defgroup Item Item
 */

/**
 * @addtogroup Item
 * @{
 */

/**
 * @brief Gets application item data.
 *
 * @param item The item of the data to be get
 *
 * @return list_type_default_s or NULL on failure
 */
extern list_type_default_s *item_get_info(Evas_Object *item);

/**
 * @brief Sets clear all item disabled.
 *
 * @param scroller The object of the scroller
 */
extern void item_clear_all_disabled_set(Evas_Object *scroller);

/**
 * @brief Creates clear all item.
 *
 * @param scroller The object of the scroller
 */
extern Evas_Object *item_clear_all_create(Evas_Object *scroller);

/**
 * @brief Destroys clear all item.
 *
 * @param scroller The object of the scroller
 */
extern void item_clear_all_destroy(Evas_Object *scroller);

/**
 * @brief Creates new applications item.
 *
 * @param scroller The object of the scroller
 * @param info Pointer to the info structure of the item
 *
 * @return The Evas_Object of the item
 */
extern Evas_Object *item_create(Evas_Object *scroller, list_type_default_s *info);

/**
 * @brief Destroys applications item.
 *
 * @param item The item to be destroyed
 */
extern void item_destroy(Evas_Object *item);

/**
 * @brief Terminates application related with item.
 *
 * @param item The item of the application to be terminated
 */
extern void item_terminate(Evas_Object *item);

/**
 * @}
 */


#endif //__TASK_MGR_ITEM_H__
