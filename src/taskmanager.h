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




#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

#include <Elementary.h>
#include <Ecore_X.h>
#include <utilX.h>
#include <bundle.h>

#if !defined(PACKAGE)
#define PACKAGE "taskmanager"
#endif

#ifndef PREFIX
#define PREFIX "/usr/apps/org.tizen."PACKAGE
#endif

#if !defined(RESDIR)
#	define RESDIR PREFIX"/res"
#endif

#if !defined(LOCALEDIR)
#define LOCALEDIR RESDIR"/locale"
#endif

#if !defined(EDJDIR)
#define EDJDIR RESDIR"/edje"PACKAGE
#endif

#if !defined(IMAGEDIR)
#	define IMAGEDIR RESDIR"/images/"PACKAGE
#endif

#define EDJ_NAME EDJDIR"/taskmgr.edj"
#define EDJ_THEME EDJDIR"/theme_taskmanager.edj"
#define GRP_TM "task_manager"

#define S_(str) dgettext("sys_string", str)
#define T_(str) dgettext(PACKAGE, str)

#define _BUF_MAX	256
#define _EDJ(x)	elm_layout_edje_get(x)

#define POPUP_TIMER 1.0
#define POPUP_TERMINATE_TIMER 1.5

struct appdata {
	Evas *evas;
	Evas_Object *win, *ly, *nv, *gl;

	Evas_Coord root_w, root_h;

	Eina_List *applist[2];
	/* runapp : 0, history: 1 */

	Ecore_Timer *popup_timer;
	Evas_Object *popup_ask;
	Evas_Object *popup_progressbar;

	Ecore_Timer *update_timer;
	Ecore_Timer *exit_timer;

	double mem_total;

	int mode;
	int ending;
	int endcnt;
};

struct _task_info {
	char *app_name;
	char *pkg_name;
	char *icn_path;
	pid_t pid;
	struct appdata *ad;
	double mem, mem_total;
	double cpu;
	Elm_Object_Item *it;
	int category;
	bundle *b;
	unsigned int oldutime, oldstime;
	struct timeval oldtimev;
};

	/* MODE_KILL_INUSE = MODE_END_INUSE * 2
	 * MODE_KILL_ALL_INUSE = MODE_END_ALL_INUSE * 2
	 */
enum task_mode {
	MODE_NONE = 0,
	MODE_END_INUSE,
	MODE_KILL_INUSE,
	MODE_END_ALL_INUSE,
	MODE_DEL_HISTORY,
	MODE_DEL_ALL_HISTORY,
	MODE_KILL_ALL_INUSE,
};

enum task_status {
	TS_INUSE = 0,
	TS_HISTORY,
	TS_MAX,
};

Evas_Object *load_edj(Evas_Object *parent, const char *file, const char *group);
int _unset_notification_level(Evas_Object *win);
int _set_notification_level(Evas_Object *win, Utilx_Notification_Level level);
Eina_Bool _exit_cb(void *data);

#endif
/* __TASKMANAGER_H___ */
