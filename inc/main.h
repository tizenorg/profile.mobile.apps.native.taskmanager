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
 * @mainpage Task Manager documentation
 *
 * @section desc Detailed description
 *
 * @image html taskmanager.png "List of the Task Manager applications"
 * \n
 * @image html taskmanager_no.png "No application could be found"
 *
 * Task Manager is application that provides application lifecycle management
 * functionality and is launched on device home button longpress. It allows
 * relaunching or killing currently running and lets see list of all lately
 * used applications in launch time order.
 *
 * This application is made up the scroller and box of the items. Every item
 * consists of icon and name of the application. The scroller might be scrolled
 * up and down depending on the length of the list of items. If the list is
 * longer than screen height, scroller is enabled to be scrolled otherwise box
 * is aligned to the bottom edge of the screen. The first item of the list is
 * clear item. The clear item does not represent any of the installed on the
 * device applications. The purpose of it is to provide instant kill of all
 * currently running applications. Clicking on the clear item icon will shut
 * down all applications and clear the whole list leaving it empty. Each of
 * displayed application might be killed separately. Task Manager listens on
 * user’s swipe gesture. If user puts finger on application’s icon and swipes
 * it horizontally, it will shut down related application and the swiped icon
 * will be removed from the list. To launch any of the related with item application,
 * it is required to simple click on it. Task Manager loads application list on
 * launch and shows proper information if no application is found.
 *
 * External applications can’t effect on Task Manager directly.However external
 * collaboration with application manager API, application info API and rua
 * database may effect on Task Manager. For example calling rua_clear_history()
 * function will clear the whole rua history, and Task Manager will not be able
 * to load the list of items in launch time order.
 *
 * @subsection adding Enabling application to be shown
 *
 * Every developer may decide whether his application should or should not be
 * shown on Task Manager’s list. To disable application management by Task Manager
 * it is needed to set in tizen-manifest.xml file in ui-application section flag
 * taskmanage=”false” as below:
 *
 * @code
 *
 * <ui-application appid="org.tizen.task-mgr" exec="task-mgr" hw-acceleration="on"
 * 		process-pool="true" multiple="false" nodisplay="true"<b>
 * 		taskmanage="false"</b> type="capp">
 * 	<label>task-mgr</label>
 * 	<icon>taskmanager.png</icon>
 * </ui-application>
 * @endcode
 * Setting taskmanage flag to “true” provides opposite behavior.
 *
 * @subsection data Data
 *
 * Task Manager reads all records from rua history database. After that selects
 * all manageable applications and from received app_info_h handler, gets required
 * icon path and name of the applicaiton.
 *
 */

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
