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

#include <stdbool.h>
#include <Eina.h>
#include "util.h"

/**
 * @defgroup List List
 */

/**
 * @addtogroup List
 * @{
 */

/**
 * @brief Structure of the list item info.
 */
typedef struct {
	char *pkgid;
	char *appid;
	char *name;
	char *icon;

	int launch_time;

} list_type_default_s;

/**
 * @brief Gets currently running applications list.
 *
 * @return List of the currently running applications.
 */
extern Eina_List *list_pkg_list_get(void);

/**
 * @brief Initializes application list.
 *
 * @return TASK_MANAGER_ERROR_NONE on success,
 * 		otherwise a negative error value
 */
extern task_mgr_error_e list_init(void);

/**
 * @brief Destroys application list.
 *
 * @param list The list to be destroyed
 */
extern void list_destroy(Eina_List *pkg_list);

/**
 * @}
 */

#endif //__TASK_MGR_LIST_H__
