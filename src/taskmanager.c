/*
 * org.tizen.taskmgr
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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




#include <stdio.h>
#include <unistd.h>
#include <appcore-efl.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include <utilX.h>
#include <vconf.h>
#include <aul.h>
#include <sysman.h>

#include "taskmanager.h"
#include "_util_log.h"
#include "_util_efl.h"
#include "_logic.h"
#include "_genlist.h"

struct text_part {
	char *part;
	char *msgid;
};

enum {
	IDLELOCK_OFF = 0x0,
	IDLELOCK_ON,
	IDLELOCK_MAX,
};

static struct text_part main_txt[] = {
};

static void update_ts(Evas_Object *eo, struct text_part *tp, int size)
{
	int i;

	if (eo == NULL || tp == NULL || size < 0)
		return;

	for (i = 0; i < size; i++) {
		if (tp[i].part && tp[i].msgid)
			edje_object_part_text_set(eo,
						  tp[i].part, _(tp[i].msgid));
	}
}

static int _lang_changed(void *data)
{
	struct appdata *ad = data;

	if (ad->ly == NULL)
		return 0;

	update_ts(elm_layout_edje_get(ad->ly),
		  main_txt, sizeof(main_txt) / sizeof(main_txt[0]));

	return 0;
}

static int rotate(enum appcore_rm m, void *data)
{
	struct appdata *ad = data;
	int r;

	if (ad == NULL || ad->win == NULL)
		return 0;

	switch (m) {
	case APPCORE_RM_PORTRAIT_NORMAL:
		r = 0;
		break;
	case APPCORE_RM_PORTRAIT_REVERSE:
		r = 180;
		break;
	case APPCORE_RM_LANDSCAPE_NORMAL:
		r = 270;
		break;
	case APPCORE_RM_LANDSCAPE_REVERSE:
		r = 90;
		break;
	default:
		r = -1;
		break;
	}

	if (r >= 0)
		elm_win_rotation_with_resize_set(ad->win, r);

	return 0;
}

int _get_vconf_idlelock(void)
{
	int ret = -1;
	int lock = IDLELOCK_OFF;

	ret = vconf_get_int(VCONFKEY_IDLE_LOCK_STATE, &lock);
	retvm_if(ret < 0, -1, "Failed to get vconf %s\n",
		 VCONFKEY_IDLE_LOCK_STATE);
	_D("idlelock vconf:%d\n", lock);

	return lock == VCONFKEY_IDLE_LOCK ? IDLELOCK_ON : IDLELOCK_OFF;
}

void _exit_cb(void *data)
{
	struct appdata *ad = (struct appdata *)data;
	int lock = IDLELOCK_ON;
	lock = _get_vconf_idlelock();

	_D("lock(%d)\n", lock);
	if(lock == IDLELOCK_OFF){
		_D("normal case\n");
		elm_exit();
	}
	else{
		_D("IDLELOCK is set, taskmnager doesn't exit\n");
	}
}

void _key_grab(struct appdata *ad)
{
	int ret = 0;
	Ecore_X_Window xwin;	/* key grab */
	Ecore_X_Display *disp;	/* key grab */

	/* Key Grab */
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get(ad->win);

	ret = utilx_grab_key(disp, xwin, KEY_SELECT, SHARED_GRAB);
	retm_if(ret < 0, "Failed to grab home key\n");
}

int _set_launch_effect(Evas_Object *win)
{
	Ecore_X_Window xwin = 0;
	static Ecore_X_Atom ATOM_WM_WINDOW_ROLE = 0;
	static Ecore_X_Atom ATOM_NET_WM_NAME = 0;
	retvm_if(win == NULL, -1, "[Error] Invalid argument: win is NULL\n");

	ATOM_WM_WINDOW_ROLE = ecore_x_atom_get("WM_WINDOW_ROLE");
	if (!ATOM_WM_WINDOW_ROLE) {
		fprintf(stderr,
			"[App] %s(%d) XInternAtom(WM_WINDOW_ROLE) failed.\n",
			__func__, __LINE__);
	}

	ATOM_NET_WM_NAME = ecore_x_atom_get("_NET_WM_NAME");
	if (!ATOM_NET_WM_NAME) {
		fprintf(stderr,
			"[App] %s(%d) XInternAtom(ATOM_NET_WM_NAME) failed.\n",
			__func__, __LINE__);
	}

	xwin = elm_win_xwindow_get(win);
	ecore_x_window_prop_string_set(xwin, ATOM_WM_WINDOW_ROLE,
				       "TASK_MANAGER");
	ecore_x_window_prop_string_set(xwin, ATOM_NET_WM_NAME, "TASK_MANAGER");

	ecore_x_icccm_name_class_set(xwin, "TASK_MANAGER", "TASK_MANAGER");
	return 0;
}

int _unset_notification_level(Evas_Object *win)
{
	Ecore_X_Window xwin;

	xwin = elm_win_xwindow_get(win);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NORMAL);
	return 0;
}


int _set_notification_level(Evas_Object *win, Utilx_Notification_Level level)
{
	Ecore_X_Window xwin = 0;

	xwin = elm_win_xwindow_get(win);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(ecore_x_display_get(), xwin, level);
	return 0;
}

int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;
	int r;

	win = _add_window(PACKAGE);
	retv_if(win == NULL, -1);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_SHOW);
	ad->win = win;

	_set_launch_effect(win);
//	_set_notification_level(win, UTILX_NOTIFICATION_LEVEL_NORMAL);

	/* init internationalization */
	r = appcore_set_i18n(PACKAGE, LOCALEDIR);
	retvm_if(r < 0, -1, "Failed to set i18n\n");
	_lang_changed(ad);


	elm_theme_extension_add(NULL, EDJ_THEME);

	_app_create(ad);
	_set_itc();
	_set_genlist(ad);

	/* set dead signal listener */
	aul_listen_app_dead_signal(_dead_cb, ad);

	appcore_set_event_callback(APPCORE_EVENT_LANG_CHANGE,
			_lang_changed, ad);

	_restart_pthread(ad);
	ecore_idler_add(_create_idler_cb, ad);

	return 0;
}

static int app_terminate(void *data)
{
_D("func\n");
	struct appdata *ad = data;
//	sleep(1);
//	ecore_timer_add(0.2, _exit_cb, NULL);

	return 0;
}

static int app_pause(void *data)
{
_D("func\n");
	_fini_pthread();
	return 0;
}

static int app_resume(void *data)
{
_D("func\n");
	struct appdata *ad = data;

	refresh_app_info(ad);
	if (ad->exit_timer) {
		ecore_timer_del(ad->exit_timer);
		ad->exit_timer = NULL;
	}

	return 0;
}

static int app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;

	/* appcore measure time example */
	printf("from AUL to %s(): %d msec\n", __func__,
			appcore_measure_time_from("APP_START_TIME"));
	printf("from create to %s(): %d msec\n", __func__,
			appcore_measure_time());

	if (ad->win)
		elm_win_activate(ad->win);
	return 0;
}

int main(int argc, char *argv[])
{
	sysconf_set_mempolicy(OOM_IGNORE);

	struct appdata ad;
	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};

	/* appcore measure time example */
	_D("from AUL to %s(): %d msec\n", __func__,
	       appcore_measure_time_from("APP_START_TIME"));

	memset(&ad, 0x0, sizeof(struct appdata));
	ops.data = &ad;

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
