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

#include <app.h>
#include <Elementary.h>
#include <malloc.h>

#include "conf.h"
#include "item.h"
#include "list.h"
#include "log.h"
#include "main.h"
#include "scroller.h"
#include "util.h"

#define KEY_BACK "XF86Back"
#define KEY_HOME "XF86Home"
#define PRIVATE_DATA_KEY_LIST_TIMER "pri_list_tm"

static main_s main_info = {
	.e = NULL,
	.win = NULL,
	.layout = NULL,
	.scroller = NULL,
	.box = NULL,
	.pkg_list = NULL,

	.root_w = 0,
	.root_h = 0,
};
/* main_h main_info_h = &main_info; */



main_h main_get_info(void)
{
	return &main_info;
}



static Eina_Bool _key_pressed(void *data, Evas_Object *obj, Evas_Object *src, Evas_Callback_Type type, void *event_info)
{
	Evas_Event_Key_Down *ev = event_info;

	if (type == EVAS_CALLBACK_KEY_DOWN && ((strncmp(KEY_BACK, ev->key, strlen(KEY_BACK)) == 0) ||
			(strncmp(KEY_HOME, ev->key, strlen(KEY_HOME)) == 0))) {
		_D("KEY PRESSED: %s", ev->key);

		elm_exit();
		return EINA_TRUE;
	} else {
		return EINA_FALSE;
	}
}



static task_mgr_error_e _create_layout(Evas_Object *parent)
{
	_D("");
	Evas_Object *layout = NULL;
	Eina_Bool ret = EINA_FALSE;

	/* create layout */
	layout = elm_layout_add(parent);
	goto_if(!layout, ERROR);

	ret = elm_layout_file_set(layout, util_get_file_path(APP_DIR_RESOURCE, LAYOUT_EDJ), "layout");
	goto_if(EINA_FALSE == ret, ERROR);

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_move(layout, 0, 0);
	evas_object_resize(layout, main_info.root_w, main_info.root_h);
	evas_object_show(layout);

	main_info.layout = layout;

	/* create scroller */
	main_info.scroller = scroller_create(layout);
	goto_if(!main_info.scroller, ERROR);

	return TASK_MGR_ERROR_NONE;

ERROR:
	if (layout) evas_object_del(layout);
	evas_object_del(main_info.win);
	return TASK_MGR_ERROR_FAIL;
}



static void _destroy_layout(void)
{
	if (main_info.layout) {
		evas_object_del(main_info.layout);
		main_info.layout = NULL;
	}
}



static Eina_Bool _list_timer_cb(void *data)
{
	_D("");
	Evas_Object *clear_item = NULL;
	task_mgr_error_e ret = TASK_MGR_ERROR_NONE;

	if (main_info.pkg_list) {
		_D("Already loaded");
		goto END;
	}

	ret = list_init();
	if (ret != TASK_MGR_ERROR_NONE) {
		_E("Could not initialize list module");
		goto END;
	}

	elm_object_part_text_set(main_info.layout, "no,apps,txt", _("IDS_TASKMGR_NPBODY_NO_APPLICATIONS_ABB2"));

	main_info.pkg_list = list_pkg_list_get();
	if (!main_info.pkg_list) {
		_E("pkg_list is empty");
		elm_layout_signal_emit(main_info.layout, "no,apps,txt,show", "no,apps,txt");
		goto END;
	}

	clear_item = item_clear_all_create(main_info.scroller);
	if (!clear_item) {
		_E("Fail to create clear all button");
		return ECORE_CALLBACK_CANCEL;
	}
	scroller_push_item(main_info.scroller, clear_item);

	if (TASK_MGR_ERROR_NO_DATA == ret) {
		_D("There is no application");
		item_clear_set_disable(main_info.scroller);
	} else if (TASK_MGR_ERROR_NONE != ret) {
		_E("Fail to create pkglist");
		goto END;
	}

	goto_if(TASK_MGR_ERROR_NONE != scroller_push_all_item(main_info.scroller, main_info.pkg_list), END);

END:
	evas_object_data_del(main_info.win, PRIVATE_DATA_KEY_LIST_TIMER);
	return ECORE_CALLBACK_CANCEL;
}



static bool _create_cb(void *data)
{
	_D("");

	Ecore_Timer *timer = NULL;

	main_info.win = elm_win_add(NULL, "Task-mgr", ELM_WIN_BASIC);
	retv_if(!main_info.win, false);

	elm_win_screen_size_get(main_info.win, NULL, NULL, &main_info.root_w, &main_info.root_h);
	_D("screen size is (%d, %d)", main_info.root_w, main_info.root_h);

	elm_object_event_callback_add(main_info.win, _key_pressed, NULL);

	elm_win_indicator_mode_set(main_info.win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(main_info.win, ELM_WIN_INDICATOR_TRANSPARENT);

	elm_win_borderless_set(main_info.win, EINA_TRUE);
	elm_win_alpha_set(main_info.win, EINA_TRUE);
	evas_object_show(main_info.win);

	main_info.e = evas_object_evas_get(main_info.win);

	if (_create_layout(main_info.win) != TASK_MGR_ERROR_NONE) {
		_E("Failed to create a layout");
		return false;
	}

	/* create the list */
	timer = evas_object_data_get(main_info.win, PRIVATE_DATA_KEY_LIST_TIMER);
	if (timer) {
		ecore_timer_del(timer);
	}

	timer = ecore_timer_add(0.001f, _list_timer_cb, NULL);
	if (timer) {
		evas_object_data_set(main_info.win, PRIVATE_DATA_KEY_LIST_TIMER, timer);
	} else {
		_E("Cannot add a create list timer");
	}

	return true;
}



static void _terminate_cb(void *data)
{
	_D("");

	/* list destroy */
	list_destroy(main_info.pkg_list);
	evas_object_data_del(main_info.win, PRIVATE_DATA_KEY_LIST_TIMER);

	elm_cache_all_flush();
	malloc_trim(0);

	scroller_destroy(main_info.layout);
	_destroy_layout();

	/**
	 * Even though the window is deleted automatically,
	 * It is good habit to delete window explicitly by your hands.
	 */

	if (main_info.win) {
		evas_object_del(main_info.win);
		main_info.win = NULL;
	}
}



static void _app_control(app_control_h service, void *data)
{
	_D("");

	return;
}



static void _pause_cb(void *data)
{
	_D("");

	elm_exit();
	return;
}



static void _language_changed(app_event_info_h event_info, void *data)
{
	_D("");
}



int main(int argc, char **argv)
{
	int ret;
	ui_app_lifecycle_callback_s lifecycle_callback = {0, };
	app_event_handler_h event_handlers[5] = {NULL, };

	lifecycle_callback.create = _create_cb;
	lifecycle_callback.terminate = _terminate_cb;
	lifecycle_callback.pause = _pause_cb;
	lifecycle_callback.resume = NULL;
	lifecycle_callback.app_control = _app_control;

	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _language_changed, NULL);
	ui_app_add_event_handler(&event_handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);

	ret = ui_app_main(argc, argv, &lifecycle_callback, &main_info);

	return ret;
}

/* End of a file */
