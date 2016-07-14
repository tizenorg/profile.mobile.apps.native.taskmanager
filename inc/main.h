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

#ifndef __TASK_MGR_H__
#define __TASK_MGR_H__

/**
 * @defgroup Main Main
 */

/**
 * @addtogroup Main
 * @{
 */

/**
 * @brief Task Manager main info structure.
 */
typedef struct {
	Evas        *e;
	Evas_Object *win;
	Evas_Object *layout;
	Evas_Object *scroller;
	Evas_Object *box;

	Eina_List *pkg_list;

	int root_w;
	int root_h;
} main_s;
typedef main_s *main_h;

/**
 * @brief Gets application main information.
 *
 * @return The main application information handle
 */
extern main_h main_get_info(void);

/**
 * @}
 */

#endif //__TASK_MGR_H__
