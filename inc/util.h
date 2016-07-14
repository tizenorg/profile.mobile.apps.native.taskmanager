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

 #ifndef __TASK_MGR_UTIL_H__
 #define __TASK_MGR_UTIL_H__



/* data key */
#define DATA_KEY_RUNNING_LIST "rn_list"
#define DATA_KEY_IS_SCROLLING "is_scing"
#define DATA_KEY_ITEM_INFO "it_if"

/* Multi-language */
#if !defined(_)
#define _(str) gettext(str)
#endif

/**
 * @defgroup Utils Utilities
 */

/**
 * @addtogroup Utils
 * @{
 */

/**
 * @brief Enumerations for Task Manager Error.
 */
typedef enum {
	TASK_MGR_ERROR_NONE = 0,
	TASK_MGR_ERROR_FAIL = -1,
	TASK_MGR_ERROR_DB_FAILED = -2,
	TASK_MGR_ERROR_OUT_OF_MEMORY = -3,
	TASK_MGR_ERROR_INVALID_PARAMETER = -4,
	TASK_MGR_ERROR_NO_DATA = -5,
} task_mgr_error_e;

/**
 * @brief Enumerations for resource directories.
 */
typedef enum {
	APP_DIR_DATA = 0,
	APP_DIR_CACHE,
	APP_DIR_RESOURCE,
	APP_DIR_SHARED_DATA,
	APP_DIR_SHARED_RESOURCE,
	APP_DIR_SHARED_TRUSTED,
	APP_DIR_EXTERNAL_DATA,
	APP_DIR_EXTERNAL_CACHE,
	APP_DIR_EXTERNAL_SHARED_DATA,
} app_subdir;

/**
 * @brief Shutdowns given currently running application.
 *
 * @param appid The ID of the application
 *
 * @return app_manager_error_e type value or EINA_FALSE on failure
 */
extern Eina_Bool util_kill_app(const char *appid);

/**
 * @brief Resumes the given application.
 *
 * @param appid The ID of the application
 *
 * @return EINA_TRUE or EINA_FALSE on failure
 */
extern Eina_Bool util_launch_app(const char *appid);

/**
 * @brief Returns absolute path to resource file located in applications directory
 *
 * @param dir The type of directory
 * @param relative The relative path to resource starting from given sub directory
 *
 * @return absolute path string
 *
 * @note returns statically allocated string
 */
extern const char *util_get_file_path(app_subdir dir, const char *relative);

/**
 * @}
 */

#endif //__TASK_MGR_UTIL_H__
